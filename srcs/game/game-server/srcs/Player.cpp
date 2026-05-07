#include"Player.hpp"

Player::Player(std::string uid, int partySize, std::string partyId, std::string name, int sessionSize, uWS::WebSocket<false, true, PerSocketData> *ws)
				: _uid(uid), _sessionSize(sessionSize), _partySize(partySize),  _partyId(partyId), _name(name), _inQueue(true), _inSession(false),
					_launched(0), _connected(0), _reConnected(1), _finished(0), _hasWin(0), _nbrDeath(0), _isDead(false),
					_timeDeath(std::chrono::steady_clock::time_point{}), _finalRanking(0), _exit(' '), _timeDeconnection(std::chrono::steady_clock::time_point{}), _ws(ws), _x(0), _y(0),
					_floor(0), _startPos(-1), _anim(0), _last_dir(0), _hp(5), _atk(1), _isInvinsible(false), _timeInvincible(std::chrono::steady_clock::time_point{})/*, _def(0)*/, _box(_x, _y, _last_dir),
					_isAttacking(false), _atkFrame(0), _timeAttack(std::chrono::steady_clock::now()), _kills(0), _resultCurl(0)
{
	_wallHitBox =
	{_x - 0.1f, _y + 0.1f, 0.2f, 0.2f};
	return ;
}

Player::~Player(void)
{}

//get player value

bool Player::isLaunched(void) const
{
	return this->_launched;
}

bool Player::HasWin(void) const
{
	return this->_hasWin;
}

bool Player::getFinished(void) const
{
	return this->_finished;
}

int	Player::getFinalRanking(void) const
{
	return this->_finalRanking;
}

char Player::getExit(void) const
{
	return this->_exit;
}

std::string	Player::getUid(void) const
{
	return (_uid);
}

std::string	Player::getPartyId(void) const
{
	return (_partyId);
}

int Player::getGroupSize() const
{
	return _partySize;
}

std::string	Player::getName(void) const
{
	return (_name);
}

float	Player::getX(void) const
{
	return (_x);
}

float	Player::getY(void) const
{
	return (_y);
}

int		Player::getFloor(void) const
{
	return this->_floor;
}

int		Player::getSessionSize(void) const
{
	return this->_sessionSize;
}

int		Player::getStartPos(void) const
{
	return (_startPos);
}

int		Player::getHp(void) const
{
	return (_hp);
}

int		Player::getAtk(void) const
{
	return (_atk);
}

// int		Player::getDef(void) const
// {
// 	return (_def);
// }

int		Player::getAnim(void) const
{
	return this->_anim;
}

Room	Player::getRoom(void) const
{
	return *this->_node->getRoom().get();
}

Room	&Player::getRoomRef(void)
{
	return *this->_node->getRoom().get();
}

HitBox	&Player::getHitBox(void)
{
	return (_box);
}

double	Player::getTimeDeconnection(void) const
{
	if (this->_timeDeconnection == std::chrono::_V2::steady_clock::time_point{})
		return 0;
	return std::chrono::duration<double>(std::chrono::steady_clock::now() - this->_timeDeconnection).count();
}

double	Player::getTimeInvincible(void) const
{
	if (this->_timeInvincible == std::chrono::_V2::steady_clock::time_point{})
		return 0;
	return std::chrono::duration<double>(std::chrono::steady_clock::now() - this->_timeInvincible).count();
}

double	Player::getTimeAttack(void) const
{
	return	std::chrono::duration<double>(std::chrono::steady_clock::now() - this->_timeAttack).count();
}

bool	Player::checkInvinsibleFrame(void) const
{
	return this->_isInvinsible;
}

FRect	&Player::getWallHitBox(void)
{
	return (this->_wallHitBox);
}

int		Player::getKills(void) const
{
	return (this->_kills);
}

bool	Player::isDead(void) const
{
	return this->_isDead;
}

double	Player::getTimeDeath(void) const
{
	if (this->_timeDeath == std::chrono::steady_clock::time_point{})
		return 0;
	return std::chrono::duration<double>(std::chrono::steady_clock::now() - this->_timeDeath).count();
}

