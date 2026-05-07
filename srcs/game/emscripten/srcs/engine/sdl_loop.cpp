#include "Game.hpp"
#include <ctime>
#include <sys/time.h>

void updateRoom(Game &game, Player &player, std::string dir)
{
	Room room = player.getRoom();
	auto plan = room.getRoomPlan();

	auto exitsLoc = room.getExitsLoc();

	std::shared_ptr<ARoomEvent> rawEvent = room.getRoomEvent();

	if (rawEvent)
	{
		if (rawEvent->getType() == "MobRush")
		{
			MobRush &mobrush = dynamic_cast<MobRush &>(*rawEvent);
			for (auto &mob : mobrush.getMobs())
			{
				mob.second->setIsDead(true);
				mob.second->setInDeathAnim(false);
			}
		}
		else if (rawEvent->getType() == "QuanticRoom")
		{
			QuanticRoom &qRoom = dynamic_cast<QuanticRoom &>(*rawEvent);
			for (auto &mob : qRoom.getMobs())
			{
				mob.second->setIsDead(true);
				mob.second->setInDeathAnim(false);
			}
		}
	}

	if (rawEvent && rawEvent->getType() == "QuanticRoom")
	{
		QuanticRoom *qRoom = dynamic_cast<QuanticRoom *>(rawEvent.get());
		if (dir == "S")
		{
			game.clearOtherPlayers();
			player.setNode(qRoom->getCurrentExit()[2].lock());
			exitsLoc = player.getRoom().getExitsLoc();
			player.setPos(exitsLoc[0][0] + 0.5, exitsLoc[0][1] + 1);
		}
		else if (dir == "N")
		{
			game.clearOtherPlayers();
			player.setNode(qRoom->getCurrentExit()[0].lock());
			exitsLoc = player.getRoom().getExitsLoc();
			player.setPos(exitsLoc[2][0] + 0.5, exitsLoc[2][1] - 0.1);
		}
		else if (dir == "E")
		{
			game.clearOtherPlayers();
			player.setNode(qRoom->getCurrentExit()[1].lock());
			exitsLoc = player.getRoom().getExitsLoc();
			player.setPos(exitsLoc[3][0] + 1, exitsLoc[3][1] + 0.5);
		}
		else if (dir == "W")
		{
			game.clearOtherPlayers();
			player.setNode(qRoom->getCurrentExit()[2].lock());
			exitsLoc = player.getRoom().getExitsLoc();
			player.setPos(exitsLoc[1][0] - 0.1, exitsLoc[1][1] + 0.5);
		}
		return ;
	}

	if (dir == "S")
	{
		game.clearOtherPlayers();
		player.setNode(player.getNode()->south.lock());
		exitsLoc = player.getRoom().getExitsLoc();
		player.setPos(exitsLoc[0][0] + 0.5, exitsLoc[0][1] + 1);
	}
	else if (dir == "N")
	{
		game.clearOtherPlayers();
		player.setNode(player.getNode()->north.lock());
		exitsLoc = player.getRoom().getExitsLoc();
		player.setPos(exitsLoc[2][0] + 0.5, exitsLoc[2][1] - 0.1);
	}
	else if (dir == "E")
	{
		game.clearOtherPlayers();
		player.setNode(player.getNode()->east.lock());
		exitsLoc = player.getRoom().getExitsLoc();
		player.setPos(exitsLoc[3][0] + 1, exitsLoc[3][1] + 0.5);
	}
	else if (dir == "W")
	{
		game.clearOtherPlayers();
		player.setNode(player.getNode()->west.lock());
		exitsLoc = player.getRoom().getExitsLoc();
		player.setPos(exitsLoc[1][0] - 0.1, exitsLoc[1][1] + 0.5);
	}
	else if (dir == "U")
	{
		game.clearOtherPlayers();
		player.setNode(player.getNode()->up.lock());
		player.setStartNode(player.getNode());
		player.incrementFloor();
	}
}

