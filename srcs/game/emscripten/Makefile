CXX			= c++
CXXFLAGS	= -Wall -Werror -Wextra -std=c++20
LDFLAGS		= $(shell sdl2-config --libs)

EMSCRIPTEN		= emcc
EM_LIBS			= -s USE_SDL=2 -s USE_SDL_TTF=2 
EM_COMMON_FLAGS	= -Wall -Werror -Wextra -std=c++20 -I$(HEADERS) -O3 $(EM_LIBS)
EM_LDFLAGS = \
			-lembind \
			-s WASM=1 \
			-s MODULARIZE=1 \
			-s ALLOW_MEMORY_GROWTH=1 \
			-s EXPORTED_FUNCTIONS='["_main"]' \
			-s EXPORTED_RUNTIME_METHODS='["callMain"]' \
			-s EXIT_RUNTIME=1 \
			-s EXPORT_NAME="createModule" \
			-s EXPORT_ES6=1 \
			-s ENVIRONMENT='web'



HEADERS		= ./includes/
SRC_DIR		= ./srcs/
OBJ_DIR		= ./objs/
ASM_OBJ_DIR	= ./objs_wasm/

CXXFLAGS += -I$(HEADERS)

SRCS		=	main.cpp \
				engine/Engine.cpp \
				engine/Key.cpp \
				engine/Assets.cpp \
				engine/sdl_loop.cpp \
				engine/load_image.cpp \
				engine/init_sdl.cpp \
				engine/keyAction.cpp \
				engine/printMap/map_printer.cpp \
				engine/printMap/print_dungeon.cpp \
				engine/printMap/print_forest.cpp \
				engine/SDLTimer.cpp \
				engine/RoomEvent.cpp \
				engine/QuanticRoom.cpp \
				engine/Mob.cpp \
				player/Player.cpp \
				player/PlayerAssets.cpp \
				mapGen/mapMain.cpp Room.cpp Map.cpp Game.cpp \
				player/Camera.cpp \
				parseAction.cpp \
				Hud.cpp \
				Minimap.cpp \
				engine/printEntity.cpp \
				updateThings.cpp \
				parseJson.cpp

EMSRCS		= $(patsubst %, $(SRC_DIR)%, $(SRCS))

OBJS		= $(patsubst %.cpp, $(OBJ_DIR)%.o, $(SRCS))
WASM_OBJS	= $(patsubst %.cpp, $(ASM_OBJ_DIR)%.o, $(SRCS))

NAME		= game
EMNAME		= ./build/game.js

all: $(NAME)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME) $(LDFLAGS)

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

wasm: $(EMNAME)

$(EMNAME): $(WASM_OBJS)
	@mkdir -p $(dir $@)
	$(EMSCRIPTEN) $(EM_COMMON_FLAGS) $(EM_LDFLAGS) $(WASM_OBJS) --preload-file ../assets -o $(EMNAME)

$(ASM_OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	$(EMSCRIPTEN) $(EM_COMMON_FLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)

wfclean:
	rm -f ./build/game.js
	rm -rf ./$(ASM_OBJ_DIR)
	rm -f ./build/game.wasm
	rm -f ./build/game.data

fclean: clean
	rm -rf $(NAME)

re: fclean wfclean all

.PHONY: all clean fclean re wfclean
