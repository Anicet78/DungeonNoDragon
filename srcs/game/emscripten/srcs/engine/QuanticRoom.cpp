#include "Room.hpp"

QuanticRoom::QuanticRoom(void): _current_place(0), _started(true)
{
	this->_type = "QuanticRoom";
	this->_lights_on = true;
	_light_place = {0, 0};
}

QuanticRoom::~QuanticRoom(){}

void	QuanticRoom::addMob(int id, float x, float y, int hp)
{
	_mobs.emplace(id, std::make_unique<Mob>(id, x, y, hp));
	return ;
}

std::array<std::weak_ptr<chainedMap>, 4>	QuanticRoom::getCurrentExit(void) const
{
	return this->_links[this->_current_place];
}

bool	QuanticRoom::isCleared(void)
{
	return (true);
}

bool	QuanticRoom::isStarted(void)
{
	return this->_started;
}

void	QuanticRoom::addPlace(std::array<std::weak_ptr<chainedMap>, 4> dir, uint8_t loc)
{
	if (loc >= 5)
		return ;
	_links[loc] = dir;
}

std::unordered_map<int, std::unique_ptr<Mob> >	&QuanticRoom::getMobs(void)
{
	return (_mobs);
}