# include "Hud.hpp"

//Constructors/Destructors------------------------------------------------


Hud::Hud(): _minimap(), _timeCountDown(6)
{
	int w = 800, h = 150;
	this->_placeHolderTexture = loadTexture("assets/sprite/hud/hud.bmp", w, h);
	this->_healthTexture = SDL_CreateTexture(gSdl.renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, 400, 30);
	SDL_SetTextureBlendMode(this->_healthTexture, SDL_BLENDMODE_BLEND);
	SDL_Color color = (SDL_Color){255, 0, 0, 255};
	SDL_Surface* surf = TTF_RenderText_Blended(gSdl.font, "Hp :", color);
	if (!surf)
	{
		SDL_Log("RenderText error: %s", TTF_GetError());
		return ;
	}
	this->_hp = SDL_CreateTextureFromSurface(gSdl.renderer, surf);
	SDL_FreeSurface(surf);
	color = (SDL_Color){255, 128, 0, 255};
	surf = TTF_RenderText_Blended(gSdl.font, "number of mobs :", color);
	if (!surf)
	{
		SDL_Log("RenderText error: %s", TTF_GetError());
		return ;
	}
	this->_nbMobsLine = SDL_CreateTextureFromSurface(gSdl.renderer, surf);
	SDL_FreeSurface(surf);
	color = (SDL_Color){0, 255, 0, 255};
	surf = TTF_RenderText_Blended(gSdl.font, "0", color);
	if (!surf)
	{
		SDL_Log("RenderText error: %s", TTF_GetError());
		return ;
	}
	this->_nbMobs = SDL_CreateTextureFromSurface(gSdl.renderer, surf);
	SDL_FreeSurface(surf);
}

Hud::~Hud(void)
{
	SDL_DestroyTexture(this->_placeHolderTexture);
	SDL_DestroyTexture(this->_healthTexture);
	SDL_DestroyTexture(this->_nbMobsLine);
	SDL_DestroyTexture(this->_countDown);
	SDL_DestroyTexture(this->_nbMobs);
	SDL_DestroyTexture(this->_hp);
}

//Member Functions--------------------------------------------------------

void	Hud::setCountDown(int time)
{
	this->_timeCountDown = time;
}

void	Hud::printTimer(float t)
{
	std::string secM, minM, milM;
	int min = static_cast<int>(t / 60);
	float sec = std::fmod(t, 60);
	float mil = sec - (int)sec;
	milM = std::to_string(mil).substr(2, 2);
	secM = (sec < 10) ? '0' + std::to_string(sec).substr(0, 1) : std::to_string(sec).substr(0, 2);
	minM = (min < 10) ? '0' + std::to_string(min) : std::to_string(min);

	std::string timer = minM + ':' + secM + ':' + milM;
	SDL_Surface* surf = TTF_RenderText_Blended(gSdl.font, timer.c_str(), (SDL_Color){0, 255, 0, 255});
	if (!surf)
	{
		SDL_Log("RenderText error: %s", TTF_GetError());
		return ;
	}

	SDL_Texture *time = SDL_CreateTextureFromSurface(gSdl.renderer, surf);

	int w, h;
	SDL_QueryTexture(time, nullptr, nullptr, &w, &h);
	SDL_Rect dst = {static_cast<int>(215 - (w / 6)), static_cast<int>(75 - (h / 6)), w / 3, h / 3};
	SDL_RenderCopy(gSdl.renderer, time, nullptr, &dst);

	SDL_FreeSurface(surf);
	SDL_DestroyTexture(time);
}

