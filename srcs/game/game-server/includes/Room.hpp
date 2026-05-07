#ifndef ROOM_HPP

# define ROOM_HPP

# include "Mob.hpp"

class chainedMap;

class ARoomEvent
{
	protected:
		std::string _type;
	public:
		virtual ~ARoomEvent() {};
		virtual	void	createEvent(void) = 0;
		virtual bool	isCleared(void) = 0;
		virtual bool	isStarted(void) = 0;
		virtual void	checkCleared(void) = 0;
		std::string	const	&getType(void) const;
};

//make the room event a mob rush, player need to kill every mob in the room to clear it
class MobRush : public ARoomEvent
{
	private:
		std::vector<std::string>			&_roomPlan;
		std::unordered_map<int, std::unique_ptr<Mob>>	_mobs;
		std::vector<int>					_mobsId;
		int									_nbrMob;
		int									_nbrDead;

		bool	_started;
		bool	_cleared;

		void	createEvent(void);
	public:
		MobRush(std::vector<std::string> &roomPlan);
		~MobRush();

		bool	isCleared(void);
		bool	isStarted(void);
		void	destroyEvent(void);
		void	checkCleared(void);

		void	makeDie(int id);

		std::unordered_map<int, std::unique_ptr<Mob>>	&getMobs(void);
};

class QuanticRoom : public ARoomEvent
{
	private:
		std::vector<std::string>						&_roomPlan;
		std::unordered_map<int, std::unique_ptr<Mob>>	_mobs;
		std::vector<int>					_mobsId;
		int									_nbrMob;
		int									_nbrDead;

		std::array<std::array<std::weak_ptr<chainedMap>, 4>, 5>	_links;
		std::array<std::array<int, 2>, 5>	_linksPos;
		
		uint8_t				_current_place;
		std::array<int, 2>	_light_place;
		bool				_started;
		bool				_lights_on;
		bool				_isEventOnFloor;
		bool				_doesAllLocSet;
		
	private:
		void	createEvent(void);
	
	public:
		QuanticRoom(std::vector<std::string> &roomplan, std::array<std::weak_ptr<chainedMap>, 4> dir, int x, int y);
		~QuanticRoom();

	public:
		
		void													addPlace(std::array<std::weak_ptr<chainedMap>, 4> dir, bool is5thLoc);
		std::array<std::weak_ptr<chainedMap>, 4>				getCurrentExit(void) const;
		std::array<std::array<std::weak_ptr<chainedMap>, 4>, 5>	getLinks(void) const;
		std::array<std::array<int, 2>, 5>						getLinksPos(void) const;
		bool													doesAllLocSet();
		bool													isCleared(void);
		bool													isStarted(void);
		void													checkCleared(void);
		void													makeDie(int id);
		std::unordered_map<int, std::unique_ptr<Mob>>			&getMobs(void);
};

class Room
{
	private:
		std::string	_roomID;
		//Width of the room
		int _width;
		//Height of the room
		int _height;
		//if the room is rotated
		int _rotated;
		//_exits[0] = North, _exits[1] = East, _exits[2] = South, _exits[3] = West
		std::array<bool, 4> _exits;
		//localisations of the exits
		std::array<std::array<int, 2>, 4> _exitsLoc;
		//Name of the room
		std::string _name;
		//floor of the room
		uint8_t	_floor;
		//Plan of the room
		std::vector<std::string> _roomPlan;
		//Wating Room maps
		static std::map<std::string, std::shared_ptr<Room>> _WaitingRooms;
		//Map containing all of the rooms of the floor 0
		static std::map<std::string, std::shared_ptr<Room>> _RoomsF0;
		//Map containing all of the rooms of the floor 1
		static std::map<std::string, std::shared_ptr<Room>> _RoomsF1;
		//Map containing all of the rooms of the floor 2
		static std::map<std::string, std::shared_ptr<Room>> _RoomsF2;
		//Map containing all of the rooms of the floor 3
		static std::map<std::string, std::shared_ptr<Room>> _RoomsF3;
		//Map containing all of the rooms of the floor 4
		static std::map<std::string, std::shared_ptr<Room>> _RoomsF4;

		std::shared_ptr<ARoomEvent>	_event;

	private:
		static void importMap(std::string &fullPath, std::string mapName, std::map<std::string, std::shared_ptr<Room>> &set);
		static void importFloor(std::string fullPath, std::map<std::string, std::shared_ptr<Room>> &set);
		void		identifyExits(void);
		void		updateSize(void);


	public:
		Room(void);
		Room(Room const &rhs);
		Room &operator=(Room const &rhs);
		~Room(void);
	
	public:
		int													getWidth() const;
		int													getHeight() const;
		int													getRotated() const;
		void												incrementRotate();
		static Room											getWatingRoom();
		std::array<std::array<int, 2>, 4>					getExitsLoc() const;
		std::array<bool, 4>									getExits() const;
		std::string											getName() const;
		std::string											getRoomId() const;
		std::vector<std::string>							getRoomPlan() const;
		static std::map<std::string, std::shared_ptr<Room>>	getFloor(int nb);
		static void											importRooms();
		void												randomizeRoom();
		void												turnMapLeft(void);
		bool												setEvent(uint8_t event, std::array<std::weak_ptr<chainedMap>, 4> dir, int x, int y);
		void												setRoomId(std::string id);
		std::shared_ptr<ARoomEvent>							getRoomEvent(void) const;
		std::shared_ptr<ARoomEvent>							getRoomEventRef(void);
};

std::ostream &operator<<(std::ostream &o, Room const &obj);

#endif