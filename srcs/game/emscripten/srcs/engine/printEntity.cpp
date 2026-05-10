#include "Game.hpp"

void	print_others(Player &player, std::vector<Player> &otherPlayers, int flag)
{
	if (otherPlayers.size())
	{
		int	tile_s = gSdl.getMapTileSize() * 2;
		float	playerScreenX;
		float	playerScreenY;
		Camera	&camera = player.getCamera();
		for (Player &op : otherPlayers)
		{
			playerScreenX = (op.getX() - camera.getCamX()) * tile_s;
			playerScreenY = (op.getY() - camera.getCamY()) * tile_s;
			if (!flag || (flag && isUnderTree(player.getRoomRef().getRoomPlan(), op.getX(),op.getY())))
				op.printPlayer(playerScreenX, playerScreenY, flag);
		}
	}
}

void print_mobs(std::shared_ptr<ARoomEvent> &event, Player &player, int flag)
{
	int		tile_s = gSdl.getMapTileSize() * 2;
	Camera	&cam = player.getCamera();
	std::unordered_map<int, std::unique_ptr<Mob>> *mobs = nullptr;
	if (event->getType() == "MobRush")
	{
		MobRush *mobrush = dynamic_cast<MobRush *>(event.get());
		mobs = &mobrush->getMobs();
	}
	else if (event->getType() == "QuanticRoom")
	{
		QuanticRoom *qRoom = dynamic_cast<QuanticRoom *>(event.get());
		mobs = &qRoom->getMobs();
	}
	else
		return ;
	for (auto &mob : *mobs)
	{
		if (mob.second->isDead() == false)
		{
			if (!flag || (flag && isUnderTree(player.getRoomRef().getRoomPlan(), mob.second->getX(), mob.second->getY())))
				mob.second->printMob(cam.getCamX(), cam.getCamY(), tile_s, flag);
		}
		else if (mob.second->getInDeathAnim() == true)
		{
			mob.second->setAnim(MOB_DEATH);
			if (!flag || (flag && isUnderTree(player.getRoomRef().getRoomPlan(), mob.second->getX(), mob.second->getY())))
				mob.second->printMob(cam.getCamX(), cam.getCamY(), tile_s, flag);
		}
	}
}