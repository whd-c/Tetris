#include <iostream>
#include <vector>
#include <algorithm>
#include <random>

#include <SFML/Graphics.hpp>

enum Color
{
    EMPTY,
    CYAN,
    BLUE,
    ORANGE,
    YELLOW,
    GREEN,
    PURPLE,
    RED,
    DARK_PURPLE,
    TRANSPARENT
};

struct Position
{
    int8_t x{};
    int8_t y{};
};

constexpr int GRID_WIDTH{10};
constexpr int GRID_HEIGHT{20};

class Tetromino
{
public:
    uint8_t squareSize;
    Color color;
    char id;

    Position pos{};
    int8_t rotationIndex{};

    std::vector<std::vector<Color>> piece;

    Tetromino() : squareSize{},
                  color{},
                  id{},
                  pos{0, 0},
                  rotationIndex{},
                  piece{} {}
    Tetromino(int _squareSize, char _id, Color _color)
        : squareSize(_squareSize), id(_id), color(_color), piece(_squareSize, std::vector<Color>(_squareSize, EMPTY))
    {
    }

    void initializePosition()
    {
        pos.x = (GRID_WIDTH - squareSize) / 2;
        pos.y = 0;
        if (id == 'I')
            pos.y--;
    }
};

sf::Color enumToColor(Color choice);
void initializeTetrominoes();
std::vector<Tetromino> generateBag();
Tetromino rotatedTetrominoCCW(const Tetromino &currentTetromino);
Tetromino rotatedTetrominoCW(const Tetromino &currentTetromino);
bool tryRotate(Tetromino &currentTetromino, const Tetromino &rotatedPiece);
std::optional<Tetromino> newTetromino(const Tetromino &tetromino);
bool isValidPosition(const Tetromino &tetromino, int8_t deltaX = 0, int8_t deltaY = 0);
bool isGrounded(const Tetromino &tetromino);
void handleCollision(const Tetromino &tetromino);
void handleWreck(Tetromino &tetromino, std::vector<Tetromino> &bag);
bool holdTetromino(Tetromino &tetromino, std::vector<Tetromino> &bag);
void printHeldTetromino(sf::RenderWindow &window, float startX, float startY);
void clearRows();
void printTetromino(sf::RenderWindow &window, const Tetromino &tetromino, float startX, float startY);
void printNextTetromino(sf::RenderWindow &window, const Tetromino &tetromino, float startX, float startY);
void printGrid(sf::RenderWindow &window, float &gridX, float &gridY);

std::vector<Tetromino> tetrominoes = {
    {2, 'O', YELLOW},
    {4, 'I', CYAN},
    {3, 'S', GREEN},
    {3, 'Z', RED},
    {3, 'L', ORANGE},
    {3, 'J', BLUE},
    {3, 'T', PURPLE},
};

