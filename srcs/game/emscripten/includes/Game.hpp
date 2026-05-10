#ifndef GAME_HPP

#define GAME_HPP

# include "Hud.hpp"


class Game
{
	private:
		std::vector<Map>	_maps;
		Player				&_player;
		std::vector<Player>	_otherPlayers;
		Hud					_hud;
		std::string			_sessionId;
		int					_launched;
		float				_time_in_s;


	public:
		Game(Player &player);
		~Game(void);
	
	public:
		std::vector<Map>	&getMaps(void);
		Player				&getPlayer(void);
		std::vector<Player> &getOtherPlayers(void);
		Player				&getOtherPlayer(std::string &uid);
		Hud					&getHud(void);
		float				getTime(void) const;
		int					getLaunched(void) const;
		std::string	const	&getSessionId(void) const;
		void				setSessionId(std::string sessionId);
		void				setLaunched(int nb);
		void				setTime(float time);
		void				drawHud(void);
		void				addMap(Map &map);
		void				clearOtherPlayers(void);
		bool				isInOtherPlayers(std::string &uid) const;
		void				addOtherPlayer(std::string &uid, std::string &name);
		void				suppOtherPlayer(std::string &uid);
		void				suppOtherPlayer(const std::string &uid);
};


#ifdef __EMSCRIPTEN__
	extern std::queue<val> msgJson;
#endif

int	init_sdl(Engine &gSdl);
void	game_loop(Game &game, double fps);

void	key_down(void);
void	key_up(void);
void	reset_key(void);
void	updateRoom(Game &game, Player &player, std::string dir);

void	print_map(Player &player);
void	manage_floor(int x, int y, Player &player);
void	manage_wall(int x, int y, Player &player);
void	manage_water(int x, int y, Player &player);
void	manage_stairs(int x, int y, Player &player);

void	manageSoil(int x, int y, Player &player);
void	manage_wall_forest(int x, int y, Player &player, int iteration);

void	print_others(Player &player, std::vector<Player> &otherPlayers, int flag);
void	print_mobs(std::shared_ptr<ARoomEvent> &event, Player &player, int flag);
bool	isUnderTree(std::vector<std::string> plan, int x, int y);
void	loopPlayerState(Game &game, val playerUpdate);
void	loopRoomState(Game &game, val roomUpdate);
void	parseJson(bool &init, Game &game);
void	changeRoom(Game &game, val playerLeave);
void	finishGame(void);



#endif