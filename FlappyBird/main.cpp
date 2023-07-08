#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>


#define SCALE 3

const sf::Vector2f VECTOR_SCALE = sf::Vector2f(SCALE, SCALE);
const int WINDOW_WIDTH = 143*SCALE;
const int WINDOW_HEIGHT = 255*SCALE;
const float GRAVITY = 0.5f;
const float JUMP_VELOCITY = 9.0f;

typedef struct assets_ {
    sf::Texture global_texture;
} assets_t;

typedef enum state_ {
    Startwait,
    Playing,
    GameOver
} state_t;


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
        birdSprite.setTextureRect(sf::IntRect(145 + 18*wing_frame,226,17,12));
        wing_frame = (wing_frame+1)%3;
        velocityY += GRAVITY;
        birdSprite.move(0, velocityY);
        birdSprite.setRotation(1.5*velocityY);
        if (birdSprite.getPosition().y < 0) {
            birdSprite.setPosition(sf::Vector2f(SCALE*50, 0));
            birdSprite.setRotation(0);
        }
        if (birdSprite.getPosition().y > SCALE*189) {
            birdSprite.setPosition(sf::Vector2f(SCALE*50, SCALE*189));
            birdSprite.setRotation(0);
            velocityY = 0;
        }
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
    int wing_frame = 0;
};


class Pipes {
public:
    Pipes(assets_t* assets) : assets(assets) {}
    std::vector<sf::Sprite> top_pipes;
    std::vector<sf::Sprite> bottom_pipes;
    assets_t* assets;

    void create_pipe() {
        int h = rand()%100 + 20;
        sf::Sprite top_pipe;
        sf::Sprite bottom_pipe;
        top_pipe.setTexture(assets->global_texture);
        bottom_pipe.setTexture(assets->global_texture);
        top_pipe.setTextureRect(sf::IntRect(287,0,26,160));
        bottom_pipe.setTextureRect(sf::IntRect(315,0,26,h));
        top_pipe.setPosition(sf::Vector2f(WINDOW_WIDTH, -10*SCALE -h*SCALE));
        bottom_pipe.setPosition(sf::Vector2f(WINDOW_WIDTH, 200*SCALE -h*SCALE));
        top_pipe.setScale(VECTOR_SCALE);
        bottom_pipe.setScale(VECTOR_SCALE);

        top_pipes.push_back(top_pipe);
        bottom_pipes.push_back(bottom_pipe);
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
				bottom_pipes.erase(bottom_pipes.begin() + i);
	        }
			else {
				sf::Vector2f position = top_pipes.at(i).getPosition();
				top_pipes.at(i).move(-1*SCALE, 0);

                position = bottom_pipes.at(i).getPosition();
				bottom_pipes.at(i).move(-1*SCALE, 0);
			}
		}
    }

    void draw(sf::RenderWindow* window) {
        for (int i = 0; i < top_pipes.size(); i++) {
            window->draw(top_pipes.at(i));
            window->draw(bottom_pipes.at(i));
        }
    }

};



class Game {
public:
    Game() {
        // Set up the window
        sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
        window.create(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Flappy Bird", sf::Style::Close);
        window.setPosition(sf::Vector2i((desktop.width - WINDOW_WIDTH) / 2, (desktop.height - WINDOW_HEIGHT) / 2));
        window.setFramerateLimit(50);

        if (!assets.global_texture.loadFromFile("texture.png")) {
            throw std::runtime_error("Failed to load the texture");
        }
        backgroundSprite.setTexture(assets.global_texture);
        backgroundSprite.setTextureRect(sf::IntRect(0,0,143,255));
        backgroundSprite.scale(VECTOR_SCALE);
        startSprite.setTexture(assets.global_texture);
        startSprite.setTextureRect(sf::IntRect(145,176,57,42));
        startSprite.scale(VECTOR_SCALE);
        startSprite.setPosition((WINDOW_WIDTH-startSprite.getGlobalBounds().width)/2, (WINDOW_HEIGHT-startSprite.getGlobalBounds().height)/2);
        titleSprite.setTexture(assets.global_texture);
        titleSprite.setTextureRect(sf::IntRect(145,57,92,26));
        titleSprite.scale(VECTOR_SCALE);
        titleSprite.setPosition((WINDOW_WIDTH-titleSprite.getGlobalBounds().width)/2, (WINDOW_HEIGHT-titleSprite.getGlobalBounds().height)/4);
        
        bird = new Bird(&assets);
        pipes = new Pipes(&assets);
        state = Startwait;
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
    sf::Sprite startSprite;
    sf::Sprite titleSprite;
    Bird* bird;
    Pipes* pipes;
    assets_t assets;
    state_t state;

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Space) {

                    switch (state) {
                    case Startwait:
                        state = Playing;
                        break;
                    case Playing:
                        bird->flap();
                        break;
                    case GameOver:
                        state = Startwait;
                        pipes->bottom_pipes.clear();
                        pipes->top_pipes.clear();
                        bird->reset_position();
                        titleSprite.setTextureRect(sf::IntRect(145,57,92,26));
                        break;
                    default:
                        break;
                    }

                }
            }
        }
    }

    bool collision() {
        sf::FloatRect bird_rect = bird->getSprite().getGlobalBounds();
        for (int i = 0; i < pipes->top_pipes.size(); i++) {
            if (pipes->top_pipes.at(i).getGlobalBounds().intersects(bird_rect))
                return true;
        }
        for (int i = 0; i < pipes->bottom_pipes.size(); i++) {
            if (pipes->bottom_pipes.at(i).getGlobalBounds().intersects(bird_rect))
                return true;
        }
        return false;
    }

    void update() {
        switch (state) {
        case Startwait:
            break;
        
        case Playing:
            bird->update();
            pipes->update();
            if(collision()) {
                state = GameOver;
                titleSprite.setTextureRect(sf::IntRect(145,90,96,21));
            }
            break;
        
        case GameOver:
            break;
        
        default:
            break;
        }
        
    }

    void render() {
        window.clear();
        window.draw(backgroundSprite);

        switch (state) {
        case Startwait:
            window.draw(startSprite);
            window.draw(titleSprite);
            break;
        
        case Playing:
            pipes->draw(&window);
            window.draw(bird->getSprite());
            break;
        
        case GameOver:
            pipes->draw(&window);
            window.draw(bird->getSprite());
            window.draw(titleSprite);
            break;
        
        default:
            break;
        }
        
        
        window.display();
    }
};




int main() {
    Game game;
    game.run();

    return 0;
}
