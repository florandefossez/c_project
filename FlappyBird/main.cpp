#include <SFML/Graphics.hpp>
#include <iostream>

#define SCALE 3

const sf::Vector2f VECTOR_SCALE = sf::Vector2f(SCALE, SCALE);
const int WINDOW_WIDTH = 143*SCALE;
const int WINDOW_HEIGHT = 255*SCALE;
const float GRAVITY = 0.5f;
const float JUMP_VELOCITY = 10.0f;

class Bird {
public:
    Bird(sf::Texture* globalTexture) {
        birdSprite.setTexture(*globalTexture);
        birdSprite.setTextureRect(sf::IntRect(3,491,17,12));
        birdSprite.scale(VECTOR_SCALE);
        reset_position();
    }

    void reset_position() {
        birdSprite.setPosition(SCALE*50, SCALE*150);
        velocityY = 0;
    }

    bool update() {
        velocityY += GRAVITY;
        birdSprite.move(0, velocityY);
        if (birdSprite.getPosition().y < 0) birdSprite.setPosition(sf::Vector2f(SCALE*50, 0));
        return (birdSprite.getPosition().y > WINDOW_HEIGHT);
    }

    void flap() {
        velocityY = -JUMP_VELOCITY;
    }

    sf::Sprite getSprite() {
        return birdSprite;
    }

private:
    sf::Texture birdTexture;
    sf::Sprite birdSprite;
    float velocityY = 0;
};



class Game {
public:
    Game() {
        // Set up the window
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Flappy Bird", sf::Style::Close);
        window.setPosition(sf::Vector2i((desktop.width - WINDOW_WIDTH) / 2, (desktop.height - WINDOW_HEIGHT) / 2));
        window.setFramerateLimit(60);

        if (!globalTexture.loadFromFile("texture.png")) {
            throw std::runtime_error("Failed to load the texture");
        }
        backgroundSprite.setTexture(globalTexture);
        backgroundSprite.setTextureRect(sf::IntRect(0,0,143,255));
        backgroundSprite.scale(VECTOR_SCALE);
        
        bird = new Bird(&globalTexture);
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
    sf::Texture globalTexture;
    sf::Sprite backgroundSprite;
    Bird* bird;

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
        if(bird->update()) {
            std::cout << "You loose\n";
            bird->reset_position();
        }
    }

    void render() {
        window.clear();
        window.draw(backgroundSprite);
        window.draw(bird->getSprite());
        window.display();
    }
};




int main() {
    Game game;
    game.run();

    return 0;
}
