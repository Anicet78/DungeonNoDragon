#include"Game.hpp"

std::shared_ptr<ARoomEvent>	initMobRush(val &r)
{
	std::shared_ptr<ARoomEvent> event = std::make_shared<MobRush>("MobRush");
	MobRush	&rush = dynamic_cast<MobRush &>(*event);
	int nbr_mob = r["nbr_mob"].as<int>();
	if (nbr_mob != 0)
	{
		val mobs = r["mobs"];
		for (int j = 0; j < nbr_mob; j++)
		{
			val mob = mobs[j];
			int id = mob["mob_id"].as<int>();
			float x = mob["mob_x"].as<float>();
			float y = mob["mob_y"].as<float>();
			rush.addMob(id, x, y, 3);
		}
	}
	return (event);
}

void	initQRoom(val &r, Map &map, std::string roomName, int rot, int roomSet)
{
	std::shared_ptr<ARoomEvent> event = std::make_shared<QuanticRoom>();
	QuanticRoom &qRoom = dynamic_cast<QuanticRoom &>(*event);
	int nbr_mob = r["nbr_mob"].as<int>();

	auto mapSet = Room::getFloor(roomSet);
	Room room = *mapSet[roomName].get();
	while (rot--)
		room.turnMapLeft();

	if (nbr_mob != 0)
	{
		val mobs = r["mobs"];
		for (int j = 0; j < nbr_mob; j++)
		{
			val mob = mobs[j];
			int id = mob["mob_id"].as<int>();
			float x = mob["mob_x"].as<float>();
			float y = mob["mob_y"].as<float>();
			qRoom.addMob(id, x, y, 3);
		}
	};

	quadList node = map.getNodesQuantic()[0];

	qRoom.addPlace({node->north, node->east, node->south, node->west}, 4);

	room.setEvent(event);
	map.setQRoom(std::make_shared<Room>(room));
}

void	fillMap(std::vector<Map> &maps, val &msg, std::string mapName)
{
	val mObj = msg[mapName];
	int sx = mObj["size_x"].as<int>();
	int sy = mObj["size_y"].as<int>();
	maps.emplace_back(sx, sy);
	val rooms = mObj["rooms"];
	int len = mObj["nb_rooms"].as<int>();
	for (int i = 0; i < len; i++)
	{
		val r = rooms[i];
		std::string name = r["name"].as<std::string>();
		int x = r["x"].as<int>();
		int y = r["y"].as<int>();
		int rot = r["rot"].as<int>();
		if (r.hasOwnProperty("room_event"))
		{
			std::string event_type = r["room_event"].as<std::string>();
			if (event_type == "MobRush")
			{
				auto mobrush = initMobRush(r);
				maps.back().setRoomInNode(name, x, y, rot, maps.size() - 1, mobrush);
			}
			else if (event_type == "QuanticRoom")
			{
				if (!maps.back().getQRoom())
					initQRoom(r, maps.back(), name, rot, maps.size() - 1);
				maps.back().setRoomInNode(maps.back().getQRoom(), x, y, r["qRoomLoc"].as<int>());
			}
			else
				maps.back().setRoomInNode(name, x, y, rot, maps.size() - 1, NULL);
		}
		else
			maps.back().setRoomInNode(name, x, y, rot, maps.size() - 1, NULL);
	}
}

void	fillMapInfos(val &msg, Game &game)
{
	auto &vmaps = game.getMaps();
	val maps = msg["maps"];
	game.setSessionId(msg["session_id"].as<std::string>());
	fillMap(vmaps, maps, "waiting_map");
	fillMap(vmaps, maps, "floor_0");
	fillMap(vmaps, maps, "floor_1");
	vmaps[1].link(vmaps[2]);
}

