#include <iostream>

#include "common.hpp"
#include "tetromino.hpp"
#include "render.hpp"
#include "game_manager.hpp"

int main()
{
    auto window{sf::RenderWindow(sf::VideoMode({RWIDTH, RHEIGHT}), "Tetris", sf::State::Fullscreen)};

    bool grounded{false};
    bool wasGrounded{grounded};
    unsigned int lockCounter{};

    window.setFramerateLimit(144);

    sf::Image icon;
    if (!icon.loadFromFile("favicon/favicon.png"))
    {
        std::cerr << "Failed to load icon.\n";
        return 1;
    }

    window.setIcon(icon);

    sf::Clock delayClock;
    sf::Clock lockDelayClock;

    sf::Font roboto;
    if (!roboto.openFromFile("fonts/Roboto-VariableFont_wdth,wght.ttf"))
    {
        std::cerr << "Failed to load font.\n";
        return 1;
    }

    sf::Music themeMusic;
    if (!themeMusic.openFromFile("audio/theme.mp3"))
    {
        std::cerr << "Failed to load theme music.\n";
        return 1;
    }

    themeMusic.setLooping(true);
    themeMusic.setPlayingOffset(sf::seconds(1.0f));
    themeMusic.play();

    sf::SoundBuffer rotate;
    if (!rotate.loadFromFile("audio/rotate.wav"))
    {
        std::cerr << "Failed to load rotate sound.\n";
        return 1;
    }
    sf::Sound rotateSound(rotate);
    rotateSound.setVolume(75.0f);

    sf::SoundBuffer hardDrop;
    if (!hardDrop.loadFromFile("audio/hard-drop.wav"))
    {
        std::cerr << "Failed to load hard-drop sound.\n";
        return 1;
    }
    sf::Sound hardDropSound(hardDrop);
    hardDropSound.setVolume(75.0f);

    sf::Text textScore(roboto);
    sf::Text textLevel(roboto);

    Render renderer{window, roboto};
    GameManager gameManager{themeMusic};

    textScore.setString("Score: " + std::to_string(gameManager.score));
    textScore.setCharacterSize(96);

    textLevel.setString("Level " + std::to_string(gameManager.level));
    textLevel.setCharacterSize(96);

    unsigned int delayModifier{gameManager.level};

    gameManager.initializeTetrominoes();
    auto bag{gameManager.generateBag()};
    std::optional<Tetromino> tempTetromino{gameManager.newTetromino(*bag.begin())};
    Tetromino currentTetromino;
    if (!tempTetromino)
    {
        std::cerr << "Failed to generate tetromino.\n";
        return 1;
    }
    currentTetromino = *tempTetromino;
    bag.erase(bag.begin());

    while (window.isOpen())
    {
        sf::Time delayElapsed{delayClock.getElapsedTime()};
        sf::Time lockDelayElapsed{lockDelayClock.getElapsedTime()};
        delayModifier = gameManager.level;
        if (delayModifier > 9)
            delayModifier = 9;
        wasGrounded = grounded;
        grounded = gameManager.isGrounded(currentTetromino);
        if (grounded && lockCounter >= LOCK_LIMIT)
        {
            gameManager.handleCollision(currentTetromino);
            gameManager.handleWreck(currentTetromino, bag);
            lockDelayClock.restart();
            lockCounter = 0;
        }
        if (delayElapsed.asSeconds() > DELAY - ((DELAY * (delayModifier - 1)) / 10))
        {

            if (!grounded)
            {
                currentTetromino.pos.y++;
                lockDelayClock.restart();
            }
            else
            {
                lockCounter++;
                if (lockDelayElapsed.asSeconds() >= LOCK_DELAY)
                {
                    gameManager.handleCollision(currentTetromino);
                    gameManager.handleWreck(currentTetromino, bag);
                    lockDelayClock.restart();
                    lockCounter = 0;
                }
            }
            delayClock.restart();
        }
        while (const std::optional event{window.pollEvent()})
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto *keyPressed{event->getIf<sf::Event::KeyPressed>()})
            {
                switch (keyPressed->scancode)
                {
                case sf::Keyboard::Scancode::Escape:
                    window.close();
                    break;
                case sf::Keyboard::Scancode::Up:
                case sf::Keyboard::Scancode::W:
                {
                    Tetromino rotatedCCW{currentTetromino.rotatedCCW()};
                    if (gameManager.tryRotate(currentTetromino, rotatedCCW))
                    {
                        lockDelayClock.restart();
                        if (gameManager.isGrounded(currentTetromino))
                            lockCounter++;
                        rotateSound.play();
                    }
                    break;
                }
                case sf::Keyboard::Scancode::Z:
                {
                    Tetromino rotatedCW{currentTetromino.rotatedCW()};
                    if (gameManager.tryRotate(currentTetromino, rotatedCW))
                    {
                        lockDelayClock.restart();
                        if (gameManager.isGrounded(currentTetromino))
                            lockCounter++;
                        rotateSound.play();
                    }
                    break;
                }
                case sf::Keyboard::Scancode::Right:
                case sf::Keyboard::Scancode::D:
                {
                    Tetromino next{currentTetromino};
                    next.pos.x += 1;
                    if (gameManager.isValidPosition(next))
                    {
                        currentTetromino.pos.x = next.pos.x;
                        if (!wasGrounded && gameManager.isGrounded(currentTetromino))
                        {
                            lockDelayClock.restart();
                            lockCounter++;
                        }
                    }
                    break;
                }
                case sf::Keyboard::Scancode::Down:
                case sf::Keyboard::Scancode::S:
                {
                    Tetromino next{currentTetromino};
                    next.pos.y += 1;
                    if (gameManager.isValidPosition(next))
                    {
                        currentTetromino.pos.y = next.pos.y;
                        lockDelayClock.restart();
                    }
                    else
                    {
                        if (lockDelayElapsed.asSeconds() >= LOCK_DELAY || lockCounter >= LOCK_LIMIT)
                        {
                            gameManager.handleCollision(currentTetromino);
                            gameManager.handleWreck(currentTetromino, bag);
                            lockDelayClock.restart();
                            lockCounter = 0;
                        }
                    }
                    break;
                }
                case sf::Keyboard::Scancode::Left:
                case sf::Keyboard::Scancode::A:
                {
                    Tetromino next{currentTetromino};
                    next.pos.x -= 1;
                    if (gameManager.isValidPosition(next))
                    {
                        currentTetromino.pos.x = next.pos.x;
                        if (!wasGrounded && gameManager.isGrounded(currentTetromino))
                        {
                            lockDelayClock.restart();
                            lockCounter++;
                        }
                    }
                    break;
                }
                case sf::Keyboard::Scancode::Space:
                {
                    while (gameManager.isValidPosition(currentTetromino))
                    {
                        currentTetromino.pos.y++;
                    }
                    currentTetromino.pos.y--;
                    gameManager.handleCollision(currentTetromino);
                    gameManager.handleWreck(currentTetromino, bag);
                    lockDelayClock.restart();
                    lockCounter = 0;
                    hardDropSound.play();
                    break;
                }
                case sf::Keyboard::Scancode::R:
                {
                    bag = gameManager.generateBag();
                    std::optional<Tetromino> next{gameManager.newTetromino(bag[0])};
                    if (next)
                    {
                        for (int i = 0; i < GRID_HEIGHT; i++)
                        {
                            for (int j = 0; j < GRID_WIDTH; j++)
                            {
                                gameManager.screenState[i][j] = EMPTY;
                            }
                        }
                        gameManager.score = 0;
                        gameManager.level = 1;
                        gameManager.canHold = true;
                        gameManager.hasHeld = false;
                        lockDelayClock.restart();
                        lockCounter = 0;
                        gameManager.heldTetromino = Tetromino();
                        currentTetromino = *next;
                        bag.erase(bag.begin());
                        themeMusic.setPlayingOffset(sf::seconds(1.0f));
                    }
                    break;
                }
                case sf::Keyboard::Scancode::C:
                {
                    if (gameManager.holdTetromino(currentTetromino, bag))
                    {
                        lockDelayClock.restart();
                        lockCounter = 0;
                        // play success sound
                    }
                    else
                        // play failure sound
                        ;
                    break;
                }
                default:
                    break;
                }
            }
        }

        Tetromino ghostTetromino{currentTetromino};
        ghostTetromino.color = TRANSPARENT;
        while (gameManager.isValidPosition(ghostTetromino))
        {
            ghostTetromino.pos.y++;
        }
        ghostTetromino.pos.y--;
        if (bag.empty())
            bag = gameManager.generateBag();
        gameManager.clearRows();

        window.clear();
        renderer.drawGrid(gameManager.screenState);
        renderer.drawTetromino(ghostTetromino);
        renderer.drawTetromino(currentTetromino);
        renderer.drawNextTetromino(bag[0]);
        renderer.drawHeldTetromino(gameManager.heldTetromino);
        const float textLevelX{renderer.startX - GRID_WIDTH * CELL_SIZE};
        const float textLevelY{renderer.startY};
        renderer.drawText(textLevel, "Level " + std::to_string(gameManager.level), textLevelX, textLevelY);
        const float textScoreX{renderer.startX + GRID_WIDTH * CELL_SIZE + CELL_SIZE * 2};
        const float textScoreY{renderer.startY};
        renderer.drawText(textScore, "Score: " + std::to_string(gameManager.score), textScoreX, textScoreY);
        window.display();
    }
}
