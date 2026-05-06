# include "Map.hpp"

static int checkRoomBorders(quadList const &place, Room const &room, Map const &map)
{
	if (place->north.expired() && room.getExits()[0])
		return 0;
	if (place->east.expired() && room.getExits()[1])
		return 0;
	if (place->south.expired() && room.getExits()[2])
		return 0;
	if (place->west.expired() && room.getExits()[3])
		return 0;
	
	if (!place->north.expired() && !room.getExits()[0]
			&& place->north.lock()->getRoom() && place->north.lock()->getRoom()->getExits()[2])
		return 0;

	if (!place->east.expired() && !room.getExits()[1]
			&& place->east.lock()->getRoom() && place->east.lock()->getRoom()->getExits()[3])
		return 0;

	if (!place->south.expired() && !room.getExits()[2]
			&& place->south.lock()->getRoom() && place->south.lock()->getRoom()->getExits()[0])
		return 0;

	if (!place->west.expired() && !room.getExits()[3]
			&& place->west.lock()->getRoom() && place->west.lock()->getRoom()->getExits()[1])
		return 0;

	int x = place->getX() - 2;
	int y = place->getY() - 2;
	auto nodes = map.getNodes();
	int maxX, maxY;
	x = (x < 0) ? 0 : x;
	y = (y < 0) ? 0 : y;
	maxX = place->getX() + 2;
	maxY = place->getY() + 2;
	maxX = (maxX >= map.getWidth()) ? map.getWidth() - 1 : maxX;
	maxY = (maxY >= map.getHeight()) ? map.getHeight() - 1 : maxY;
	for (int j = y; j <= maxY; j++)
	{
		for (int i = x; i <= maxX; i++)
		{
			if (nodes[j * map.getWidth() + i]->getRoom())
				return 0;
		}
	}

	return 1;
}

quadList Map::chooseRoom(std::string mapName, int lvl)
{
	auto floor = Room::getFloor(lvl);
	Room temp;

	int j = 0;
	while (true)
	{
		if (j == 10)
			throw std::runtime_error("could not find a place for " + mapName);
		int x = rand() % this->_width;
		int y = rand() % this->_height;

		while (this->_nodes[y * _width + x]->getRoom() || (x == 0 && y == 0))
		{
			x = rand() % this->_width;
			y = rand() % this->_height;
		}
		quadList tmp = this->_nodes[y * _width + x];

		temp = *floor[mapName];
		temp.randomizeRoom();
		int i = 0;
		while (!checkRoomBorders(tmp, temp, *this) && i < 4)
		{
			temp.turnMapLeft();
			i++;
		}
		if (i == 4)
		{
			j++;
			continue;
		}

		tmp->addRoom(temp, this->_sessionId, this->_quanticRooms, *this);
		return tmp;
	}
}

static void chooseDirections(quadList &node, std::array<bool, 4> &directions)
{
	if (node->north.expired() == false)
	{
		quadList next = node->north.lock();
		if (next && next->getPath() > 0 && node->getPath() != 3)
			directions[0] = 1;
		else if (next && next->getPath() == 3 && node->getPath() == 3)
			directions[0] = 1;
	}
	if (node->east.expired() == false)
	{
		quadList next = node->east.lock();
		if (next && next->getPath() && node->getPath() != 3)
			directions[1] = 1;
		else if (next && next->getPath() == 3 && node->getPath() == 3)
			directions[1] = 1;
	}
	if (node->south.expired() == false)
	{
		quadList next = node->south.lock();
		if (next && next->getPath() && node->getPath() != 3)
			directions[2] = 1;
		else if (next && next->getPath() == 3 && node->getPath() == 3)
			directions[2] = 1;
	}
	if (node->west.expired() == false)
	{
		quadList next = node->west.lock();
		if (next && next->getPath() && node->getPath() != 3)
			directions[3] = 1;
		else if (next && next->getPath() == 3 && node->getPath() == 3)
			directions[3] = 1;
	}
}

bool neighborExists(quadList node, int dir)
{
	switch (dir) {
		case 0: return !node->north.expired();
		case 1: return !node->east.expired();
		case 2: return !node->south.expired();
		case 3: return !node->west.expired();
	}
	return false;
}


static void selectRoom(quadList &node, std::vector<Room> &candidates, std::array<bool, 4> &directions, int lvl)
{
	int sum = directions[0] + directions[1] + directions[2] + directions[3];
	for (auto &room : Room::getFloor(lvl))
	{
		if (room.first == "start" || room.first == "stairs")
			continue ;
		Room temp = *room.second.get();
		int j;
		for (j = 0; j < 4; j++)
		{
			int i, flag = 0;
			for (i = 0; i < 4; i++)
			{
				bool roomExit = temp.getExits()[i];
				bool required = directions[i];

				// Required exit missing -> reject
				if (sum != 4 && required && !roomExit)
				{
					flag = 1;
					break;
				}
				else if (sum == 4 && required && !roomExit)
				{
					if ((i == 0 && node->north.lock()->getRoom())
						|| (i == 1 && node->east.lock()->getRoom())
						|| (i == 2 && node->south.lock()->getRoom())
						|| (i == 3 && node->west.lock()->getRoom()))
					{
						flag = 1;
						break ;
					}
				}

				//Room has an exit but neighbor is invalid → reject
				if (!required && roomExit && !neighborExists(node, i))
				{
					flag = 1;
					break;
				}
			}
			if (flag)
				temp.turnMapLeft();
			else
				break ;
		}
		if (j < 4)
			candidates.push_back(temp);
	}
}