void	Hud::printCountDown()
{
	static int t = 0;
	static int w, h;
	if (this->_timeCountDown >= 6 || this->_timeCountDown < 0)
		return ;
	if (t != this->_timeCountDown)
	{
		t = this->_timeCountDown;
		SDL_Surface* surf = TTF_RenderText_Blended(gSdl.font, std::to_string(t).c_str(), (SDL_Color){0, 0, 0, 255});
		if (!surf)
		{
			SDL_Log("RenderText error: %s", TTF_GetError());
			return ;
		}
		SDL_DestroyTexture(this->_countDown);
		this->_countDown = SDL_CreateTextureFromSurface(gSdl.renderer, surf);
		SDL_QueryTexture(this->_countDown, nullptr, nullptr, &w, &h);
		w /= 2;
		h /= 2;
	}
	else
	{
		w *= 1.1;
		h *= 1.1;
	}
	SDL_SetRenderTarget(gSdl.renderer, gSdl.game);
	SDL_Rect dst = {static_cast<int>(400 - (w / 2)), static_cast<int>(400 - (h / 2)), w, h};
	SDL_RenderCopy(gSdl.renderer, this->_countDown, nullptr, &dst);
	SDL_SetRenderTarget(gSdl.renderer, gSdl.hud);
}

void	Hud::printPlayerName(Player const &player)
{
	int w, h;
	SDL_QueryTexture(player.getNameTex(), nullptr, nullptr, &w, &h);
	SDL_Rect dst = {static_cast<int>(215 - (w / 6)), static_cast<int>(30 - (h / 6)), w / 3, h / 3};
	SDL_SetTextureColorMod(player.getNameTex(), 255, 255, 0);
	SDL_RenderCopy(gSdl.renderer, player.getNameTex(), nullptr, &dst);
	SDL_SetTextureColorMod(player.getNameTex(), 0, 255, 0);
}

void	Hud::printNbMobs(Player const &player)
{
	Room &room = player.getRoom();
	std::shared_ptr<ARoomEvent> event = room.getRoomEvent();
	static int nb = 0;

	if (event)
	{
		std::unordered_map<int, std::unique_ptr<Mob>> *mobs = nullptr;
		if (event->getType() == "MobRush")
		{
			MobRush *mobrush = dynamic_cast<MobRush *>(event.get());
			mobs = &mobrush->getMobs();
		}
		else if (event->getType() == "MobRush")
		{
			QuanticRoom *qRoom = dynamic_cast<QuanticRoom *>(event.get());
			mobs = &qRoom->getMobs();
		}
		int size = 0;
		for (auto &mob : *mobs)
		{
			if (!mob.second->isDead())
				size++;
		}
		if (size != nb)
		{
			nb = size;
			SDL_Color color = (nb) ? (SDL_Color){255, 0, 0, 255} : (SDL_Color){0, 255, 0, 255};
			SDL_Surface *surf = TTF_RenderText_Blended(gSdl.font, std::to_string(nb).c_str(), color);
			if (!surf)
			{
				SDL_Log("RenderText error: %s", TTF_GetError());
				return ;
			}
			SDL_DestroyTexture(this->_nbMobs);
			this->_nbMobs = SDL_CreateTextureFromSurface(gSdl.renderer, surf);
			SDL_FreeSurface(surf);
		}
	}
	else if (nb > 0)
	{
		nb = 0;
		SDL_Color color = (nb) ? (SDL_Color){255, 0, 0, 255} : (SDL_Color){0, 255, 0, 255};
		SDL_Surface *surf = TTF_RenderText_Blended(gSdl.font, std::to_string(nb).c_str(), color);
		if (!surf)
		{
			SDL_Log("RenderText error: %s", TTF_GetError());
			return ;
		}
		SDL_DestroyTexture(this->_nbMobs);
		this->_nbMobs = SDL_CreateTextureFromSurface(gSdl.renderer, surf);
		SDL_FreeSurface(surf);
	}

	int w, h, w2;
	SDL_QueryTexture(this->_nbMobsLine, nullptr, nullptr, &w, &h);
	SDL_Rect dst = {static_cast<int>(235 - 5 - (w / 6)), static_cast<int>(120 - (h / 6)), w / 3, h / 3};
	SDL_RenderCopy(gSdl.renderer, this->_nbMobsLine, nullptr, &dst);

	SDL_QueryTexture(this->_nbMobs, nullptr, nullptr, &w2, &h);
	dst = {static_cast<int>(235 + (w / 6) - (w2 / 6)), static_cast<int>(120 - (h / 6)), w2 / 3, h / 3};
	SDL_RenderCopy(gSdl.renderer, this->_nbMobs, nullptr, &dst);
}

