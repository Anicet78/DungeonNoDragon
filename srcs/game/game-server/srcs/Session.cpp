# include "Session.hpp"

Session::Session(void): _maxNumPlayer(2), _running(0), _ended(0), _startTime(std::chrono::steady_clock::time_point{}), _countDown(std::chrono::steady_clock::time_point{}),
						_numPlayersFinished(0), _readyToRun(0), _timerBeforeRun(std::chrono::_V2::steady_clock::now()), _readyToRunStartTimer(0.0f)
{
	static std::string set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int size = static_cast<int>(2 * sqrt(8 + 5 * (_maxNumPlayer - 1)));

	for (int i = 0; i < 25; i++)
	{
		int nb = rand() % 62;
		this->_sessionId.push_back(set[nb]);
	}

	_separedSpace.emplace_back(3, 3, this->_sessionId);
	_separedSpace.emplace_back(3, 3, this->_sessionId);
	
	_maps.emplace_back(1, 1, this->_sessionId);
	_maps.back().setWaitingRoom();
	_maps.emplace_back(size, size, this->_sessionId);
	_maps.back().fillMap(_maxNumPlayer, 0);
	_maps.emplace_back(size * 0.8, size * 0.8, this->_sessionId);
	_maps.back().fillMap(_maxNumPlayer, 1);
	printMap(_maps[1]);
	printMap(_maps[2]);
	this->linkMaps(_maps[1], _maps[2]);
}

Session::Session(int numPLayer):	_maxNumPlayer(numPLayer), _running(0), _ended(0), _startTime(std::chrono::steady_clock::time_point{}), _countDown(std::chrono::steady_clock::time_point{}),
									_numPlayersFinished(0), _readyToRun(0), _timerBeforeRun(std::chrono::_V2::steady_clock::now()), _readyToRunStartTimer(0.0f)
{
	static std::string set = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int size = static_cast<int>(2 * sqrt(8 + 5 * (_maxNumPlayer - 1)));

	for (int i = 0; i < 25; i++)
	{
		int nb = rand() % 62;
		this->_sessionId.push_back(set[nb]);
	}

	_maps.reserve(3);

	_maps.emplace_back(1, 1, this->_sessionId);
	_maps.back().setWaitingRoom();
	_maps.emplace_back(size, size, this->_sessionId);
	_maps.back().fillMap(_maxNumPlayer, 0);
	_maps.emplace_back(size * 0.8, size * 0.8, this->_sessionId);
	_maps.back().fillMap(_maxNumPlayer, 1);
	printMap(_maps[1]);
	printMap(_maps[2]);
	this->linkMaps(_maps[1], _maps[2]);
}

Session::~Session(void)
{
	std::cout << "session " << this->_sessionId << " has ended !!" << std::endl;
}

//-----------------------------------------------------------------

void	Session::launch()
{
	quadList start, vide;
	this->_running = true;
	size_t pos = 0;
	this->_startTime = std::chrono::_V2::steady_clock::now();
	for (quadList &node : this->_maps[1].getNodes())
	{
		if (!node->getRoom() || node->getRoom()->getName() != "start")
			continue ;
		if (pos >= this->_players.size())
			break ;
		if (this->_players[pos].expired())
		{
			pos++;
			continue ;
		}
		std::shared_ptr<Player> player = this->_players[pos].lock();
		std::string roomId = player->getRoom().getRoomId();
		player->setNode(node);

		player->setStartNode(node);

		player->setStartPos(pos);

		if (player->isReConnected())
		{
			player->getWs()->unsubscribe(roomId);
			player->getWs()->subscribe(node->getRoom()->getRoomId());
		}
		pos++;
	}
}

void	Session::sendToAll(Player &sender)
{
	for (auto &player : _players)
	{
		if (player.expired())
			continue ;
		if (sender.getUid() == player.lock()->getUid())
			continue ;
		sendPlayerState(*player.lock(), *this, sender.getUid());
	}
}

void	Session::linkMaps(Map &down, Map &up)
{
	down.link(up);
}

