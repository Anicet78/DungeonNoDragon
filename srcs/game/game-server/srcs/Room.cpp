#include "Room.hpp"

std::map<std::string, std::shared_ptr<Room>>	Room::_RoomsF0;
std::map<std::string, std::shared_ptr<Room>>	Room::_RoomsF1;
std::map<std::string, std::shared_ptr<Room>>	Room::_RoomsF2;
std::map<std::string, std::shared_ptr<Room>>	Room::_RoomsF3;
std::map<std::string, std::shared_ptr<Room>>	Room::_RoomsF4;
std::map<std::string, std::shared_ptr<Room>>	Room::_WaitingRooms;

Room &Room::operator=(Room const &rhs)
{
	if (&rhs == this)
		return *this;
	this->_name = rhs._name;
	this->_height = rhs._height;
	this->_width = rhs._width;
	this->_exits = rhs._exits;
	this->_roomPlan = rhs._roomPlan;
	this->_exitsLoc = rhs._exitsLoc;
	this->_event = rhs._event;
	this->_roomID = rhs._roomID;
	return *this;
}

//Constructors/Destructors------------------------------------------------

Room::Room(void)
{
	this->_width = 10;
	this->_height = 10;
	this->_rotated = 0;
	this->_exits = {0, 0, 0, 0};
	this->_exitsLoc.fill({-1, -1});
	this->_name = "Empty";
	this->_roomID = "None";
	this->_floor = 0;
}

Room::Room(Room const &rhs): _roomID(rhs._roomID), _width(rhs._width), _height(rhs._height), _rotated(rhs._rotated),
				_exits(rhs._exits), _exitsLoc(rhs._exitsLoc), _name(rhs._name), _floor(rhs._floor), _roomPlan(rhs._roomPlan), _event(rhs._event)
{}

Room::~Room()
{}

//Member Functions--------------------------------------------------------

int Room::getWidth() const
{
	return this->_width;
}

int Room::getHeight() const
{
	return this->_height;
}

int Room::getRotated(void) const
{
	return this->_rotated;
}

std::string Room::getName() const
{
	return this->_name;
}

std::string	Room::getRoomId(void) const
{
	return this->_roomID;
}

std::vector<std::string> Room::getRoomPlan() const
{
	return this->_roomPlan;
}

std::array<bool, 4> Room::getExits() const
{
	return this->_exits;
}

void	Room::incrementRotate(void)
{
	if (this->_rotated == 3)
		this->_rotated = 0;
	else
		this->_rotated++;
}

std::array<std::array<int, 2>, 4> Room::getExitsLoc() const
{
	return this->_exitsLoc;
}

void Room::updateSize()
{
	int nWidth = 0, nHeight = 0;

	for (auto &line : this->_roomPlan)
	{
		nHeight++;
		if (nWidth < static_cast<int>(line.size()))
			nWidth = line.size();
	}
	this->_width = nWidth;
	this->_height = nHeight;
}

void Room::randomizeRoom()
{
	if (this->_roomPlan.empty())
		return ;
	this->_rotated = 0;
	int num = rand() % 4;
	if (num)
	{
		while (num--)
			this->turnMapLeft();	
	}
}

void Room::identifyExits()
{
	_exits = {0, 0, 0, 0};
    for (size_t i = 0; i < _roomPlan.size(); ++i)
    {
        const std::string& line = _roomPlan[i];
        size_t pos = 0;

        while ((pos = line.find('E', pos)) != std::string::npos)
        {
            // West
            if (pos + 1 < line.size() && (line[pos + 1] == '0' || line[pos + 1] == '2'))
			{
                _exits[3] = 1;
				_exitsLoc[3] = {static_cast<int>(pos), static_cast<int>(i)};
			}

            // East
            else if (pos > 0 && (line[pos - 1] == '0' || line[pos - 1] == '2'))
			{
                _exits[1] = 1;
				_exitsLoc[1] = {static_cast<int>(pos), static_cast<int>(i)};
			}

            // North
            else if (i + 1 < _roomPlan.size() && (_roomPlan[i + 1][pos] == '0' || _roomPlan[i + 1][pos] == '2'))
			{
                _exits[0] = 1;
				_exitsLoc[0] = {static_cast<int>(pos), static_cast<int>(i)};
			}

            // South
            else if (i > 0 && (_roomPlan[i - 1][pos] == '0' || _roomPlan[i - 1][pos] == '2'))
			{
                _exits[2] = 1;
				_exitsLoc[2] = {static_cast<int>(pos), static_cast<int>(i)};
			}
            ++pos;
        }
    }
}