bool Player::isInQueue(void) const
{
	return this->_inQueue;
}

bool Player::isInSession(void) const
{
	return this->_inSession;
}

bool Player::isConnected(void) const
{
	return this->_connected;
}

int	Player::getAtkFrame(void) const
{
	return this->_atkFrame;
}

bool Player::isReConnected(void) const
{
	return this->_reConnected;
}

uWS::WebSocket<false, true, PerSocketData> *Player::getWs() const
{
	return this->_ws;
}

quadList Player::getNode() const
{
	return this->_node;
}

quadList Player::getPrevNode() const
{
	return this->_prev_node;
}

quadList Player::getStartNode() const
{
	return this->_startNode;
}

int Player::getLastDir(void) const
{
	return this->_last_dir;
}

bool Player::getResultCurl(void) const
{
	return this->_resultCurl;
}

//set player value

void	Player::setWs(uWS::WebSocket<false, true, PerSocketData> *ws)
{
	this->_ws = ws;
}

void	Player::setLaunched(bool flag)
{
	this->_launched = flag;
}

void	Player::setReconnexion(bool c)
{
	this->_reConnected = c;
	if (!c)
		this->_timeDeconnection = std::chrono::steady_clock::now();
	else
		this->_timeDeconnection = std::chrono::steady_clock::time_point{};
}

void	Player::setConnexion(bool c)
{
	this->_connected = c;
}

void	Player::setFinished(bool flag)
{
	this->_finished = flag;
}

void	Player::setHasWin(bool flag)
{
	this->_hasWin = flag;
}

void	Player::setFinalRanking(int place)
{
	this->_finalRanking = place;
}

void	Player::setExit(char c)
{
	this->_exit = c;
}

void	Player::setPrevNode(const quadList &node)
{
	this->_prev_node = node;
}

void	Player::setStartNode(const quadList &node)
{
	this->_startNode = node;
}

void	Player::setNode(const quadList &node)
{
	this->_node = node;
	if (node->getRoom()->getName() == "waiting" || node->getRoom()->getName() == "start")
	{
		int i = 0;
		for (auto &line : node->getRoom()->getRoomPlan())
		{
			size_t pos = 0;
			if ((pos = line.find('P')) == std::string::npos)
			{
				i++;
				continue ;
			}
			this->_x = static_cast<int>(pos) + 0.5;
			this->_y = i + 0.5;
			i++;
		}
	}
}

void	Player::setIsDead(bool value)
{
	if (value && !this->_isDead)
		this->_timeDeath = std::chrono::steady_clock::now();
	else if (!value && this->_isDead)
		this->_timeDeath = std::chrono::steady_clock::time_point{};
	this->_isDead = value;
}

void	Player::setAnim(int anim)
{
	this->_anim = anim;
}

void	Player::setLastDir(int dir)
{
	this->_last_dir = dir;
}

void	Player::setPos(float x, float y)
{
	_x = x;
	_y = y;
	return ;
}

void	Player::incrementFloor(void)
{
	this->_floor++;
}

void	Player::setStartPos(int pos)
{
	_startPos = pos;
	return ;
}

void	Player::setHp(int hp)
{
	_hp = hp;
	return ;
}

void	Player::setAtkFrame(int frame)
{
	this->_atkFrame = frame;
}

void	Player::setAtk(int atk)
{
	_atk = atk;
	return ;
}

// void	Player::setDef(int def)
// {
// 	_def = def;
// 	return ;
// }

void	Player::setWallHitBox(void)
{
	_wallHitBox =
	{_x - 0.1f, _y + 0.1f, 0.2f, 0.2f};
	return ;
}

void	Player::setInQueue(bool flag)
{
	this->_inQueue = flag;
}

void	Player::setInSession(bool flag)
{
	this->_inSession = flag;
}

void	Player::setResultCurl(bool flag)
{
	this->_resultCurl = flag;
}

void	Player::addKills(void)
{
	this->_kills++;
}

