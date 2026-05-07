#include "Map.hpp"

//CHAINED-MAP------------------------------------------------------------------


//Constructors/Destructors------------------------------------------------


chainedMap::chainedMap(void): _path(0), _x(0), _y(0)
{}

chainedMap::~chainedMap(void)
{}

//Member Functions--------------------------------------------------------

void chainedMap::resetRoom()
{
	this->_room.reset();
}

void chainedMap::addRoom(const Room &room, std::string sessionId, std::shared_ptr<Room> &qRoom, Map &map)
{
	if (this->_room)
		*this->_room = room;
	else
		this->_room = std::make_shared<Room>(room);

	QuanticRoom *qEvent = nullptr;
	if (qRoom)
	{
		auto ref = qRoom->getRoomEventRef();
		qEvent = dynamic_cast<QuanticRoom *>(ref.get());
	}
	this->_room->setRoomId(sessionId + ":" + room.getName() + "_" + std::to_string(this->getX()) + std::to_string(this->getY()));
	std::array<std::weak_ptr<chainedMap>, 4> dir = {this->north, this->east, this->south, this->west};
	uint8_t val = rand() % 100;
	if (val < 80)
	{
		val = rand() % 100;
		bool set = false;
		if (val < 50 || (qRoom && this->_room->getName() == qRoom->getName()))
		{
			val = 49;
			if (!qRoom || !qEvent)
			{
				if (this->_room->setEvent(2, dir, this->_x, this->_y))
				{
					map.setQRoom(this->_room);
					set = true;
					QuanticRoom &event = dynamic_cast<QuanticRoom &>(*this->_room->getRoomEventRef());
					std::array<quadList, 2>	nodes = map.getNodesQuantic();
					event.addPlace({nodes[0]->north, nodes[0]->east, nodes[0]->south, nodes[0]->west}, 1);
					nodes[0]->_room = this->_room;
					std::cout << "first quantic at: " << this->_x << ", " << this->_y << std::endl;
				}
			}
			else if (!qEvent->doesAllLocSet())
			{
				if (!(this->_room->getName() != qRoom->getName() || this->_room->getExits() != qRoom->getExits()))
				{
					std::cout << "quantic done 2 at: " << this->_x << ", " << this->_y << std::endl;
					this->_room = qRoom;
					QuanticRoom &event = dynamic_cast<QuanticRoom &>(*this->_room->getRoomEventRef());
					event.addPlace({this->north, this->east, this->south, this->west}, 0);
					set = true;
				}
			}
		}
		if (val >= 50 || !set)
			this->_room->setEvent(1, dir, this->_x, this->_y);
	}

	auto exits = this->_room->getExits();
	if (this->_room->getRoomEvent() && this->_room->getRoomEvent()->getType() == "QuanticRoom")
	{
		std::array<quadList, 2>	nodes = map.getNodesQuantic();
		if (exits[0] && !this->north.expired())
		{
			auto tmp = this->north.lock();	
			if (!tmp->getRoom())
				tmp->setPath(1);
			tmp->south = nodes[0];
			this->north.reset();
		}
		if (exits[1] && !this->east.expired())
		{
			auto tmp = this->east.lock();	
			if (!tmp->getRoom())
				tmp->setPath(1);
			tmp->west = nodes[0];
			this->east.reset();
		}
		if (exits[2] && !this->south.expired())
		{
			auto tmp = this->south.lock();	
			if (!tmp->getRoom())
				tmp->setPath(1);
			tmp->north = nodes[0];
			this->south.reset();
		}
		if (exits[3] && !this->west.expired())
		{
			auto tmp = this->west.lock();	
			if (!tmp->getRoom())
				tmp->setPath(1);
			tmp->east = nodes[0];
			this->west.reset();
		}
	}

	if (!exits[0] && !this->north.expired())
	{
		auto tmp = this->north.lock();
		tmp->south.reset();
		this->north.reset();
	}
	if (!exits[1] && !this->east.expired())
	{
		auto tmp = this->east.lock();
		tmp->west.reset();
		this->east.reset();
	}
	if (!exits[2] && !this->south.expired())
	{
		auto tmp = this->south.lock();
		tmp->north.reset();
		this->south.reset();
	}
	if (!exits[3] && !this->west.expired())
	{
		auto tmp = this->west.lock();
		tmp->east.reset();
		this->west.reset();
	}
}

std::shared_ptr<Room> chainedMap::getRoom(void) const
{
	return this->_room;
}

void	chainedMap::setPath(int flag)
{
	this->_path = flag;
}

int chainedMap::getPath() const
{
	return this->_path;
}

int chainedMap::getX() const
{
	return this->_x;
}

int chainedMap::getY() const
{
	return this->_y;
}

void chainedMap::setX(int nb)
{
	this->_x = nb;
}

void chainedMap::setY(int nb)
{
	this->_y = nb;
}

//MAP--------------------------------------------------------------------------

//Constructors/Destructors------------------------------------------------