void	putRoomEvent(std::string &msg, std::shared_ptr<Room> room)
{
	msg += ", \"room_event\": \"" + room->getRoomEvent()->getType() + '\"';
	if (room->getRoomEvent()->getType() == "MobRush")
	{
		MobRush &event = dynamic_cast<MobRush &>(*room->getRoomEvent());
		std::unordered_map<int, std::unique_ptr<Mob>> &mobs = event.getMobs();
		if (mobs.size())
		{
			msg += ", \"nbr_mob\": " + std::to_string(mobs.size());
			msg += ", \"mobs\": [";
			for (auto it = mobs.begin(); it != mobs.end(); ++it)
			{
				Mob	&mob = *it->second;
				msg += "{\"mob_id\": " + std::to_string(it->first) + ", "
					+ "\"mob_x\": " + std::to_string(mob.getX()) + ", "
					+ "\"mob_y\": " + std::to_string(mob.getY()) + "},";
			}
			if (*msg.rbegin() == ',')
				msg.pop_back();
			msg += "]";
		}
	}
}

std::string	Session::sendMaps(void)
{
	if (!this->_mapInfos.empty())
		return this->_mapInfos;

	std::string msg = "{\"action\": \"waiting\", \"session_id\": \"" + this->_sessionId + "\", \"maps\": { ";
	for (size_t i = 0; i < this->_maps.size(); i++)
	{
		if (!i)
			msg += "\"waiting_map\": { ";
		else
			msg += ", \"floor_" + std::to_string(i - 1) + "\": { ";
		msg += "\"size_x\": " + std::to_string(this->_maps[i].getWidth()) + ", "
			+ "\"size_y\": " + std::to_string(this->_maps[i].getHeight()) + ", "
			+ "\"rooms\": [";
		int j = 0;
		for (quadList &node : this->_maps[i].getNodes())
		{
			if (!node->getRoom())
				continue ;
			auto room = node->getRoom();
			if (j)
				msg += ", ";
			msg += "{\"name\": \"" + room->getName() + "\", "
				+ "\"x\": " + std::to_string(node->getX()) + ", "
				+ "\"y\": " + std::to_string(node->getY()) + ", "
				+ "\"rot\": " + std::to_string(room->getRotated());
			if (room->getRoomEvent())
				putRoomEvent(msg, room);
			msg += '}';
			j++;
		}
		msg += "], \"nb_rooms\": " + std::to_string(j) + "}";
	}
	msg += "}}";
	this->_mapInfos = msg;
	return this->_mapInfos;
}

void	Session::addParty(Party &newParty)
{
	std::string msg;
	for (std::weak_ptr<Player> &player : newParty.getPlayers())
	{
		if (player.expired() || !player.lock()->isReConnected())
			continue ;
		auto p = player.lock();
		p->setNode(this->_maps[0].getNodes()[0]);
		if (p->getWs()->subscribe(p->getRoom().getRoomId()))
			std::cout << "added to the waiting room" << std::endl;
		this->_players.push_back(player);
		msg = this->sendMaps();
		p->getWs()->send(msg, uWS::OpCode::TEXT);
        p->getWs()->send("You have been added to a session !", uWS::OpCode::TEXT);
	}
}

bool	Session::removePlayer(std::weak_ptr<Player> rmPlayer)
{
	for (size_t i = 0; i < this->_players.size(); i++)
	{
		if (this->_players[i].expired())
			continue ;
		if (this->_players[i].lock()->getUid() == rmPlayer.lock()->getUid())
		{
			this->_players.erase(this->_players.begin() + i);
			return 1;
		}
	}
	return 0;
}

bool	Session::removePlayer(std::string uid)
{
	for (size_t i = 0; i < this->_players.size(); i++)
	{
		if (this->_players[i].expired())
			continue ;
		if (this->_players[i].lock()->getUid() == uid)
		{
			this->_players.erase(this->_players.begin() + i);
			return 1;
		}
	}
	return 0;
}

std::vector<std::weak_ptr<Player>>	Session::getPlayers() const
{
	return this->_players;
}

std::weak_ptr<Player> &Session::getPlayer(std::string &uid)
{
	for (auto &player : _players)
	{
		if (player.expired())
			continue ;
		if (player.lock()->getUid() == uid)
		{
			return player;
			break ;
		}
	}
	return _players[0];
}

