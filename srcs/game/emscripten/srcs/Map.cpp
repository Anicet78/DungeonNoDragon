#include "Map.hpp"

//CHAINED-MAP------------------------------------------------------------------


//Constructors/Destructors------------------------------------------------


chainedMap::chainedMap(void): _path(0), _x(0), _y(0)
{}

chainedMap::~chainedMap(void)
{}

//Member Functions--------------------------------------------------------

void	chainedMap::setRoomInQNode(std::shared_ptr<Room> room)
{
	this->_room = room;
}

void	chainedMap::addRoom(const Room &room, Map &map)
{
	if (this->_room)
		*this->_room = room;
	else
		this->_room = std::make_shared<Room>(room);
	auto exits = this->_room->getExits();

	std::shared_ptr<ARoomEvent> rawEvent = room.getRoomEvent();

	if (rawEvent && rawEvent->getType() == "QuanticRoom")
	{
		std::array<quadList, 2>	nodes = map.getNodesQuantic();
		if (exits[0] && !this->north.expired())
		{
			auto tmp = this->north.lock();	
			tmp->south = nodes[0];
			this->north.reset();
		}
		if (exits[1] && !this->east.expired())
		{
			auto tmp = this->east.lock();	
			tmp->west = nodes[0];
			this->east.reset();
		}
		if (exits[2] && !this->south.expired())
		{
			auto tmp = this->south.lock();
			tmp->north = nodes[0];
			this->south.reset();
		}
		if (exits[3] && !this->west.expired())
		{
			auto tmp = this->west.lock();	
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

void chainedMap::addRoom(std::shared_ptr<Room> &room, Map &map)
{
	this->_room = room;
	auto exits = this->_room->getExits();

	std::shared_ptr<ARoomEvent> rawEvent = room->getRoomEvent();

	if (rawEvent && rawEvent->getType() == "QuanticRoom")
	{
		std::array<quadList, 2>	nodes = map.getNodesQuantic();
		if (exits[0] && !this->north.expired())
		{
			auto tmp = this->north.lock();	
			tmp->south = nodes[0];
			this->north.reset();
		}
		if (exits[1] && !this->east.expired())
		{
			auto tmp = this->east.lock();	
			tmp->west = nodes[0];
			this->east.reset();
		}
		if (exits[2] && !this->south.expired())
		{
			auto tmp = this->south.lock();
			tmp->north = nodes[0];
			this->south.reset();
		}
		if (exits[3] && !this->west.expired())
		{
			auto tmp = this->west.lock();	
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

Map::Map(void)
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

	_nodesQuantic[1]->north = _nodesQuantic[0];
	_nodesQuantic[1]->south = _nodesQuantic[0];
	_nodesQuantic[1]->east = _nodesQuantic[0];
	_nodesQuantic[1]->west = _nodesQuantic[0];
}

Map::Map(int width, int height)
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

	_nodesQuantic[1]->north = _nodesQuantic[0];
	_nodesQuantic[1]->south = _nodesQuantic[0];
	_nodesQuantic[1]->east = _nodesQuantic[0];
	_nodesQuantic[1]->west = _nodesQuantic[0];
}

Map::~Map(void)
{}

//Member Functions--------------------------------------------------------

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

void	Map::setRoomInNode(std::string &roomName, int x, int y, int rot, int roomSet, std::shared_ptr<ARoomEvent> event)
{
	quadList &node = _nodes[y * _width + x];
	auto mapSet = Room::getFloor(roomSet);
	Room room = *mapSet[roomName].get();
	while (rot--)
		room.turnMapLeft();
	if (event)
	{
		room.setEvent(event);
	}
	node->addRoom(room, *this);
}

void	Map::setRoomInNode(std::shared_ptr<Room> room, int x, int y, uint8_t loc)
{
	quadList &node = _nodes[y * _width + x];

	std::shared_ptr<ARoomEvent> rawEvent = this->_quanticRoom->getRoomEvent();
	QuanticRoom *event = dynamic_cast<QuanticRoom *>(rawEvent.get());
	event->addPlace({node->north, node->east, node->south, node->west}, loc);

	node->addRoom(room, *this);
}

void	Map::setWaitingRoom()
{
	this->_nodes[0]->addRoom(Room::getWatingRoom(), *this);
}

quadList &Map::getHead()
{
	return this->_head;
}

std::vector<quadList> &Map::getNodes()
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

std::array<quadList, 2>	&Map::getNodesQuantic()
{
	return this->_nodesQuantic;
}


std::shared_ptr<Room>	Map::getQRoom(void)
{
	return this->_quanticRoom;
}

void	Map::setQRoom(std::shared_ptr<Room> qRoom)
{
	this->_quanticRoom = qRoom;
}