void	launchGame(Game &game, val msg)
{
	auto &maps = game.getMaps();
	quadList start;

	const int nbr_player = msg["player_num"].as<int>();
	int startPos = -2;

	val playerStatus = msg["player_status"];
	for (int i = 0; nbr_player; i++)
	{
		val pStatus = playerStatus[i];
		if (game.getPlayer().getUid() == pStatus["player_uid"].as<std::string>())
		{
			startPos = pStatus["player_start"].as<int>();
			break;
		}
	}
	if (startPos < 0)
		return ;

	for (quadList &node : maps[1].getNodes())
	{
		if (node->getRoom() && node->getRoom()->getName() == "start" && !startPos--)
		{
			start = node;
			break ;
		}
	}

	game.getPlayer().setNode(start);
	game.getPlayer().setStartNode(start);
	game.setLaunched(1);

	gSdl.enableIsRunning();

	EM_ASM_({onCppMessage({action: "launched"});});
}

void	endGame(val &msg, Game &game)
{
	Player &player = game.getPlayer();
	float	time = msg["time"].as<float>();
	int		win = msg["win"].as<int>();
	int min, sec;
	min = static_cast<int>(time / 60);
	sec = static_cast<int>(std::fmod(time, 60));
	float mil = std::fmod(time, 60) - sec;
	EM_ASM_({
			sendResults({
				mob_killed: $0,
				completion_time_min: $1,
				completion_time_sec: $2,
				completion_time_mil: $3,
				is_winner: $4
			});
		}, player.getKills(),  min, sec, mil, win);
	finishGame();
}

void	parseJson(bool &init, Game &game)
{
	if (!msgJson.size())
		return ;
	val msg = msgJson.front();
	msgJson.pop();
	if (msg.isUndefined() || msg.isNull() || !msg.hasOwnProperty("action"))
		return ;
	std::string action = msg["action"].as<std::string>();
	if (action == "waiting" || action == "reconnect")
	{
		init = true;
		fillMapInfos(msg, game);
		if (action == "waiting")
		{
			game.getPlayer().setNode(game.getMaps()[0].getNodes()[0]);
			EM_ASM_({onCppMessage({action: "connected"});});
		}
		else
		{
			int floor = msg["player_floor"].as<int>();
			int x = msg["map_x"].as<int>();
			int y = msg["map_y"].as<int>();
			Map &map = game.getMaps()[floor + 1];
			int w = map.getWidth();
			game.getPlayer().setNode(map.getNodes()[y * w + x]);
			float px = msg["room_x"].as<float>();
			float py = msg["room_y"].as<float>();
			game.getPlayer().setNbrDeath(msg["nbr_death"].as<int>());

			int startPos = msg["start_pos"].as<int>();
			for (quadList &node : map.getNodes())
			{
				if (node->getRoom() && node->getRoom()->getName() == "start" && !startPos--)
				{
					quadList start;
					start = node;
					game.getPlayer().setStartNode(start);
					break ;
				}
			}
			
			game.getPlayer().setPos(px, py);
			while (floor--)
				game.getPlayer().incrementFloor();
			game.setLaunched(1);
			gSdl.enableIsRunning();
			EM_ASM_({onCppMessage({action: "reconnected"});});
		}
	}
	else if (action == "loop_action")
	{
		val loop = msg["loop"];
		if (!gSdl.getIsRunning())
		{
			if (msg.hasOwnProperty("running") && msg["running"].as<int>() == 1)
			{
				game.getOtherPlayers().clear();
				if (loop.hasOwnProperty("player_update"))
					launchGame(game, loop["player_update"]);
			}
		}
		if (msg.hasOwnProperty("session_timer"))
			game.setTime(msg["session_timer"].as<float>());
		if (msg.hasOwnProperty("countdown"))
			game.getHud().setCountDown(msg["countdown"].as<int>());
		if (loop.hasOwnProperty("player_update"))
			loopPlayerState(game, loop["player_update"]);
		if (loop.hasOwnProperty("room_update"))
			loopRoomState(game, loop["room_update"]);
	}
	else if (action == "finished")
	{
		init = false;
		endGame(msg, game);
	}
	else if (action == "room_change")
		changeRoom(game, msg["player_leave"]);
}