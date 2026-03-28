#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <random>


struct Chunk {
	sf::Vector2f pos;
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
	void initChunks(int, int, int);
	int height;
	int width;

	sf::RenderWindow* window;
	sf::View* view;

	sf::Vector2i lastMousePos;
	bool isDrag = 0;

	std::vector<Chunk> chunks;


	sf::Texture bgT;
	sf::Sprite* bgS;

};