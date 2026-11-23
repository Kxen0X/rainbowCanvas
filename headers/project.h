#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <random>
#include <algorithm>
#include <atomic>
#include <mutex>
#include <thread>
#include <queue>


struct ChunkPos {
	int x, y;
	bool operator==(const ChunkPos& other) const {
		return other.x == this->x && other.y == this->y;
	}
};

namespace std {
	template <>
	struct hash<ChunkPos> {
		size_t operator()(const ChunkPos& p) const {
			size_t h1 = hash<int>{}(p.x);
			size_t h2 = hash<int>{}(p.y);
			return h1 ^ (h2 + 0x9e3779b9 + (h1 << 6) + (h1 >> 2));
		}
	};
}
struct Chunk {
	ChunkPos pos;
	bool isLoaded;
	sf::RectangleShape ch;
};



class Project {
public:
	Project();
	~Project();

	void update();
	void render();
	void run();
private:
	void eventUpdate();
	void createChunk(int);
	void checkArea();
	int height;
	int width;
	int chSize;
	int finalRadius;
	sf::RenderWindow* window;
	sf::View* view;

	sf::Vector2i lastMousePos;
	bool isDrag = 0;

	std::unordered_map<ChunkPos, Chunk> chunks;
	std::unordered_set<ChunkPos> pendingChunks;
	std::mutex pendingMutex;
	std::mutex chunkMutex;

	std::vector<ChunkPos> queue;
	std::mutex queueMutex;

	std::thread createChunks;
	std::atomic<bool> isRunning;

};