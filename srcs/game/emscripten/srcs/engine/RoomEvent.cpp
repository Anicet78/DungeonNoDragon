#include "Room.hpp"

MobRush::MobRush(std::string type) : _started(false), _cleared(false)
{
	this->_type = type;
	return ;
}

MobRush::~MobRush() {
	return ;
}

void	MobRush::addMob(int id, float x, float y, int hp)
{
	_mobs.emplace(id, std::make_unique<Mob>(id, x, y, hp));
	return ;
}

void	MobRush::setCleared(bool value)
{
	this->_cleared = value;
}

bool	MobRush::isStarted(void)
{
	return(_started);
}

bool	MobRush::isCleared(void)
{
	return (_cleared);
}

std::unordered_map<int, std::unique_ptr<Mob>>	&MobRush::getMobs(void)
{
	return (_mobs);
}