static void drawRed(int x)
{
	SDL_SetRenderDrawColor(gSdl.renderer, 255, 0, 0, 255);
	if (x <= 13)
	{
		SDL_Rect rect = {13, 4, 374, 22};
		SDL_RenderFillRect(gSdl.renderer, &rect);
		rect = {11, 5, 378, 20};
		SDL_RenderFillRect(gSdl.renderer, &rect);
		rect = {10, 6, 380, 18};
		SDL_RenderFillRect(gSdl.renderer, &rect);
		rect = {8, 7, 384, 16};
		SDL_RenderFillRect(gSdl.renderer, &rect);
		rect = {7, 8, 386, 14};
		SDL_RenderFillRect(gSdl.renderer, &rect);
		rect = {6, 10, 388, 10};
		SDL_RenderFillRect(gSdl.renderer, &rect);
		rect = {5, 11, 390, 8};
		SDL_RenderFillRect(gSdl.renderer, &rect);
		rect = {4, 12, 392, 4};
		SDL_RenderFillRect(gSdl.renderer, &rect);
	}
	else
	{
		SDL_Rect rect = {x, 4, 387 - x, 22};
		SDL_RenderFillRect(gSdl.renderer, &rect);
		rect = {x, 5, 389 - x, 20};
		SDL_RenderFillRect(gSdl.renderer, &rect);
		rect = {x, 6, 390 - x, 18};
		SDL_RenderFillRect(gSdl.renderer, &rect);
		rect = {x, 7, 392 - x, 16};
		SDL_RenderFillRect(gSdl.renderer, &rect);
		rect = {x, 8, 393 - x, 14};
		SDL_RenderFillRect(gSdl.renderer, &rect);
		rect = {x, 10, 394 - x, 10};
		SDL_RenderFillRect(gSdl.renderer, &rect);
		rect = {x, 11, 395 - x, 8};
		SDL_RenderFillRect(gSdl.renderer, &rect);
		rect = {x, 12, 396 - x, 4};
		SDL_RenderFillRect(gSdl.renderer, &rect);
	}
}

static void drawYellow(int x, int lenX)
{
	SDL_SetRenderDrawColor(gSdl.renderer, 255, 255, 0, 255);
	int corr = 0;
	if (x < 13)
		x = 13;
	if (x + lenX > 387)
		corr = x + lenX - 387;
	SDL_Rect rect = {x, 4, lenX - corr, 22};
	SDL_RenderFillRect(gSdl.renderer, &rect);
	if (x < 11)
		x = 11;
	if (x + lenX > 389)
		corr = x + lenX - 389;
	rect = {x, 5, lenX - corr, 20};
	SDL_RenderFillRect(gSdl.renderer, &rect);
	if (x < 10)
		x = 10;
	if (x + lenX > 390)
		corr = x + lenX - 390;
	rect = {x, 6, lenX - corr, 18};
	SDL_RenderFillRect(gSdl.renderer, &rect);
	if (x < 8)
		x = 8;
	if (x + lenX > 392)
		corr = x + lenX - 392;
	rect = {x, 7, lenX - corr, 16};
	SDL_RenderFillRect(gSdl.renderer, &rect);
	if (x < 7)
		x = 7;
	if (x + lenX > 393)
		corr = x + lenX - 393;
	rect = {x, 8, lenX - corr, 14};
	SDL_RenderFillRect(gSdl.renderer, &rect);
	if (x < 6)
		x = 6;
	if (x + lenX > 394)
		corr = x + lenX - 394;
	rect = {x, 10, lenX - corr, 10};
	SDL_RenderFillRect(gSdl.renderer, &rect);
	if (x < 5)
		x = 5;
	if (x + lenX > 395)
		corr = x + lenX - 395;
	rect = {x, 11, lenX - corr, 8};
	SDL_RenderFillRect(gSdl.renderer, &rect);
	if (x < 4)
		x = 4;
	if (x + lenX > 396)
		corr = x + lenX - 396;
	rect = {x, 12, lenX - corr, 4};
	SDL_RenderFillRect(gSdl.renderer, &rect);
}

