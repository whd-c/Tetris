#pragma once

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
    sf::View fixedView;
    bool isFullscreen{false};
    const sf::VideoMode fullscreenMode = sf::VideoMode::getDesktopMode();

    sf::Image icon;
    sf::Font roboto;
    sf::Music themeMusic;
    sf::SoundBuffer rotate;
    sf::Sound rotateSound;
    sf::SoundBuffer hardDrop;
    sf::Sound hardDropSound;
    sf::SoundBuffer hold;
    sf::Sound holdSound;
    sf::SoundBuffer invalid;
    sf::Sound invalidSound;

    Tetromino currentTetromino;
    std::vector<Tetromino> bag;

    bool grounded{false};
    bool wasGrounded{grounded};

    sf::Clock delayClock;
    sf::Clock lockDelayClock;
    sf::Time lockDelayElapsed;

    uint8_t lockCounter{};

    Render renderer{window, roboto};
    GameManager gameManager{themeMusic, lockCounter};

    void applyView();
    void loadAssets();
    void handleInputs();
};