void	Player::findP(void)
{
	auto plan = this->getRoom().getRoomPlan();

	int i = 0;
	for (std::string &line : plan)
	{
		size_t pos = 0;
		if ((pos = line.find('P')) != std::string::npos)
		{
			this->_x = pos + 0.5;
			this->_y = i + 0.5;
		}
		i++;
	}
}

static bool	checkWallHitBox(std::vector<std::string> const &plan, FRect const &rect, int const flag, Player &player, float deltaTime)
{
	if (flag == 0)
	{
		int y = static_cast<int>(rect.y - (6.0f * deltaTime));
		int rx = static_cast<int>(rect.x);
		int rh = static_cast<int>(rect.h);
		if (plan[y][rx] == '1' || plan[y][rx + rh] == '1' || plan[y][rx] == '3' || plan[y][rx + rh] == '3')
			return (true);
		//if the event in the room is not cleared, player cant go on 'E' tiles
		std::shared_ptr<ARoomEvent> event = player.getRoomRef().getRoomEvent();

		if (event && event->getType() == "MobRush" && event->isCleared() == false)
		{
			if (plan[y][rx] == 'E' || plan[y][rx + rh] == 'E')
				return (true);
		}
	}
	if (flag == 1)
	{
		int x = static_cast<int>(rect.x - (6.0f * deltaTime));
		int ry = static_cast<int>(rect.y);
		int rh = static_cast<int>(rect.h);
		if (plan[ry][x] == '1' || plan[ry + rh][x] == '1' || plan[ry][x] == '3' || plan[ry + rh][x] == '3')
			return (true);
		//if the event in the room is not cleared, player cant go on 'E' tiles
		std::shared_ptr<ARoomEvent> event = player.getRoomRef().getRoomEvent();

		if (event && event->getType() == "MobRush" && event->isCleared() == false)
		{
			if (plan[ry][x] == 'E' || plan[ry + rh][x] == 'E')
				return (true);
		}
	}
	if (flag == 2)
	{
		int y = static_cast<int>(rect.y + (6.0f * deltaTime));
		int rx = static_cast<int>(rect.x);
		int rh = static_cast<int>(rect.h);
		int rw = static_cast<int>(rect.w);
		if (plan[y + rh][rx] == '1' || plan[y + rh][rx + rw] == '1' || plan[y + rh][rx] == '3' || plan[y + rh][rx + rw] == '3')
			return (true);
		//if the event in the room is not cleared, player cant go on 'E' tiles
		std::shared_ptr<ARoomEvent> event = player.getRoomRef().getRoomEvent();

		if (event && event->getType() == "MobRush" && event->isCleared() == false)
		{
			if (plan[y + rh][rx] == 'E' || plan[y + rh][rx + rw] == 'E')
				return (true);
		}
	}
	if (flag == 3)
	{
		int x = static_cast<int>(rect.x + (6.0f * deltaTime));
		int ry = static_cast<int>(rect.y);
		int rh = static_cast<int>(rect.h);
		int rw = static_cast<int>(rect.w);
		if (plan[ry][x + rh] == '1' || plan[ry + rh][x + rw] == '1' || plan[ry][x + rh] == '3' || plan[ry + rh][x + rw] == '3')
			return (true);
		//if the event in the room is not cleared, player cant go on 'E' tiles
		std::shared_ptr<ARoomEvent> event = player.getRoomRef().getRoomEvent();

		if (event && event->getType() == "MobRush" && event->isCleared() == false)
		{
			if (plan[ry][x + rh] == 'E' || plan[ry + rh][x + rw] == 'E')
				return (true);
		}
	}
	
	return (false);
}

void	Player::updateAnim(std::string const &req)
{
    if (!req.empty())
	{
        if (req == "idling")
            this->setAnim(PLAYER_IDLE);
        else if (req == "walking")
            this->setAnim(PLAYER_WALKING);
        else if (req == "attacking")
		{
			if (this->_atkFrame != 2 && this->getTimeAttack() > 0.2f)
				this->setAnim(PLAYER_ATTACKING);
			else if (this->_atkFrame == 2 && this->getTimeAttack() > 0.2f)
			{
				this->resetTimeAttack();
            	this->setAnim(PLAYER_IDLE);
			}
		}
    }
}

