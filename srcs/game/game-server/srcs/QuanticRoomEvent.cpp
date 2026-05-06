#include "Room.hpp"


QuanticRoom::QuanticRoom(std::vector<std::string> &roomPlan, std::array<std::weak_ptr<chainedMap>, 4> dir): _roomPlan(roomPlan), _nbrDead(0), _started(false), _lights_on(true)
{
	this->_type = "QuanticRoom";
	this->_lights_on = true;
	this->_started = false;
	this->_current_place = 0;
	this->_links[0] = dir;
	_isEventOnFloor = true;
	_doesAllLocSet = false;
	createEvent();
}

QuanticRoom::~QuanticRoom(){}


static bool	farEnought(float objetX, float objetY, float farX, float farY)
{
	if (std::fabs(farY - objetY) + std::fabs(farX - objetX) > 5.0f)
		return (true);
	return (false);
}

void QuanticRoom::createEvent(void)
{
	int	id = 0;
	int	maxY = _roomPlan.size();

	//find every door coord
	std::vector<std::pair<int, int>> doorPos;
	for (int y = 0; y < maxY; y++)
	{
		int maxX = _roomPlan[y].size();
		for (int x = 0; x < maxX; x++)
		{
			if (_roomPlan[y][x] == 'E')
				doorPos.push_back(std::make_pair(x, y));
		}
	}

	int nbr_mob = (rand() % 3) + 2;
	while (id <= nbr_mob)
	{
		int y = rand() % _roomPlan.size();
		int x = rand() % _roomPlan[y].size();

		if (_roomPlan[y][x] == '0')
		{
			for (auto &i : doorPos)
			{
				if (!farEnought(x + 0.5f, y + 0.5f, i.first, i.second))
					continue;
			}
			_mobs.emplace(id, std::make_unique<Mob>(x + 0.5f, y + 0.5f, 3));
			_mobsId.push_back(id);
			id++;
		}
	}
	_nbrMob = _mobs.size();

	while (1)
	{
		int y = rand() % _roomPlan.size();
		int x = rand() % _roomPlan[y].size();
		if (_roomPlan[y][x] == '1')
		{
			this->_light_place = {x, y};
			_roomPlan[y][x] = 'L';
			break ;
		}
	}
}

void	QuanticRoom::addPlace(std::array<std::weak_ptr<chainedMap>, 4> dir, bool is5thLoc)
{
	size_t i;
	if (is5thLoc)
	{
		_links[4] = dir;
		return ;
	}
	for (i = 0; i < 4; i++)
	{
		if (!_links[i][0].expired() || !_links[i][1].expired()
			|| !_links[i][2].expired() || !_links[i][3].expired())
			continue ;
		_links[i] = dir;
		break ;
	}
	if (i == 3)
		_doesAllLocSet = true;
}



bool QuanticRoom::doesAllLocSet()
{
	return _doesAllLocSet;
}

bool	QuanticRoom::isStarted(void)
{
	return(0);
}

void	QuanticRoom::makeDie(int id)
{
	if (_mobs[id]->isDead() == false)
	{
		_mobs[id]->die();
		_nbrDead++;
	}
}

bool	QuanticRoom::isCleared(void)
{
	return (0);
}

void	QuanticRoom::checkCleared(void)
{
	if (0 == false && _nbrDead == _nbrMob)
		;
}