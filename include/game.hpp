#pragma once
#include <iostream>

#include "common.hpp"
#include "tetromino.hpp"
#include "render.hpp"
#include "game_manager.hpp"

class Game
{
public:
    Game();
    void run();

private:
    sf::RenderWindow window;
    sf::Image icon;
    sf::Font roboto;
    sf::Music themeMusic;
    sf::SoundBuffer rotate;
    sf::Sound rotateSound;
    sf::SoundBuffer hardDrop;
    sf::Sound hardDropSound;

    Tetromino currentTetromino;
    std::vector<Tetromino> bag;

    bool grounded{false};
    bool wasGrounded{grounded};

    sf::Clock delayClock;
    sf::Clock lockDelayClock;
    sf::Time lockDelayElapsed;

    uint8_t lockCounter{};

    Render renderer{window, roboto};
    GameManager gameManager{themeMusic};

    bool loadAssets();
    void handleInputs();
};