#ifndef SESSION_HPP

# define SESSION_HPP

# include "Party.hpp"

class Server;

class Session
{
	private:
		int											_maxNumPlayer;
		std::vector<std::string>					_spectators;//maybe spectator class later
		std::vector<std::weak_ptr<Player>>			_players;
		std::vector<Map>							_maps;
		std::vector<Map>							_separedSpace;
		std::string									_sessionId;
		bool										_running;
		bool										_ended;
		std::string									_mapInfos;
		std::chrono::_V2::steady_clock::time_point	_startTime;
		std::chrono::_V2::steady_clock::time_point	_countDown;
		int											_numPlayersFinished;

		bool										_readyToRun;
		std::chrono::_V2::steady_clock::time_point	_timerBeforeRun;
		double										_readyToRunStartTimer;


	private:
		void									linkMaps(Map &down, Map &up);


	public:
		Session(void);
		Session(int numPlayer);
		~Session();
	
	public:
		void									launch();
		void									addParty(Party &newParty);
		std::string								sendMaps(void);
		void									checkFinishedPlayers(uWS::App &app, Server &server);
		void									sendEndResults(uWS::App &app, std::shared_ptr<Player> &player, bool abort);
		bool									removePlayer(std::weak_ptr<Player> rmPlayer);
		bool									removePlayer(std::string uid);
		bool									isPlayerInSession(std::string uid) const;
		void									sendToAll(Player &sender);
		void									startCountDown(void);
		std::weak_ptr<Player>					&getPlayer(std::string &uid);
		std::vector<std::weak_ptr<Player>>		getPlayers(void) const;
		int										getMaxNumPlayer(void) const;
		int										getPlaceLeft(void) const;
		double									getActualTime(void) const;
		int										getCountDown(void) const;
		int										getNumPlayers(void) const;
		std::string	const						&getSessionId(void) const;
		bool									isRunning(void) const;
		bool									isReadyToRun(void) const;
		bool									doesAllPlayersConnected() const;
		bool									hasEnded(void) const;

		double									getActualTimeBeforeRun(void) const;
		void									startLaunching(void);
		bool									isEnoughtReadyTime(void) const;

};

void	sendPlayerState(Player &player, Session &session, std::string uid_leave);
void	sendLeaveUpdate(Player &player, uWS::App &app, std::string &topic);

void	sendViaCurl(Server &server, std::string url, std::string method, std::string body, int flag);
void	sendPlayerResultCurl(Server &server, Session const &session, Player &player);

# endif