void Map::selectAndAddRoom(quadList &node, int lvl, std::string &sessionId)
{
	if (!node->getRoom())
	{
		int i = 0;
		while (true)
		{
			if (i == 5)
				throw std::runtime_error("No candidate found for the path made\n");
			std::vector<Room> candidates;
			std::array<bool, 4> directions = {0, 0, 0, 0};

			chooseDirections(node, directions);
			selectRoom(node, candidates, directions, lvl);

			if (candidates.empty())
			{
				i++;
				continue ;
			}
			int r = rand() % candidates.size();
			node->addRoom(candidates[r], sessionId, this->_quanticRooms, *this);
			break ;
		}
	}
}

int Map::checkObs(quadList &node)
{
	int x = node->getX();
	int y = node->getY();
	for (int i = y - 1; i <= y + 1; i++)
	{
		if (i < 0 || i >= _height)
			return 0;
		for (int j = x - 1; j <= x + 1; j++)
		{
			if (j < 0 || j >= _width)
				return 0;
			if (_nodes[i * _width + j]->getRoom()
				|| _nodes[i * _width + j]->getPath() != 1)
				return 0;
		}
	}

	return 1;
}


bool randomDFS(quadList start, quadList end,
						 std::unordered_set<quadList> &visited,
						 std::vector<quadList> &path)
{
	std::unordered_map<quadList, quadList> parent;
	std::stack<quadList> stack;

	stack.push(start);
	parent[start] = nullptr;

	while (!stack.empty())
	{
		quadList node = stack.top();
		stack.pop();

		if (visited.count(node))
			continue;

		visited.insert(node);

		if (node == end)
		{
			quadList cur = end;
			while (cur)
			{
				path.push_back(cur);
				cur = parent[cur];
			}
			std::reverse(path.begin(), path.end());
			return true;
		}

		std::vector<quadList> neighbors;
		if (!node->north.expired()) neighbors.push_back(node->north.lock());
		if (!node->east.expired())  neighbors.push_back(node->east.lock());
		if (!node->south.expired()) neighbors.push_back(node->south.lock());
		if (!node->west.expired())  neighbors.push_back(node->west.lock());

		std::shuffle(neighbors.begin(), neighbors.end(),
					 std::mt19937(std::random_device{}()));

		for (auto &next : neighbors)
		{
			if (next && !visited.count(next))
			{
				parent[next] = node;
				stack.push(next);
			}
		}
	}

	return false;
}


void Map::preparePathMap(int numPlayers, int depth)
{
	std::vector<quadList> stairs;
	std::vector<quadList> starts;
	int numStart = (std::ceil(numPlayers / (depth + 1.f)) > 0) ? std::ceil(numPlayers / (depth + 1.f)) : 1;
	int numStairs = (std::ceil(numPlayers / (depth + 1.75f)) > 0) ? std::ceil(numPlayers / (depth + 1.75f)) : 1;
	for (int i = 0; i < numStart; i++)
		starts.emplace_back(this->chooseRoom("start", depth + 1));
	for (int i = 0; i < numStairs; i++)
		stairs.emplace_back(this->chooseRoom("stairs", depth + 1));
	for (int i = 0; i < numStart; i++)
	{
		quadList start;
		quadList goal = starts[i];
	
		if (i >= numStairs && i > 0)
			start = stairs[numStairs - 1];
		else
			start = stairs[i];
		quadList tmp = start, end = goal;
		std::unordered_set<quadList> visited;
		std::vector<quadList> path1;

		randomDFS(goal, start, visited, path1);
		for (auto &node : path1)
		{
			if (!node->getPath())
				node->setPath(1);
		}
		tmp->setPath(3);
		goal->setPath(3);
		auto path = Map::astar(tmp, end);

		for (auto &node : path)
			node->setPath(3);
	}
}

void Map::fillPrimaryPath(int lvl)
{
	for (auto &node : this->_nodes)
	{
		if (node->getPath() != 3)
			continue ;
		this->selectAndAddRoom(node, lvl, this->_sessionId);
		if (!node->north.expired() && !node->north.lock()->getPath())
			node->north.lock()->setPath(1);
		if (!node->east.expired() && !node->east.lock()->getPath())
			node->east.lock()->setPath(1);
		if (!node->south.expired() && !node->south.lock()->getPath())
			node->south.lock()->setPath(1);
		if (!node->west.expired() && !node->west.lock()->getPath())
			node->west.lock()->setPath(1);
	}
}

void Map::fillOtherRooms(int lvl)
{
	int count = 1;
	while (count)
	{
		count = 0;
		for (auto &node : this->_nodes)
		{
			if (node->getPath() != 1 && node->getPath() != 2)
				continue ;
			this->selectAndAddRoom(node, lvl, this->_sessionId);
			if (!node->north.expired() && !node->north.lock()->getPath())
			{
				quadList next = node->north.lock();
				this->selectAndAddRoom(next, lvl, this->_sessionId);
				next->setPath(1);
				count++;
			}
			if (!node->east.expired() && !node->east.lock()->getPath())
			{
				quadList next = node->east.lock();
				this->selectAndAddRoom(next, lvl, this->_sessionId);
				next->setPath(1);
				count++;
			}
			if (!node->south.expired() && !node->south.lock()->getPath())
			{
				quadList next = node->south.lock();
				this->selectAndAddRoom(next, lvl, this->_sessionId);
				next->setPath(1);
				count++;
			}
			if (!node->west.expired() && !node->west.lock()->getPath())
			{
				quadList next = node->west.lock();
				this->selectAndAddRoom(next, lvl, this->_sessionId);
				next->setPath(1);
				count++;
			}
		}
	}
}
void Map::fillMap(int numPlayers, int depth)
{
	this->_quanticRooms = nullptr;
	while (true)
	{
		try
		{
			this->preparePathMap(numPlayers, depth);
			this->fillPrimaryPath(depth + 1);
			this->fillOtherRooms(depth + 1);
			break ;
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			this->reset();
		}
	}
}