void	Hud::printHealthBar(Player const &player)
{
	static int hp = 0;
	static int delay = 0;
	static int level = 0;

	if (delay <= level)
	{
		SDL_SetRenderTarget(gSdl.renderer, this->_healthTexture);
		SDL_SetRenderDrawColor(gSdl.renderer, 0, 0, 0, 0);
		SDL_RenderClear(gSdl.renderer);
		if (delay < 395)
			drawYellow(delay, level - delay);
		if (level != 400)
			drawRed(level);
		SDL_SetRenderTarget(gSdl.renderer, gSdl.hud);
		SDL_SetRenderDrawColor(gSdl.renderer, 0, 0, 0, 255);
		if (delay == level)
			delay++;
		else
		{
			delay += 15;
			if (delay > level)
				delay = level;
		}
	}
	if (player.getHp() != hp && player.getHp() >= 0)
	{
		hp = player.getHp();
		if (hp > 0 && delay > level)
		    delay = level + 1000;
		SDL_SetRenderTarget(gSdl.renderer, this->_healthTexture);
		SDL_SetRenderDrawColor(gSdl.renderer, 0, 0, 0, 0);
		SDL_RenderClear(gSdl.renderer);
		if (!hp)
		{
			SDL_SetRenderTarget(gSdl.renderer, gSdl.hud);
			SDL_SetRenderDrawColor(gSdl.renderer, 0, 0, 0, 255);
			SDL_Rect rect = {370, 30, 400, 30};
			SDL_RenderCopy(gSdl.renderer, this->_healthTexture, NULL, &rect);
			delay = 400 - int(400 * ((hp + 1) / float(MAX_PLAYER_HP)));
			level = 400;
			return ;
		}
		int x = 400 - int(400 * (hp / float(MAX_PLAYER_HP)));
		if (hp != MAX_PLAYER_HP)
		{
			delay = 400 - int(400 * ((hp + 1) / float(MAX_PLAYER_HP)));
			level = x;
		}
		drawRed(x);
		SDL_SetRenderTarget(gSdl.renderer, gSdl.hud);
		SDL_SetRenderDrawColor(gSdl.renderer, 0, 0, 0, 255);
	}
	
	int w, h;
	SDL_QueryTexture(this->_hp, nullptr, nullptr, &w, &h);
	SDL_Rect dst = {static_cast<int>(350 - (w / 4)), static_cast<int>(75 - (h / 4)), w / 2, h / 2};
	SDL_RenderCopy(gSdl.renderer, this->_hp, nullptr, &dst);
	SDL_Rect rect = {370, 60, 400, 30};
	SDL_RenderCopy(gSdl.renderer, this->_healthTexture, NULL, &rect);
}

void	Hud::print(std::vector<Map> const &maps, Player const &player, int launched, float time)
{
	SDL_Rect rect = {0, 0, 800, 150};
	SDL_RenderCopy(gSdl.renderer, this->_placeHolderTexture, NULL, &rect);
	this->printHealthBar(player);
	if (launched)
		this->_minimap.printMinimap(maps, player);
	this->printTimer(time);
	this->printPlayerName(player);
	this->printNbMobs(player);
	this->printCountDown();
}

