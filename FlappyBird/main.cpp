#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>


#define SCALE 3

const sf::Vector2f VECTOR_SCALE = sf::Vector2f(SCALE, SCALE);
const int WINDOW_WIDTH = 143*SCALE;
const int WINDOW_HEIGHT = 255*SCALE;
const float GRAVITY = 0.5f;
const float JUMP_VELOCITY = 10.0f;

typedef struct assets_ {
    sf::Texture global_texture;
} assets_t;


class Bird {
public:
    Bird(assets_t* assets) {
        birdSprite.setTexture(assets->global_texture);
        birdSprite.setTextureRect(sf::IntRect(145,226,17,12));
        birdSprite.scale(VECTOR_SCALE);
        reset_position();
    }

    void reset_position() {
        birdSprite.setPosition(SCALE*50, SCALE*150);
        velocityY = 0;
    }

    void update() {
        velocityY += GRAVITY;
        birdSprite.move(0, velocityY);
        if (birdSprite.getPosition().y < 0) birdSprite.setPosition(sf::Vector2f(SCALE*50, 0));
        if (birdSprite.getPosition().y > SCALE*189) birdSprite.setPosition(sf::Vector2f(SCALE*50, SCALE*189));
    }

    void flap() {
        velocityY = -JUMP_VELOCITY;
    }

    sf::Sprite getSprite() {
        return birdSprite;
    }

private:
    sf::Sprite birdSprite;
    float velocityY = 0;
};


class Pipes {
public:
    Pipes(assets_t* assets) : assets(assets) {}

    void create_pipe() {
        sf::Sprite top_pipe;
        top_pipe.setTexture(assets->global_texture);
        top_pipe.setTextureRect(sf::IntRect(287,0,26,160));
        top_pipe.setPosition(sf::Vector2f(WINDOW_WIDTH, 0));
        top_pipe.setScale(VECTOR_SCALE);

        top_pipes.push_back(top_pipe);
    }

    void update() {
        static int frame = 0;
        frame++;
        if (frame%100 == 0) {
            create_pipe();
            frame = 1;
        }

        for (int i = 0; i < top_pipes.size(); i++) {
			if (top_pipes.at(i).getPosition().x < 0) {
				top_pipes.erase(top_pipes.begin() + i);
	        }
			else {
				sf::Vector2f position = top_pipes.at(i).getPosition();
				top_pipes.at(i).move(-1*SCALE, 0);
			}
		}
    }

    void draw(sf::RenderWindow* window) {
        for (int i = 0; i < top_pipes.size(); i++) {
            window->draw(top_pipes.at(i));
        }
    }

private:
    std::vector<sf::Sprite> top_pipes;
    std::vector<sf::Sprite> bottom_pipes;
    assets_t* assets;
};



class Game {
public:
    Game() {
        // Set up the window
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Flappy Bird", sf::Style::Close);
        window.setPosition(sf::Vector2i((desktop.width - WINDOW_WIDTH) / 2, (desktop.height - WINDOW_HEIGHT) / 2));
        window.setFramerateLimit(60);

        if (!assets.global_texture.loadFromFile("texture.png")) {
            throw std::runtime_error("Failed to load the texture");
        }
        backgroundSprite.setTexture(assets.global_texture);
        backgroundSprite.setTextureRect(sf::IntRect(0,0,143,255));
        backgroundSprite.scale(VECTOR_SCALE);
        
        bird = new Bird(&assets);
        pipes = new Pipes(&assets);
    }

    void run() {
        while (window.isOpen()) {
            handleEvents();
            update();
            render();
        }
    }

private:
    sf::RenderWindow window;
    sf::Sprite backgroundSprite;
    Bird* bird;
    Pipes* pipes;
    assets_t assets;

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {
                    bird->flap();
                }
            }
        }
    }

    void update() {
        bird->update();
        pipes->update();
    }

    void render() {
        window.clear();
        window.draw(backgroundSprite);
        window.draw(bird->getSprite());
        pipes->draw(&window);
        window.display();
    }
};




int main() {
    Game game;
    game.run();

    return 0;
}
