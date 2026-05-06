CXX      = clang++
CC       = clang
CXXFLAGS = -Wall -Werror -Wextra -std=c++20 -g
CFLAGS   = -DLIBUS_USE_EPOLL -DLIBUS_NO_SSL -I./uWebSockets/uSockets/src
LDFLAGS  = -lpthread -lz -no-pie -lcurl

HEADERS  = ./includes/
SRC_DIR  = ./srcs/
OBJ_DIR  = ./objs/

CXXFLAGS += -I$(HEADERS) -I./uWebSockets/src -I./uWebSockets/uSockets/src

# CXXFLAGS += -I$(HOME)/local/include

# LDFLAGS  += -L$(HOME)/local/lib -lcurl

SRCS =  main.cpp \
		parseServer.cpp \
		updateClient.cpp \
		Server.cpp \
		Session.cpp \
		map/Map.cpp \
		map/mapMain.cpp \
		map/mapGen.cpp \
		map/aStar.cpp \
		Room.cpp \
		RoomEvent.cpp \
		QuanticRoomEvent.cpp \
		Mob.cpp \
		Player.cpp \
		HitBox.cpp \
		Party.cpp \
		curlRequest.cpp


OBJS = $(patsubst %.cpp, $(OBJ_DIR)%.o, $(SRCS))

NAME = game-server

# Liste TOUS les fichiers .c de uSockets
USOCKETS_C = $(wildcard ./uWebSockets/uSockets/src/*.c) \
			 $(wildcard ./uWebSockets/uSockets/src/eventing/*.c)

all: $(NAME)

# Compile en liant directement les .c
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -c $(USOCKETS_C)
	$(CXX) $(CXXFLAGS) $(OBJS) *.o -o $(NAME) $(LDFLAGS)
	rm -f *.o

$(OBJ_DIR)%.o: $(SRC_DIR)%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJ_DIR)
	rm -f *.o

fclean: clean
	rm -rf $(NAME)

re: fclean all

.PHONY: all clean fclean re