void	Session::startCountDown(void)
{
	this->_countDown = std::chrono::steady_clock::now();
}

int	Session::getCountDown(void) const
{
	if (_countDown == std::chrono::steady_clock::time_point{})
		return 6;
	return std::ceil(5 - std::chrono::duration<double>(std::chrono::steady_clock::now() - this->_countDown).count());
}

double	Session::getActualTime(void) const
{
	if (_startTime == std::chrono::steady_clock::time_point{})
	    return 0;
	return std::chrono::duration<double>(std::chrono::steady_clock::now() - this->_startTime).count();
}

int	Session::getMaxNumPlayer() const
{
	return this->_maxNumPlayer;
}

int	Session::getPlaceLeft() const
{
	return this->_maxNumPlayer - this->_players.size();
}

int	Session::getNumPlayers() const
{
	return this->_players.size();
}

std::string	const	&Session::getSessionId(void) const
{
	return this->_sessionId;
}

bool	Session::hasEnded() const
{
	return this->_ended;
}

bool	Session::isRunning() const
{
	return this->_running;
}

bool Session::isReadyToRun() const
{
	return this->_readyToRun;
}

bool Session::doesAllPlayersConnected() const
{
	for (auto &player : this->_players)
	{
		if (player.expired())
			continue ;
		if (!player.lock()->isReConnected())
			return false;
	}
	return true;
}

bool	Session::isPlayerInSession(std::string uid) const
{
	for (auto &player : _players)
	{
		if (player.expired())
			continue ;
		if (player.lock()->getUid() == uid)
			return true;
	}
	return false;
}

void	Session::sendEndResults(uWS::App &app, std::shared_ptr<Player> &player, bool abort)
{
	int win = 0;

	if (!abort)
	{
		this->_numPlayersFinished++;
		win = (this->_numPlayersFinished == 1) ? true : false;
	}
	else
		win = false;

	player->setHasWin(win);
	player->setFinalRanking(this->_maxNumPlayer);

	std::string msg = "{ \"action\": \"finished\", \"time\": "
		+ std::to_string(this->getActualTime()) + ", \"win\": "
		+ std::to_string(win) + "}";
	
	player->getWs()->send(msg, uWS::OpCode::TEXT);
	std::string	oldTopic = player->getRoomRef().getRoomId();
	sendLeaveUpdate(*player, app, oldTopic);
	if (player->isReConnected())
	{
		player->getWs()->unsubscribe(oldTopic);
	}
}

void	Session::checkFinishedPlayers(uWS::App &app, Server &server)
{
	int count = 0;

	for (auto &p : this->_players)
	{
		if (p.expired() || !p.lock()->isReConnected())
			continue ;
		std::shared_ptr<Player> player = p.lock();
		if (player->getFinished() && !player->getResultCurl())
		{
			this->sendEndResults(app, player, 0);
			sendPlayerResultCurl(server, *this, *player.get());
		}
		if (player->checkInvinsibleFrame() && player->getTimeInvincible() > 1.0f)
			player->endInvinsibleFrame();
	}

	for (auto player : this->_players)
		if ((!player.expired() && player.lock()->isReConnected()) || (!player.expired() && player.lock()->getTimeDeconnection() < 7.f))
			count++;

	if ((!count && this->_running) || this->getActualTime() > 1200.f)
	{
		std::cout << "SESSION STOP" << std::endl;
		this->_ended = 1;
	}
}

double Session::getActualTimeBeforeRun(void) const
{
	return std::chrono::duration<double>(std::chrono::steady_clock::now() - this->_timerBeforeRun).count();
}

void	Session::startLaunching(void)
{
	if (this->_running || this->_readyToRun)
		return ;
	if (this->getCountDown() >= 0)
	{
		if (this->getCountDown() == 6)
			this->startCountDown();
		return ;
	}
	this->_readyToRun = true;
	this->_readyToRunStartTimer = getActualTimeBeforeRun();
	return ;
}

bool	Session::isEnoughtReadyTime(void) const
{
	if (this->getActualTimeBeforeRun() - this->_readyToRunStartTimer > 1)
		return(true);
	return(false);
}
