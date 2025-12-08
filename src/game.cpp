#include "game.hpp"

Game::Game() : rotateSound(rotate), hardDropSound(hardDrop)
{
    window = sf::RenderWindow(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), static_cast<std::string>(WINDOW_TITLE), sf::State::Windowed);
    window.setFramerateLimit(144);

    fixedView.setSize({TARGET_WIDTH, TARGET_HEIGHT});
    fixedView.setCenter({TARGET_WIDTH / 2.0f, TARGET_HEIGHT / 2.0f});

    applyView();

    if (!loadAssets())
    {
        throw std::runtime_error("Game initialization failed due to critical asset loading errors.");
    }
}
void Game::applyView()
{
    sf::Vector2u currentSize = window.getSize();
    float windowWidth = static_cast<float>(currentSize.x);
    float windowHeight = static_cast<float>(currentSize.y);

    const float targetWidth = static_cast<float>(TARGET_WIDTH);
    const float targetHeight = static_cast<float>(TARGET_HEIGHT);

    float windowRatio = windowWidth / windowHeight;
    float targetRatio = targetWidth / targetHeight;

    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float viewportX = 0.0f;
    float viewportY = 0.0f;

    if (windowRatio > targetRatio)
    {
        scaleX = targetRatio / windowRatio;
        viewportX = (1.0f - scaleX) / 2.0f;
    }
    else if (windowRatio < targetRatio)
    {
        scaleY = windowRatio / targetRatio;
        viewportY = (1.0f - scaleY) / 2.0f;
    }

    fixedView.setSize({targetWidth, targetHeight});
    fixedView.setCenter({targetWidth / 2.0f, targetHeight / 2.0f});

    sf::FloatRect viewportRect({viewportX, viewportY}, {scaleX, scaleY});
    fixedView.setViewport(viewportRect);

    window.setView(fixedView);
}

bool Game::loadAssets()
{
    if (!icon.loadFromFile("icon/icon.png"))
    {
        std::cerr << "Failed to load icon.\n";
        return false;
    }
    if (!roboto.openFromFile("fonts/Roboto-VariableFont_wdth,wght.ttf"))
    {
        std::cerr << "Failed to load font.\n";
        return false;
    }
    if (!themeMusic.openFromFile("audio/theme.mp3"))
    {
        std::cerr << "Failed to load theme music.\n";
        return false;
    }
    if (!rotate.loadFromFile("audio/rotate.wav"))
    {
        std::cerr << "Failed to load rotate sound.\n";
        return false;
    }
    if (!hardDrop.loadFromFile("audio/hard-drop.wav"))
    {
        std::cerr << "Failed to load hard-drop sound.\n";
        return false;
    }

    window.setIcon(icon);
    themeMusic.setLooping(true);
    themeMusic.setPlayingOffset(sf::seconds(1.0f));
    themeMusic.play();
    rotateSound.setVolume(75.0f);
    hardDropSound.setVolume(75.0f);

    return true;
}

void Game::run()
{
    sf::Text textScore(roboto);
    sf::Text textLevel(roboto);

    textScore.setString("Score: " + std::to_string(gameManager.getScore()));
    textScore.setCharacterSize(96);

    textLevel.setString("Level " + std::to_string(gameManager.getLevel()));
    textLevel.setCharacterSize(96);

    gameManager.initializeTetrominoes();
    bag = gameManager.generateBag();
    std::optional<Tetromino> tempTetromino{gameManager.newTetromino(*bag.begin())};
    if (!tempTetromino)
    {
        std::cerr << "Failed to generate tetromino.\n";
        throw std::runtime_error("Failed to generate initial Tetromino.");
    }
    currentTetromino = *tempTetromino;
    bag.erase(bag.begin());

    while (window.isOpen())
    {
        sf::Time delayElapsed{delayClock.getElapsedTime()};
        sf::Time lockDelayElapsed = lockDelayClock.getElapsedTime();
        uint8_t delayModifier = gameManager.getLevel();
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
        handleInputs();
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
        renderer.drawHeldTetromino(gameManager.getHeldTetromino());
        const float textLevelX{renderer.getStartX() - GRID_WIDTH * CELL_SIZE};
        const float textLevelY{renderer.getStartY()};
        renderer.drawText(textLevel, "Level " + std::to_string(gameManager.getLevel()), textLevelX, textLevelY);
        const float textScoreX{renderer.getStartX() + GRID_WIDTH * CELL_SIZE + CELL_SIZE * 2};
        const float textScoreY{renderer.getStartY()};
        renderer.drawText(textScore, "Score: " + std::to_string(gameManager.getScore()), textScoreX, textScoreY);
        window.display();
    }
}

void Game::handleInputs()
{
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
            case sf::Keyboard::Scancode::F11:
            {
                isFullscreen = !isFullscreen;
                if (isFullscreen)
                {
                    window.create(fullscreenMode, static_cast<std::string>(WINDOW_TITLE), sf::State::Fullscreen);
                }
                else
                {
                    window.create(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), static_cast<std::string>(WINDOW_TITLE), sf::State::Windowed);
                    window.setIcon(icon);
                }
                window.setFramerateLimit(144);
                applyView();
                break;
            }

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
                    gameManager.setScore(0);
                    gameManager.setLevel(1);
                    gameManager.setCanHold(true);
                    gameManager.setHasHeld(false);
                    lockDelayClock.restart();
                    lockCounter = 0;
                    gameManager.setHeldTetromino(Tetromino());
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
                {
                    // play failure sound
                }

                break;
            }
            default:
                break;
            }
        }
    }
}