// SRS offset data for all tetrominos except I
std::array<std::array<Position, 5>, 4> offsetData{{{{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}},
                                                   {{{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}}},
                                                   {{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}},
                                                   {{{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}}}};
// SRS kick table for the I tetromino
std::array<std::array<Position, 5>, 4> kickTableICW{{{{{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}}},
                                                     {{{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}}},
                                                     {{{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}}},
                                                     {{{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}}}}};

std::array<std::array<Position, 5>, 4> kickTableICCW{{{{{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}}},
                                                      {{{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}}},
                                                      {{{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}}},
                                                      {{{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}}}}};

constexpr unsigned int RWIDTH{1920};
constexpr unsigned int RHEIGHT{1080};

constexpr float DELAY{1.0f};
constexpr float LOCK_DELAY{0.5f};
constexpr unsigned int LOCK_LIMIT{10};

constexpr float COLOR_SIZE{40.0f};
constexpr float SPACING{0.0f};
constexpr float CELL_SIZE{COLOR_SIZE + SPACING};
constexpr float RECTANGLE_OUTLINE_SIZE{-1.5f};

// init with EMPTY (0)
std::array<std::array<Color, GRID_WIDTH>, GRID_HEIGHT> screenState{};

Tetromino heldTetromino;
bool canHold{true};
bool hasHeld{false};

sf::Font roboto;

unsigned int level{1};
int score{};

int main()
{
    auto window{sf::RenderWindow(sf::VideoMode({RWIDTH, RHEIGHT}), "Tetris", sf::State::Fullscreen)};
    unsigned int delayModifier{level};

    bool grounded{false};
    bool wasGrounded{grounded};
    unsigned int lockCounter{};

    window.setFramerateLimit(144);

    sf::Image icon;
    if (!icon.loadFromFile("favicon/favicon.png"))
    {
        std::cerr << "Failed to load icon\n";
        return 1;
    }

    window.setIcon(icon);

    sf::Clock delayClock;
    sf::Clock lockDelayClock;

    if (!roboto.openFromFile("fonts/Roboto/Roboto-VariableFont_wdth,wght.ttf"))
    {
        std::cerr << "Failed to load font\n";
        return 1;
    }

    sf::Text textScore(roboto);
    sf::Text textLevel(roboto);

    textScore.setString("Score: " + std::to_string(score));
    textScore.setCharacterSize(96);

    textLevel.setString("Level " + std::to_string(level));
    textLevel.setCharacterSize(96);

    initializeTetrominoes();
    auto bag{generateBag()};
    std::optional<Tetromino> tempTetromino{newTetromino(*bag.begin())};
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
        delayModifier = level;
        if (delayModifier > 9)
            delayModifier = 9;
        wasGrounded = grounded;
        grounded = isGrounded(currentTetromino);
        if (grounded && lockCounter >= LOCK_LIMIT)
        {
            handleCollision(currentTetromino);
            handleWreck(currentTetromino, bag);
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
                    handleCollision(currentTetromino);
                    handleWreck(currentTetromino, bag);
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
                    Tetromino rotatedCCW{rotatedTetrominoCCW(currentTetromino)};
                    if (tryRotate(currentTetromino, rotatedCCW))
                    {
                        lockDelayClock.restart();
                        if (isGrounded(currentTetromino))
                            lockCounter++;
                        // play success sound
                    }

                    else
                        // play failure sound
                        ;
                    break;
                }
                case sf::Keyboard::Scancode::Z:
                {
                    Tetromino rotatedCW{rotatedTetrominoCW(currentTetromino)};
                    if (tryRotate(currentTetromino, rotatedCW))
                    {
                        lockDelayClock.restart();
                        if (isGrounded(currentTetromino))
                            lockCounter++;
                        // play success sound
                    }
                    else
                        // play failure sound
                        ;
                    break;
                }
                case sf::Keyboard::Scancode::Right:
                case sf::Keyboard::Scancode::D:
                {
                    Tetromino next{currentTetromino};
                    next.pos.x += 1;
                    if (isValidPosition(next))
                    {
                        currentTetromino.pos.x = next.pos.x;
                        if (!wasGrounded && isGrounded(currentTetromino))
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
                    if (isValidPosition(next))
                    {
                        currentTetromino.pos.y = next.pos.y;
                        lockDelayClock.restart();
                    }
                    else
                    {
                        if (lockDelayElapsed.asSeconds() >= LOCK_DELAY || lockCounter >= LOCK_LIMIT)
                        {
                            handleCollision(currentTetromino);
                            handleWreck(currentTetromino, bag);
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
                    if (isValidPosition(next))
                    {
                        currentTetromino.pos.x = next.pos.x;
                        if (!wasGrounded && isGrounded(currentTetromino))
                        {
                            lockDelayClock.restart();
                            lockCounter++;
                        }
                    }
                    break;
                }
                case sf::Keyboard::Scancode::Space:
                {
                    while (isValidPosition(currentTetromino))
                    {
                        currentTetromino.pos.y++;
                    }
                    currentTetromino.pos.y--;
                    handleCollision(currentTetromino);
                    handleWreck(currentTetromino, bag);
                    lockDelayClock.restart();
                    lockCounter = 0;
                    break;
                }
                case sf::Keyboard::Scancode::R:
                {
                    bag = generateBag();
                    std::optional<Tetromino> next{newTetromino(bag[0])};
                    if (next)
                    {
                        for (int i = 0; i < GRID_HEIGHT; i++)
                        {
                            for (int j = 0; j < GRID_WIDTH; j++)
                            {
                                screenState[i][j] = EMPTY;
                            }
                        }
                        score = 0;
                        level = 1;
                        canHold = true;
                        hasHeld = false;
                        lockDelayClock.restart();
                        lockCounter = 0;
                        heldTetromino = Tetromino();
                        currentTetromino = *next;
                        bag.erase(bag.begin());
                    }
                    break;
                }
                case sf::Keyboard::Scancode::C:
                {
                    if (holdTetromino(currentTetromino, bag))
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
        while (isValidPosition(ghostTetromino))
        {
            ghostTetromino.pos.y++;
        }
        ghostTetromino.pos.y--;

        window.clear();
        float gridStartX, gridStartY;
        float textScoreX{gridStartX + GRID_WIDTH * CELL_SIZE + CELL_SIZE * 2};
        float textScoreY{gridStartY};
        float textLevelX{gridStartX - GRID_WIDTH * CELL_SIZE};
        float textLevelY{gridStartY};
        if (bag.empty())
            bag = generateBag();

        textScore.setPosition({textScoreX, textScoreY});
        textLevel.setPosition({textLevelX, textLevelY});
        textScore.setString("Score: " + std::to_string(score));
        textLevel.setString("Level " + std::to_string(level));
        printGrid(window, gridStartX, gridStartY);
        clearRows();
        printTetromino(window, ghostTetromino, gridStartX, gridStartY);
        printTetromino(window, currentTetromino, gridStartX, gridStartY);
        printNextTetromino(window, bag[0], gridStartX, gridStartY);
        printHeldTetromino(window, gridStartX, gridStartY);
        window.draw(textScore);
        window.draw(textLevel);
        window.display();
    }
}

sf::Color enumToColor(Color choice)
{
    switch (choice)
    {
    case EMPTY:
        return sf::Color(66, 73, 82);
    case CYAN:
        return sf::Color::Cyan;
    case BLUE:
        return sf::Color::Blue;
    case ORANGE:
        return sf::Color(255, 127, 0);
    case YELLOW:
        return sf::Color::Yellow;
    case GREEN:
        return sf::Color::Green;
    case PURPLE:
        return sf::Color(128, 0, 128);
    case RED:
        return sf::Color::Red;
    case DARK_PURPLE:
        return sf::Color(49, 49, 80);
    case TRANSPARENT:
        return sf::Color(255, 255, 255, 64);
    default:
        return sf::Color::Black;
    }
}
void initializeTetrominoes()
{
    for (Tetromino &tetromino : tetrominoes)
    {
        switch (tetromino.id)
        {
        case 'O':
            tetromino.piece[0][0] = tetromino.color;
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            break;
        case 'I':
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            tetromino.piece[1][3] = tetromino.color;
            break;
        case 'S':
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[0][2] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            break;
        case 'Z':
            tetromino.piece[0][0] = tetromino.color;
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            break;
        case 'L':
            tetromino.piece[0][2] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            break;
        case 'J':
            tetromino.piece[0][0] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            break;
        case 'T':
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            break;
        }
    }
}

std::vector<Tetromino> generateBag()
{
    std::vector<Tetromino> bag{tetrominoes};

    std::random_device rd;
    std::mt19937 mt(rd());
    std::shuffle(bag.begin(), bag.end(), mt);
    return bag;
}

Tetromino rotatedTetrominoCCW(const Tetromino &currentTetromino)
{
    Tetromino rotatedTetrominoCCW{currentTetromino};
    if (--rotatedTetrominoCCW.rotationIndex == -1)
        rotatedTetrominoCCW.rotationIndex = 3;
    for (int i = 0; i < rotatedTetrominoCCW.squareSize; i++)
    {
        for (int j = 0; j < rotatedTetrominoCCW.squareSize; j++)
        {
            rotatedTetrominoCCW.piece[rotatedTetrominoCCW.squareSize - 1 - j][i] = currentTetromino.piece[i][j];
        }
    }

    return rotatedTetrominoCCW;
}
Tetromino rotatedTetrominoCW(const Tetromino &currentTetromino)
{
    Tetromino rotatedTetrominoCW{currentTetromino};
    if (++rotatedTetrominoCW.rotationIndex == 4)
        rotatedTetrominoCW.rotationIndex = 0;
    for (int i = 0; i < rotatedTetrominoCW.squareSize; i++)
    {
        for (int j = 0; j < rotatedTetrominoCW.squareSize; j++)
        {
            rotatedTetrominoCW.piece[j][rotatedTetrominoCW.squareSize - 1 - i] = currentTetromino.piece[i][j];
        }
    }

    return rotatedTetrominoCW;
}

bool tryRotate(Tetromino &currentTetromino, const Tetromino &rotatedPiece)
{
    const int startRot = currentTetromino.rotationIndex;
    const int endRot = rotatedPiece.rotationIndex;
    if (rotatedPiece.id == 'I')
    {
        const int nextRotCW = (startRot + 1) % 4;
        const bool isCW = (endRot == nextRotCW);
        const auto &kickTable = isCW ? kickTableICW : kickTableICCW;
        for (int i = 0; i < 5; i++)
        {
            const int deltaX = kickTable[startRot][i].x;
            const int deltaY = kickTable[startRot][i].y;

            if (isValidPosition(rotatedPiece, deltaX, deltaY))
            {
                currentTetromino = rotatedPiece;
                currentTetromino.pos.x += deltaX;
                currentTetromino.pos.y += deltaY;
                return true;
            }
        }
    }
    else
    {
        for (int i = 0; i < 5; i++)
        {
            const int deltaX = offsetData[startRot][i].x - offsetData[endRot][i].x;
            const int deltaY = offsetData[startRot][i].y - offsetData[endRot][i].y;
            if (isValidPosition(rotatedPiece, deltaX, deltaY))
            {
                currentTetromino = rotatedPiece;
                currentTetromino.pos.x += deltaX;
                currentTetromino.pos.y += deltaY;
                return true;
            }
        }
    }
    return false;
}

std::optional<Tetromino> newTetromino(const Tetromino &tetromino)
{
    Tetromino temp{tetromino};
    temp.initializePosition();

    for (int i = 0; i < temp.squareSize; i++)
    {
        for (int j = 0; j < temp.squareSize; j++)
        {
            int gridX = temp.pos.x + j;
            int gridY = temp.pos.y + i;
            if (gridY >= 0 && gridY < GRID_HEIGHT && gridX >= 0 && gridX < GRID_WIDTH)
            {
                if (screenState[gridY][gridX] != EMPTY)
                {
                    return std::nullopt;
                }
            }
        }
    }

    return temp;
}

bool isValidPosition(const Tetromino &tetromino, int8_t deltaX, int8_t deltaY)
{
    for (int i = 0; i < tetromino.squareSize; i++)
    {
        for (int j = 0; j < tetromino.squareSize; j++)
        {
            if (tetromino.piece[i][j] != EMPTY)
            {
                const int8_t gridX = tetromino.pos.x + j + deltaX;
                const int8_t gridY = tetromino.pos.y + i + deltaY;

                if (gridX < 0 || gridX >= GRID_WIDTH)
                {
                    return false;
                }
                if (gridY >= GRID_HEIGHT)
                {
                    return false;
                }
                if (gridY >= 0 && screenState[gridY][gridX] != EMPTY)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

bool isGrounded(const Tetromino &tetromino)
{
    Tetromino next = tetromino;
    next.pos.y++;
    return !isValidPosition(next);
}

void handleCollision(const Tetromino &tetromino)
{
    for (int i = 0; i < tetromino.squareSize; i++)
    {
        for (int j = 0; j < tetromino.squareSize; j++)
        {
            if (tetromino.piece[i][j] != EMPTY)
            {
                int gridX{tetromino.pos.x + j};
                int gridY{tetromino.pos.y + i};

                if (gridY >= 0 && gridY < GRID_HEIGHT && gridX >= 0 && gridX < GRID_WIDTH)
                    screenState[gridY][gridX] = tetromino.piece[i][j];
            }
        }
    }
    canHold = true;
}

void handleWreck(Tetromino &tetromino, std::vector<Tetromino> &bag)
{
    std::optional<Tetromino> nextTetromino{newTetromino(bag[0])};
    if (!nextTetromino)
    {
        for (int i = 0; i < GRID_HEIGHT; i++)
        {
            for (int j = 0; j < GRID_WIDTH; j++)
            {
                screenState[i][j] = EMPTY;
            }
        }
        bag = generateBag();
        nextTetromino = newTetromino(bag[0]);
        score = 0;
        level = 1;
        canHold = true;
        hasHeld = false;
        heldTetromino = Tetromino();
    }
    if (nextTetromino)
    {
        tetromino = *nextTetromino;
        bag.erase(bag.begin());
    }
}

bool holdTetromino(Tetromino &tetromino, std::vector<Tetromino> &bag)
{
    if (!canHold)
        return false;
    canHold = false;

    if (!hasHeld)
    {
        heldTetromino = tetromino;
        tetromino = *newTetromino(*bag.begin());
        bag.erase(bag.begin());
        hasHeld = true;
    }
    else
    {
        Tetromino temp = heldTetromino;
        heldTetromino = tetromino;
        tetromino = temp;
    }
    heldTetromino.initializePosition();
    // rotate till its at the default rotation
    while (heldTetromino.rotationIndex != 0)
    {
        heldTetromino = rotatedTetrominoCCW(heldTetromino);
    }
    return true;
}

void printHeldTetromino(sf::RenderWindow &window, float startX, float startY)
{

    const float previewBoxX{startX + GRID_WIDTH * CELL_SIZE - CELL_SIZE * 19};
    const float previewBoxY{startY + CELL_SIZE * 5};
    const float previewBoxSize{CELL_SIZE * 6};

    auto boxBg{sf::RectangleShape({previewBoxSize, previewBoxSize})};
    boxBg.setPosition({previewBoxX, previewBoxY});
    boxBg.setFillColor(enumToColor(EMPTY));
    boxBg.setOutlineColor(sf::Color::White);
    boxBg.setOutlineThickness(3.0f);
    window.draw(boxBg);

    auto label{sf::Text(roboto, "HOLD", 36)};
    label.setPosition({previewBoxX + 75, previewBoxY - 50});
    window.draw(label);

    auto rectangle{sf::RectangleShape({COLOR_SIZE, COLOR_SIZE})};

    const float pieceWidth{heldTetromino.squareSize * CELL_SIZE};
    const float pieceHeight{heldTetromino.squareSize * CELL_SIZE};

    const float offsetX{previewBoxX + (previewBoxSize - pieceWidth) / 2.0f};
    const float offsetYDenominator{(heldTetromino.id != 'O') ? 1.5f : 2.0f};
    const float offsetY{previewBoxY + (previewBoxSize - pieceHeight) / offsetYDenominator};

    for (int i = 0; i < heldTetromino.squareSize; i++)
    {
        for (int j = 0; j < heldTetromino.squareSize; j++)
        {
            if (heldTetromino.piece[i][j] == EMPTY)
                continue;
            const float posX{offsetX + j * CELL_SIZE};
            const float posY{offsetY + i * CELL_SIZE};

            rectangle.setPosition({posX, posY});
            rectangle.setFillColor(enumToColor(heldTetromino.color));
            rectangle.setOutlineThickness(RECTANGLE_OUTLINE_SIZE);
            rectangle.setOutlineColor(enumToColor(DARK_PURPLE));
            window.draw(rectangle);
        }
    }
}

void clearRows()
{
    int writeRow{GRID_HEIGHT - 1};
    int rowsCleared{};
    for (int i = GRID_HEIGHT - 1; i >= 0; i--)
    {
        bool fullRow{true};
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            if (screenState[i][j] == EMPTY)
            {
                fullRow = false;
                break;
            }
        }

        if (!fullRow)
        {

            if (i != writeRow)
            {
                for (int j = 0; j < GRID_WIDTH; j++)
                {
                    screenState[writeRow][j] = screenState[i][j];
                }
            }
            writeRow--;
        }
        else
        {
            rowsCleared++;
        }
    }
    for (int i = writeRow; i >= 0; i--)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            screenState[i][j] = EMPTY;
        }
    }
    switch (rowsCleared)
    {
    case 1:
        score += 40;
        break;
    case 2:
        score += 100;
        break;
    case 3:
        score += 300;
        break;
    case 4:
        score += 1200;
        break;
    }
    if (rowsCleared > 0)
        level = (score / 500) + 1;
}

void printTetromino(sf::RenderWindow &window, const Tetromino &tetromino, float startX, float startY)
{
    auto rectangle{sf::RectangleShape({COLOR_SIZE, COLOR_SIZE})};
    for (int i = 0; i < tetromino.squareSize; i++)
    {
        for (int j = 0; j < tetromino.squareSize; j++)
        {
            if (tetromino.piece[i][j] == EMPTY || tetromino.pos.y + i < 0)
                continue;
            const float posX{startX + (tetromino.pos.x + j) * CELL_SIZE};
            const float posY{startY + (tetromino.pos.y + i) * CELL_SIZE};
            rectangle.setPosition({posX, posY});
            rectangle.setFillColor(enumToColor(tetromino.color));
            if (tetromino.color != TRANSPARENT)
            {
                rectangle.setOutlineThickness(RECTANGLE_OUTLINE_SIZE);
                rectangle.setOutlineColor(enumToColor(DARK_PURPLE));
            }

            window.draw(rectangle);
        }
    }
}

void printNextTetromino(sf::RenderWindow &window, const Tetromino &tetromino, float startX, float startY)
{
    const float previewBoxX{startX + GRID_WIDTH * CELL_SIZE + CELL_SIZE * 3};
    const float previewBoxY{startY + CELL_SIZE * 5};
    const float previewBoxSize{CELL_SIZE * 6};

    auto boxBg{sf::RectangleShape({previewBoxSize, previewBoxSize})};
    boxBg.setPosition({previewBoxX, previewBoxY});
    boxBg.setFillColor(enumToColor(EMPTY));
    boxBg.setOutlineColor(sf::Color::White);
    boxBg.setOutlineThickness(3.0f);
    window.draw(boxBg);

    auto label{sf::Text(roboto, "NEXT", 36)};
    label.setPosition({previewBoxX + 75, previewBoxY - 50});
    window.draw(label);

    auto rectangle{sf::RectangleShape({COLOR_SIZE, COLOR_SIZE})};

    const float pieceWidth{tetromino.squareSize * CELL_SIZE};
    const float pieceHeight{tetromino.squareSize * CELL_SIZE};

    const float offsetX{previewBoxX + (previewBoxSize - pieceWidth) / 2.0f};
    const float offsetYDenominator{(tetromino.id != 'O') ? 1.5f : 2.0f};
    const float offsetY{previewBoxY + (previewBoxSize - pieceHeight) / offsetYDenominator};

    for (int i = 0; i < tetromino.squareSize; i++)
    {
        for (int j = 0; j < tetromino.squareSize; j++)
        {
            if (tetromino.piece[i][j] == EMPTY)
                continue;
            const float posX{offsetX + j * CELL_SIZE};
            const float posY{offsetY + i * CELL_SIZE};

            rectangle.setPosition({posX, posY});
            rectangle.setFillColor(enumToColor(tetromino.color));
            rectangle.setOutlineThickness(RECTANGLE_OUTLINE_SIZE);
            rectangle.setOutlineColor(enumToColor(DARK_PURPLE));
            window.draw(rectangle);
        }
    }
}

void printGrid(sf::RenderWindow &window, float &startX, float &startY)
{
    constexpr float TOTAL_GRID_WIDTH{GRID_WIDTH * CELL_SIZE};
    constexpr float TOTAL_GRID_HEIGHT{GRID_HEIGHT * CELL_SIZE};

    static const float START_X{(window.getSize().x - TOTAL_GRID_WIDTH) / 2.0f};
    static const float START_Y{(window.getSize().y - TOTAL_GRID_HEIGHT) / 2.0f};

    startX = START_X;
    startY = START_Y;

    auto gridBg{sf::RectangleShape({TOTAL_GRID_WIDTH, TOTAL_GRID_HEIGHT})};
    gridBg.setPosition({START_X, START_Y});
    gridBg.setFillColor(enumToColor(EMPTY));
    gridBg.setOutlineColor(sf::Color::White);
    gridBg.setOutlineThickness(3.0f);
    window.draw(gridBg);

    auto rectangle{sf::RectangleShape({COLOR_SIZE, COLOR_SIZE})};
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            if (screenState[i][j] == EMPTY)
                continue;
            const float posX{START_X + j * CELL_SIZE};
            const float posY{START_Y + i * CELL_SIZE};

            rectangle.setPosition({posX, posY});
            rectangle.setFillColor(enumToColor(screenState[i][j]));
            rectangle.setOutlineThickness(RECTANGLE_OUTLINE_SIZE);
            rectangle.setOutlineColor(enumToColor(DARK_PURPLE));
            window.draw(rectangle);
        }
    }
}