Map::Map(std::string sessionId) : _sessionId(sessionId)
{
	for (int i = 0; i < 25; i++)
	{
		_nodes.push_back(std::make_shared<chainedMap>());
		_nodes[i]->setX(i % 5);
		_nodes[i]->setY(i / 5);
	}
	
	this->_head = _nodes[0];
	this->_height = 5;
	this->_width = 5;

	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			if (j != 0)
				_nodes[i * 5 + j]->west = _nodes[i * 5 + j - 1];
			if (j != 4)
				_nodes[i * 5 + j]->east = _nodes[i * 5 + j + 1];
			if (i != 0)
				_nodes[i * 5 + j]->north = _nodes[(i - 1) * 5 + j];
			if (i != 4)
				_nodes[i * 5 + j]->south = _nodes[(i + 1) * 5 + j];
		}
	}
	_nodesQuantic[0] = std::make_shared<chainedMap>();
	_nodesQuantic[1] = std::make_shared<chainedMap>();
	_nodesQuantic[1]->setX(1);
	_nodesQuantic[1]->setY(0);

	_nodesQuantic[0]->north = _nodesQuantic[1];
	_nodesQuantic[0]->south = _nodesQuantic[1];
	_nodesQuantic[0]->east = _nodesQuantic[1];
	_nodesQuantic[0]->west = _nodesQuantic[1];
	_nodesQuantic[0]->setPath(1);

	_nodesQuantic[1]->north = _nodesQuantic[0];
	_nodesQuantic[1]->south = _nodesQuantic[0];
	_nodesQuantic[1]->east = _nodesQuantic[0];
	_nodesQuantic[1]->west = _nodesQuantic[0];
}

Map::Map(int width, int height, std::string sessionId) : _sessionId(sessionId)
{
	for (int i = 0; i < width * height; i++)
	{
		_nodes.push_back(std::make_shared<chainedMap>());
		_nodes[i]->setX(i % width);
		_nodes[i]->setY(i / width);
	}
	
	this->_head = _nodes[0];
	this->_height = height;
	this->_width = width;

	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if (j != 0)
				_nodes[i * width + j]->west = _nodes[i * width + j - 1];
			if (j != width - 1)
				_nodes[i * width + j]->east = _nodes[i * width + j + 1];
			if (i != 0)
				_nodes[i * width + j]->north = _nodes[(i - 1) * width + j];
			if (i != height - 1)
				_nodes[i * width + j]->south = _nodes[(i + 1) * width + j];
		}
	}

	_nodesQuantic[0] = std::make_shared<chainedMap>();
	_nodesQuantic[1] = std::make_shared<chainedMap>();
	_nodesQuantic[1]->setX(1);
	_nodesQuantic[1]->setY(0);

	_nodesQuantic[0]->north = _nodesQuantic[1];
	_nodesQuantic[0]->south = _nodesQuantic[1];
	_nodesQuantic[0]->east = _nodesQuantic[1];
	_nodesQuantic[0]->west = _nodesQuantic[1];
	_nodesQuantic[0]->setPath(1);

	_nodesQuantic[1]->north = _nodesQuantic[0];
	_nodesQuantic[1]->south = _nodesQuantic[0];
	_nodesQuantic[1]->east = _nodesQuantic[0];
	_nodesQuantic[1]->west = _nodesQuantic[0];
}

Map::~Map(void)
{}

//Member Functions--------------------------------------------------------

void	Map::reset()
{
	this->_quanticRooms = nullptr;
	for (int i = 0; i < _height; i++)
	{
		for (int j = 0; j < _width; j++)
		{
			quadList node = this->_nodes[i * _width + j];
			node->resetRoom();
			node->setPath(0);
			if (j != 0)
				node->west = _nodes[i * _width + j - 1];
			if (j != _width - 1)
				node->east = _nodes[i * _width + j + 1];
			if (i != 0)
				node->north = _nodes[(i - 1) * _width + j];
			if (i != _height - 1)
				node->south = _nodes[(i + 1) * _width + j];
		}
	}
}

void	Map::link(Map &up)
{
	size_t	pos = 0;
	size_t	lastPos = 0;
	for (quadList &node : this->_nodes)
	{
		if (!node->getRoom() || node->getRoom()->getName() != "stairs")
			continue ;
		for (; pos < up._nodes.size(); pos++)
		{
			if (!up._nodes[pos]->getRoom() || up._nodes[pos]->getRoom()->getName() != "start")
				continue ;
			node->up = up._nodes[pos];
			lastPos = pos;
			pos++;
			break ;
		}
		if (node->up.expired())
			node->up = up._nodes[lastPos];
	}
}

void	Map::setWaitingRoom()
{
	std::shared_ptr<Room> room = nullptr;
	this->_nodes[0]->addRoom(Room::getWatingRoom(), this->_sessionId, room, *this);
}

void Map::setQRoom(std::shared_ptr<Room> qRoom)
{
	this->_quanticRooms = qRoom;
}

quadList &Map::getHead()
{
	return this->_head;
}

std::array<quadList, 2>	Map::getNodesQuantic() const
{
	return this->_nodesQuantic;
}

std::vector<quadList> Map::getNodes() const
{
	return this->_nodes;
}

int	Map::getHeight() const
{
	return this->_height;
}

int	Map::getWidth() const
{
	return this->_width;
}