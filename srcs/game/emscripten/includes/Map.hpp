#ifndef MAP_HPP

# define MAP_HPP

# include "Room.hpp"

class Map;

class chainedMap
{
	private:
		std::shared_ptr<Room>	_room;
		int						_path;
		int						_x;
		int						_y;


	public:
		chainedMap(void);
		~chainedMap(void);
	
	public:
		std::weak_ptr<chainedMap>	north;
		std::weak_ptr<chainedMap>	south;
		std::weak_ptr<chainedMap>	east;
		std::weak_ptr<chainedMap>	west;
		std::weak_ptr<chainedMap>	up;
	
	public:

		void					addRoom(const Room &room, Map &map);
		void					addRoom(std::shared_ptr<Room> &room, Map &map);
		void					setRoomInQNode(std::shared_ptr<Room> room);
		std::shared_ptr<Room>	getRoom(void) const;
		void					setPath(int flag);
		int						getPath() const;
		int						getX() const;
		int						getY() const;
		void					setX(int nb);
		void					setY(int nb);
};

using quadList = std::shared_ptr<chainedMap>;

class Map
{
	private:
		int						_width;
		int						_height;
		quadList				_head;
		std::vector<quadList>	_nodes;
		std::array<quadList, 2>	_nodesQuantic;
		std::shared_ptr<Room>	_quanticRoom;


	public:
		Map(void);
		Map(int width, int height);
		~Map(void);
	
	public:
		quadList				&getHead(void);
		std::vector<quadList>	&getNodes();
		void					link(Map &up);
		int						getWidth() const;
		int						getHeight() const;
		std::shared_ptr<Room>	getQRoom();
		std::array<quadList, 2>	&getNodesQuantic();
		void					setRoomInNode(std::string &roomName, int x, int y, int rot, int roomSet, std::shared_ptr<ARoomEvent> event);
		void					setRoomInNode(std::shared_ptr<Room> room, int x, int y, uint8_t loc);
		void					setWaitingRoom();
		void					setQRoom(std::shared_ptr<Room> qRoom);
};

void printMap(Map &floor0);

#endif