void Room::turnMapLeft()
{
	auto src = this->_roomPlan;
	this->_roomPlan.clear();

	int height = src.size();

	for (int x = _width - 1; x >= 0; x--)
	{
		std::string line;
		for (int y = 0; y < height; y++)
		{
			if (x < static_cast<int>(src[y].size()))
				line += src[y][x];
			else
				line += ' ';
		}
		this->_roomPlan.push_back(line);
	}
	this->_rotated = (this->_rotated + 1) % 4;
	this->updateSize();
	this->identifyExits();
}

std::map<std::string, std::shared_ptr<Room>> Room::getFloor(int nb)
{
	if (!nb)
		return _WaitingRooms;
	if (nb == 1)
		return _RoomsF0;
	if (nb == 2)
		return _RoomsF1;
	if (nb == 3)
		return _RoomsF2;
	if (nb == 4)
		return _RoomsF3;
	if (nb == 5)
		return _RoomsF4;
	return _RoomsF0;
}

static int verifFile(std::string str, std::string ext)
{
	std::string strExt = str.substr(str.rfind('.'));
	return strExt == ext;
}

void Room::importMap(std::string &fullPath, std::string mapName, std::map<std::string, std::shared_ptr<Room>> &set)
{
	auto room = std::make_shared<Room>();
	std::ifstream file(fullPath + mapName);
	if (!file.is_open())
		throw std::runtime_error("Impossible d'ouvrir le fichier\n");

	mapName = mapName.substr(0, mapName.size() - 4);
	std::string line;
	int maxWidth = -1, maxHeight = 0;

	while (std::getline(file, line))
	{
		room->_roomPlan.push_back(line);
		if (maxWidth < static_cast<int>(line.size()))
			maxWidth = line.size();
		maxHeight++;
	}
	file.close();
	for (auto& s : room->_roomPlan)
        s.resize(maxWidth, ' ');
	room->_width = maxWidth;
	room->_height = maxHeight;
	room->_name = mapName;
	room->identifyExits();
	set.emplace(mapName, room);
}

void	Room::importFloor(std::string fullPath, std::map<std::string, std::shared_ptr<Room>> &set)
{
	DIR				*dir;
	struct dirent	*entry;

	dir = opendir(fullPath.c_str());
	if (dir == NULL)
		throw std::runtime_error("Error : function oppendir failed");
	entry = readdir(dir);
	while (entry)
	{
		if (verifFile(entry->d_name, ".map"))
			Room::importMap(fullPath, entry->d_name, set);
		entry = readdir(dir);
	}
	closedir(dir);
}

Room Room::getWatingRoom()
{
	return *_WaitingRooms["waiting"].get();
}

void Room::importRooms()
{
	std::string path("../assets/rooms/");
	
	Room::importFloor(path + "waitingRooms/", _WaitingRooms);
	Room::importFloor(path + "floor0/", _RoomsF0);
	Room::importFloor(path + "floor1/", _RoomsF1);
}

bool	Room::setEvent(uint8_t event, std::array<std::weak_ptr<chainedMap>, 4> dir)
{
	std::string	name(getName());
	if (name == "start" || name == "stairs" || name == "waiting")
		return false;

	if (!_event && event == 1)
		_event = std::make_shared<MobRush>(this->_roomPlan);
	else if (!_event && event == 2)
		_event = std::make_shared<QuanticRoom>(this->_roomPlan, dir);
	return true;
}

void	Room::setRoomId(std::string id)
{
	this->_roomID = id;
	return ;
}

std::shared_ptr<ARoomEvent>	Room::getRoomEvent(void) const
{
	return (_event);
}

std::shared_ptr<ARoomEvent>	Room::getRoomEventRef(void)
{
	return (_event);
}

std::ostream &operator<<(std::ostream &o, Room const &obj)
{
	o << "Room name: " << obj.getName() << std::endl; 
	o << "Sizes: " << obj.getWidth() << ", " << obj.getHeight() << std::endl;
	o << "exits: N = " << obj.getExits()[0] << ", E = " << obj.getExits()[1]
	  << ", S = " << obj.getExits()[2] << ", W = " << obj.getExits()[3] << std::endl;
	for (auto line : obj.getRoomPlan())
		o << line << std::endl;
	return o;
}