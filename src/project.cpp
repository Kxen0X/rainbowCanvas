#include "../headers/project.h"

float getRandom(float min, float max) {
	static std::mt19937 gen(std::random_device{}());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}

Project::Project() {
	this->window = new sf::RenderWindow(sf::VideoMode::getDesktopMode(), "Rainbow Canvas", sf::Style::None);
	this->view = new sf::View({(float)this->window->getSize().x/2, (float)this->window->getSize().y/2}, { 100, 100 });
	this->window->setView(*this->view);
	
	this->width = 100;
	this->height = 100;
	this->initChunks(this->width, this->height, 100);
	

	this->bgT.loadFromFile("../resources/images.jpg");
	this->bgS = new sf::Sprite(this->bgT);
	this->bgS->setScale({ (float)this->window->getSize().x / this->bgT.getSize().x, (float)this->window->getSize().y / this->bgT.getSize().y });
}


Project::~Project(){
	delete this->bgS;
	delete this->view;
	delete this->window;
}


void Project::run() {
	while (this->window->isOpen()) {
		this->eventUpdate();
		this->update();
		this->render();
	}
}


void Project::update(){
	
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
		sf::Vector2i currPos = sf::Mouse::getPosition();
		if (this->isDrag) {
			sf::Vector2f dist = sf::Vector2f(currPos - this->lastMousePos);
			this->view->move(-dist);
		}
		this->isDrag = 1;
		this->lastMousePos = currPos;
	}
	else this->isDrag = 0;
}


void Project::render(){
	this->window->clear();
	this->window->setView(*this->view);
	//this->window->draw(*bgS);
	for (int y = 0; y < this->height; y++) {
		for (int x = 0; x < this->width; x++) {
			window->draw(this->chunks[y * this->width + x].ch);
		}
	}
	this->window->display();
}


void Project::eventUpdate(){
	while (const auto& ev = this->window->pollEvent()) {
		if (auto* k = ev->getIf<sf::Event::KeyPressed>()) {
			if (k->scancode == sf::Keyboard::Scancode::Escape) {
				this->window->close();
			}
		}
		if (auto* k = ev->getIf<sf::Event::MouseWheelScrolled>()) {
			if (k->delta > 0) this->view->zoom(0.5f);
			if (k->delta < 0) this->view->zoom(1.5f);
		}
	}
}


void Project::initChunks(int width, int height, int size) {
	this->chunks = std::vector<Chunk>(width*height);
	for (int y = 0; y < height; y++) {
		for (int x = 0; x < width; x++) {
			this->chunks[y * width + x].pos = { (float)x, (float)y };
			this->chunks[y * width + x].ch.setFillColor(sf::Color(getRandom(100, 255), getRandom(100, 255), getRandom(100, 255)));
			this->chunks[y * width + x].ch.setSize({ (float)size, (float)size });
			this->chunks[y * width + x].ch.setPosition({ (float)x * size, (float)y * size });
		}
	}

}