void	updatePlayerPosition(Player &player, double deltaTime)
{
	static int isIdling = 1;
	int	HitFrame = 0;
	std::string w_key, a_key, s_key, d_key, anim = "idling", lastDir;

	//player movement

	if (player.getPrevState() == PLAYER_DEATH)
		return ;
	player.movePrediction(deltaTime);

	if (gSdl.key.w_key)
		w_key = "true";
	if (gSdl.key.a_key)
		a_key = "true";
	if (gSdl.key.s_key)
		s_key = "true";
	if (gSdl.key.d_key)
		d_key = "true";
	//player attack
	if ((gSdl.key.attacking() || player.checkAtkState()))
	{
		anim = "attacking";
		if (player.getPrevState() == PLAYER_ATTACKING)
		{
			if (player.getFrame() >= 12 && player.getFrame() < 20)
				HitFrame = 1;
			if (player.getFrame() == 24)
				HitFrame = 2;
		}
	}
	else if (gSdl.key.w_key || gSdl.key.a_key || gSdl.key.s_key || gSdl.key.d_key)
		anim = "walking";
	
	player.updateLastDir();
	lastDir = std::to_string(player.getLastDir());
	if (!w_key.empty() || !a_key.empty() || !s_key.empty() || !d_key.empty() || anim != "idling")
	{
		EM_ASM_({
			onCppMessage({
				action: "player_move",
				w_key: UTF8ToString($0),
				a_key: UTF8ToString($1),
				s_key: UTF8ToString($2),
				d_key: UTF8ToString($3),
				anim: UTF8ToString($4),
				last_dir: UTF8ToString($5),
				deltaTime: $6,
				attack_frame : $7
			});
		}, w_key.c_str(), a_key.c_str(), s_key.c_str(), d_key.c_str(), anim.c_str(), lastDir.c_str(), deltaTime, HitFrame);
		isIdling = 0;
	}
	else if (!isIdling)
	{
		isIdling = 1;
		EM_ASM_({
			onCppMessage({
				action: "player_move",
				w_key: UTF8ToString($0),
				a_key: UTF8ToString($1),
				s_key: UTF8ToString($2),
				d_key: UTF8ToString($3),
				anim: UTF8ToString($4),
				last_dir: UTF8ToString($5)
			});
		}, w_key.c_str(), a_key.c_str(), s_key.c_str(), d_key.c_str(), anim.c_str(), lastDir.c_str());
	}
}

float	dist(float x1, float y1, float x2, float y2)
{
	return (SDL_fabs(x2 - x1) + SDL_fabs(y2 - y1));
}

void	updateOtherPlayer(std::vector<Player> &others, double deltaTime)
{
	const float smoothing = 10;

	if (others.size())
	{
		for (auto &player : others)
		{
			float x = player.getX();
			float y = player.getY();

			float targetX = player.getTargetX();
			float targetY = player.getTargetY();

			if (dist(x, y, targetX, targetY) > 6.0f)
			{
				x = targetX;
				y = targetY;
			}
			else
			{
				x += (player.getTargetX() - x) * smoothing * deltaTime;
				y += (player.getTargetY() - y) * smoothing * deltaTime;
			}
			player.setPos(x, y);
		}
	}
	return ;
}

void	drawHud(Game &game)
{
	SDL_SetRenderTarget(gSdl.renderer, gSdl.hud);
	SDL_RenderClear(gSdl.renderer);
	game.drawHud();
	SDL_SetRenderTarget(gSdl.renderer, NULL);
	SDL_Rect dstHud = {0, GAME_HEIGHT, SCREEN_WIDTH, SCREEN_HEIGHT - GAME_HEIGHT};
	SDL_RenderCopy(gSdl.renderer, gSdl.hud, NULL, &dstHud);
}

bool isUnderTree(std::vector<std::string> plan, int x, int y)
{
	for (int i = y; i < y + 4; i++)
	{
		if (i < 0 || i >= static_cast<int>(plan.size()))
			continue ;
		for (int j = x - 1; j <= x + 1; j++)
		{
			if (j < 0 || j >= static_cast<int>(plan[i].size()))
				continue ;
			if (plan[i][j] == '1' && i != y)
				return 1;
		}
	}
	return 0;
}

void	game_loop(Game &game, double deltaTime)
{
	Player	&player = game.getPlayer();
	Camera	&camera = player.getCamera();

	updatePlayerPosition(game.getPlayer(), deltaTime); 
	updateOtherPlayer(game.getOtherPlayers(), deltaTime);
	SDL_SetRenderTarget(gSdl.renderer, gSdl.game);
	SDL_RenderClear(gSdl.renderer);
	print_map(player);
	if (player.getRoom().getRoomEvent())
	{
		MobRush &mobrush = dynamic_cast<MobRush &>(*player.getRoom().getRoomEvent());
		print_mobs(mobrush, player, 0);
	}
	print_others(player, game.getOtherPlayers(), 0);
	player.printPlayer(player.getScreenX(), player.getScreenY(), 0);
	SDL_Rect dst = {0, 0, SCREEN_WIDTH, GAME_HEIGHT};
	SDL_RenderCopy(gSdl.renderer, gSdl.texture2, &camera.getCamera(), &dst);
	if (player.getRoom().getRoomEvent())
	{
		MobRush &mobrush = dynamic_cast<MobRush &>(*player.getRoom().getRoomEvent());
		print_mobs(mobrush, player, 1);
	}
	print_others(player, game.getOtherPlayers(), 1);
	if (isUnderTree(player.getRoomRef().getRoomPlan(), player.getX(), player.getY()))
		player.printPlayer(player.getScreenX(), player.getScreenY(), 1);
	drawHud(game);
	SDL_SetRenderTarget(gSdl.renderer, NULL);
	SDL_Rect dstGame = {0, 0, SCREEN_WIDTH, GAME_HEIGHT};
	SDL_RenderCopy(gSdl.renderer, gSdl.game, &dstGame, &dstGame);
}