bool	Player::updateHurt(void)
{
	if (this->_isInvinsible && this->getTimeInvincible() > 0.f && this->getTimeInvincible() <= 0.5f)
		return true;

	return false;
}

void	Player::move(std::map<std::string, std::string> &req)
{
    Room room = this->getRoom();
	float x = this->_x, y = this->_y;
	auto plan = room.getRoomPlan();
    this->setWallHitBox();
	float deltaTime = std::atof(req["deltaTime"].c_str());


	if (req["w_key"] == "true")
	{
		y -= 6.0f * deltaTime;
		FRect testHitbox = {x - 0.1f, y + 0.1f, 0.2f, 0.2f};
		if (!(y >= 0 && !checkWallHitBox(plan, testHitbox, 0, *this, deltaTime)))
			y += 6.0f * deltaTime;
	}
	if (req["a_key"] == "true")
	{
		x -= 6.0f * deltaTime;
		FRect testHitbox = {x - 0.1f, y + 0.1f, 0.2f, 0.2f};
		if (!(x >= 0 && !checkWallHitBox(plan, testHitbox, 1, *this, deltaTime)))
			x += 6.0f * deltaTime;
	}
	if (req["s_key"] == "true")
	{
		y += 6.0f * deltaTime;
		FRect testHitbox = {x - 0.1f, y + 0.1f, 0.2f, 0.2f};
		if (!(y < room.getHeight() && !checkWallHitBox(plan, testHitbox, 2, *this, deltaTime)))
			y -= 6.0f * deltaTime;
	}
	if (req["d_key"] == "true")
	{
		x += 6.0f * deltaTime;
		FRect testHitbox = {x - 0.1f, y + 0.1f, 0.2f, 0.2f};
		if (!(x < room.getWidth() && !checkWallHitBox(plan, testHitbox, 3, *this, deltaTime)))
			x -= 6.0f * deltaTime;
	}
	this->setPos(x, y);
	if (!req["last_dir"].empty())
		this->setLastDir(std::atoi(req["last_dir"].c_str()));
}

void	Player::resetTimeAttack(void)
{
	this->_timeAttack = std::chrono::steady_clock::now();
}

void	Player::startInvinsibleFrame(void)
{
	this->_isInvinsible = true;
	this->_timeInvincible = std::chrono::steady_clock::now();
}

void	Player::endInvinsibleFrame(void)
{
	this->_isInvinsible = false;
	this->_timeInvincible = std::chrono::steady_clock::time_point{};
}

bool	Player::getIsAttacking(void) const
{
	return (_isAttacking);
}

void	Player::endAttacking(void)
{
	this->_isAttacking = false;
}

void	Player::attack(void)
{
	_box.updateAtkHitBox();
	_isAttacking = true;
}

void	Player::dieAction(void)
{
	if (!this->_isDead)
		return ;
	if (this->isReConnected())
	{
		if (this->getTimeDeath() < 0.9)
			return ;
		this->setIsDead(false);
		std::string	oldTopic = this->getRoom().getRoomId();
		this->_node = this->_startNode;
		std::vector<std::string> plan = this->getRoom().getRoomPlan();
		for (size_t i = 0; i < plan.size(); i++)
		{
			size_t j = plan[i].find('P');
			if (j != plan[i].npos)
			{
				this->setPos(j + 0.5, i + 0.5);
				break;
			}
		}
		this->_ws->unsubscribe(oldTopic);
		this->_ws->subscribe(this->getRoom().getRoomId());
		this->_hp = 5;
		this->_nbrDeath++;
	}
	else
		this->_hp = 1;
}

int	Player::getNbrDeath(void) const
{
	return (this->_nbrDeath);
}

void	Player::setNbrDeath(int value)
{
	this->_nbrDeath = value;
}