#include "../headers/project.h"

float getRandom(float min, float max) {
	static std::mt19937 gen(std::random_device{}());
	std::uniform_real_distribution<float> dist(min, max);
	return dist(gen);
}

float dist(sf::Vector2f first, sf::Vector2f second) {
	sf::Vector2f tmp = second - first;
	return std::sqrt(tmp.x * tmp.x + tmp.y * tmp.y);
}


Project::Project() : isRunning{ true } {
	this->window = new sf::RenderWindow(sf::VideoMode::getDesktopMode(), "Rainbow Canvas", sf::Style::None);
	this->view = new sf::View({(float)this->window->getSize().x/2, (float)this->window->getSize().y/2}, { 100, 100 });
	this->window->setView(*this->view);
	
	this->width = 100;
	this->height = 100;
	this->chSize = 200;

	this->createChunks = std::thread{&Project::createChunk, this, this->chSize};
	
}


Project::~Project(){
	this->isRunning = false;
	if (this->createChunks.joinable()) {
		this->createChunks.join();
	}
	
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
			this->view->move(-dist*2.f);
		}
		this->isDrag = 1;
		this->lastMousePos = currPos;
	}
	else this->isDrag = 0;

	this->checkArea();

}


void Project::render(){
	this->window->clear();
	this->window->setView(*this->view);
	{
		std::lock_guard<std::mutex> lock{ this->chunkMutex };
		for (auto& [pos, ch] : this->chunks) {
			this->window->draw(ch.ch);
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
			if (k->delta > 0) {
				this->view->zoom(0.5f);
			}
			if (k->delta < 0 && this->view->getSize().x < 5000.f) {
				this->view->zoom(2.f);
			}
		}
	}
}


void Project::createChunk(int size) {
	while (this->isRunning) {
		bool hasPos = 0;
		ChunkPos pos;

		{
			std::lock_guard<std::mutex> lock{ this->queueMutex };
			if (!this->queue.empty()) {
				pos = this->queue.back();
				this->queue.pop_back();
				hasPos = 1;
			}
		}
		if (hasPos) {
			
			Chunk newChunk;

			newChunk.pos = pos;
			sf::RectangleShape tmp({ (float)size, (float)size });
			tmp.setFillColor(sf::Color(getRandom(0, 255), getRandom(0, 255), getRandom(0, 255)));
			tmp.setPosition({ (float)pos.x * size, (float)pos.y * size });
			newChunk.ch = tmp;
			newChunk.isLoaded = 1;
			{
				std::lock_guard<std::mutex> lock{ this->chunkMutex };
				this->chunks[pos] = newChunk;

			}
			{
				std::lock_guard<std::mutex> lock{ this->pendingMutex };
				this->pendingChunks.erase(pos);
			}
		}
		else {
			//std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
}


void Project::checkArea() {
	sf::Vector2f center = this->view->getCenter();
	sf::Vector2f size = this->view->getSize();

	float startX = static_cast<int>(std::floor(center.x - size.x / 2) / this->chSize);
	float endX = static_cast<int>(std::floor(center.x + size.x / 2) / this->chSize);
	float startY = static_cast<int>(std::floor(center.y - size.y / 2) / this->chSize);
	float endY = static_cast<int>(std::floor(center.y + size.y / 2) / this->chSize);

	float maxDimension = std::max(size.x, size.y);
	int dynamicRadius = static_cast<int>(std::ceil((maxDimension / 2.0f) / this->chSize));

	this->finalRadius = dynamicRadius + 3;

	int camX = static_cast<int>(std::floor(center.x / this->chSize));
	int camY = static_cast<int>(std::floor(center.y / this->chSize));
	{
		std::lock_guard<std::mutex> lock{ this->queueMutex };
		for (int x = startX; x <= endX; x++) {
			for (int y = startY; y <= endY; y++) {
				ChunkPos pos = { x, y };

				if (chunks.find(pos) == chunks.end() &&
					std::find(queue.begin(), queue.end(), pos) == queue.end()) {
					queue.push_back(pos);
				}
			}
		}

		std::sort(queue.begin(), queue.end(), [camX, camY](const ChunkPos& a, const ChunkPos& b) {
			float distA = std::pow(a.x - camX, 2) + std::pow(a.y - camY, 2);
			float distB = std::pow(b.x - camX, 2) + std::pow(b.y - camY, 2);
			return distA > distB;
			});
	}
	

	{
		std::lock_guard<std::mutex> lock{ this->chunkMutex };
		for (auto it = this->chunks.begin(); it != this->chunks.end();) {
			ChunkPos p = it->first;
			if (std::abs(p.x - camX) > (this->finalRadius + 3) ||
				std::abs(p.y - camY) > (this->finalRadius + 3)) {
				it = this->chunks.erase(it);
			}
			else ++it;
		}
	}
}