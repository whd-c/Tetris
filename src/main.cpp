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

struct Tetromino
{
    int squareSize;
    Color color;
    char id;

    int gridX = 0;
    int gridY = 0;

    std::vector<std::vector<Color>> piece;

    Tetromino(char _id)
        : id(_id) {}
    void setAttributes(int _squareSize, Color _color)
    {
        squareSize = _squareSize;
        piece.resize(squareSize);
        for (int i = 0; i < squareSize; i++)
        {
            piece[i].resize(squareSize, EMPTY);
        }
        color = _color;
    }
};
sf::Color enumToColor(Color choice);
void initializeTetrominoes();
std::vector<Tetromino> generateBag();
Tetromino rotatedTetromino(const Tetromino &currentTetromino, int iterations = 1);
std::optional<Tetromino> newTetromino(const Tetromino &tetromino);
bool isValidPosition(const Tetromino &tetromino);
void handleCollision(const Tetromino &tetromino);
void handleWreck(Tetromino &tetromino, std::vector<Tetromino> &bag, int &score, sf::Text &textScore);
void clearRows(int &score, sf::Text &textScore);
void printTetromino(sf::RenderWindow &window, const Tetromino &tetromino, float startX, float startY);
void printNextTetromino(sf::RenderWindow &window, const Tetromino &tetromino, float startX, float startY);
void printGrid(sf::RenderWindow &window, float &gridX, float &gridY);

std::vector<Tetromino> tetrominoes = {
    {'O'},
    {'I'},
    {'S'},
    {'Z'},
    {'L'},
    {'J'},
    {'T'},
};

constexpr int GRID_WIDTH = 10;
constexpr int GRID_HEIGHT = 20;

constexpr unsigned int RWIDTH = 1920;
constexpr unsigned int RHEIGHT = 1080;

constexpr float COLOR_SIZE = 40.0f;
constexpr float SPACING = 0.0f;
constexpr float CELL_SIZE = COLOR_SIZE + SPACING;
constexpr float RECTANGLE_OUTLINE_SIZE = -1.5f;

// init with EMPTY (0)
std::array<std::array<Color, GRID_WIDTH>, GRID_HEIGHT> screenState = {};

sf::Font roboto;

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({RWIDTH, RHEIGHT}), "Tetris", sf::State::Fullscreen);
    int score = 0;
    window.setFramerateLimit(144);

    sf::Image icon;
    if (!icon.loadFromFile("favicon/favicon.png"))
    {
        std::cerr << "Failed to load icon\n";
        return 1;
    }

    window.setIcon(icon);
    sf::Clock clock;

    if (!roboto.openFromFile("fonts/Roboto/Roboto-VariableFont_wdth,wght.ttf"))
    {
        std::cerr << "Failed to load font\n";
        return 1;
    }

    sf::Text textScore(roboto);

    textScore.setString("Score: " + std::to_string(score));
    textScore.setCharacterSize(96);

    initializeTetrominoes();
    auto bag = generateBag();
    std::optional<Tetromino> tempTetromino;
    if (newTetromino(*bag.begin()))
        tempTetromino = *newTetromino(*bag.begin());
    bag.erase(bag.begin());
    Tetromino currentTetromino = *tempTetromino;
    while (window.isOpen())
    {
        sf::Time elapsed = clock.getElapsedTime();
        if (elapsed.asSeconds() > 0.4)
        {
            Tetromino next = currentTetromino;
            next.gridY += 1;
            if (isValidPosition(next))
            {
                currentTetromino.gridY = next.gridY;
            }
            else
            {
                handleCollision(currentTetromino);
                handleWreck(currentTetromino, bag, score, textScore);
            }
            clock.restart();
        }
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
            else if (const auto *keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->scancode == sf::Keyboard::Scancode::Escape)
                    window.close();
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Up || keyPressed->scancode == sf::Keyboard::Scancode::W)
                {
                    Tetromino rotated = rotatedTetromino(currentTetromino);
                    if (isValidPosition(rotated))
                    {
                        currentTetromino = rotated;
                    }
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Right || keyPressed->scancode == sf::Keyboard::Scancode::D)
                {
                    Tetromino next = currentTetromino;
                    next.gridX += 1;
                    if (isValidPosition(next))
                    {
                        currentTetromino.gridX = next.gridX;
                    }
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Down || keyPressed->scancode == sf::Keyboard::Scancode::S)
                {
                    Tetromino next = currentTetromino;
                    next.gridY += 1;
                    if (isValidPosition(next))
                    {
                        currentTetromino.gridY = next.gridY;
                    }
                    else
                    {
                        handleCollision(currentTetromino);
                        handleWreck(currentTetromino, bag, score, textScore);
                    }
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Left || keyPressed->scancode == sf::Keyboard::Scancode::A)
                {
                    Tetromino next = currentTetromino;
                    next.gridX -= 1;
                    if (isValidPosition(next))
                    {
                        currentTetromino.gridX = next.gridX;
                    }
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Space)
                {
                    while (isValidPosition(currentTetromino))
                    {
                        currentTetromino.gridY++;
                    }
                    currentTetromino.gridY--;
                    handleCollision(currentTetromino);
                    handleWreck(currentTetromino, bag, score, textScore);
                }
            }
        }

        float gridStartX, gridStartY;
        Tetromino ghostTetromino = currentTetromino;
        ghostTetromino.color = TRANSPARENT;
        while (isValidPosition(ghostTetromino))
        {
            ghostTetromino.gridY++;
        }
        ghostTetromino.gridY--;

        window.clear();

        float textScoreX = gridStartX + GRID_WIDTH * CELL_SIZE + CELL_SIZE * 2;
        float textScoreY = gridStartY;
        textScore.setPosition({textScoreX, textScoreY});

        printGrid(window, gridStartX, gridStartY);
        clearRows(score, textScore);
        printTetromino(window, ghostTetromino, gridStartX, gridStartY);
        printTetromino(window, currentTetromino, gridStartX, gridStartY);
        printNextTetromino(window, bag[0], gridStartX, gridStartY);
        window.draw(textScore);
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
            tetromino.setAttributes(2, YELLOW);
            tetromino.piece[0][0] = YELLOW;
            tetromino.piece[0][1] = YELLOW;
            tetromino.piece[1][0] = YELLOW;
            tetromino.piece[1][1] = YELLOW;
            break;
        case 'I':
            tetromino.setAttributes(4, CYAN);
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            tetromino.piece[1][3] = tetromino.color;
            break;
        case 'S':
            tetromino.setAttributes(3, GREEN);
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[0][2] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            break;
        case 'Z':
            tetromino.setAttributes(3, RED);
            tetromino.piece[0][0] = tetromino.color;
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            break;
        case 'L':
            tetromino.setAttributes(3, ORANGE);
            tetromino.piece[0][2] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            break;
        case 'J':
            tetromino.setAttributes(3, BLUE);
            tetromino.piece[0][0] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            break;
        case 'T':
            tetromino.setAttributes(3, PURPLE);
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
    std::vector<Tetromino> bag = tetrominoes;

    std::random_device rd;
    std::mt19937 mt(rd());
    std::shuffle(bag.begin(), bag.end(), mt);
    return bag;
}

Tetromino rotatedTetromino(const Tetromino &currentTetromino, int iterations)
{
    Tetromino rotatedTetromino = currentTetromino;

    iterations = ((iterations % 4) + 4) % 4;

    for (int it = 0; it < iterations; it++)
    {
        Tetromino temp = rotatedTetromino;
        rotatedTetromino.squareSize = temp.squareSize;
        rotatedTetromino.squareSize = temp.squareSize;

        for (int i = 0; i < temp.squareSize; i++)
        {
            for (int j = 0; j < temp.squareSize; j++)
            {
                rotatedTetromino.piece[rotatedTetromino.squareSize - 1 - j][i] = temp.piece[i][j];
            }
        }
    }

    return rotatedTetromino;
}

std::optional<Tetromino> newTetromino(const Tetromino &tetromino)
{
    Tetromino temp = tetromino;
    temp.gridX = (GRID_WIDTH - temp.squareSize) / 2;

    temp.gridY = 0;
    if (tetromino.id == 'I')
        temp.gridY--;
    for (int i = 1; i < temp.squareSize; i++)
    {
        for (int j = 0; j < temp.squareSize; j++)
        {
            if (screenState[temp.gridY + i][temp.gridX + j] != EMPTY)
            {
                return std::nullopt;
            }
        }
    }

    return temp;
}

bool isValidPosition(const Tetromino &tetromino)
{
    for (int i = 0; i < tetromino.squareSize; i++)
    {
        for (int j = 0; j < tetromino.squareSize; j++)
        {
            if (tetromino.piece[i][j] != EMPTY)
            {
                int gridX = tetromino.gridX + j;
                int gridY = tetromino.gridY + i;

                if (gridX < 0 || gridX >= GRID_WIDTH)
                {
                    return false;
                }

                if (gridY >= GRID_HEIGHT || screenState[gridY][gridX] != EMPTY)
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void handleCollision(const Tetromino &tetromino)
{
    for (int i = 0; i < tetromino.squareSize; i++)
    {
        for (int j = 0; j < tetromino.squareSize; j++)
        {
            if (tetromino.piece[i][j] != EMPTY)
            {
                int gridX = tetromino.gridX + j;
                int gridY = tetromino.gridY + i;

                screenState[gridY][gridX] = tetromino.piece[i][j];
            }
        }
    }
}

void handleWreck(Tetromino &tetromino, std::vector<Tetromino> &bag, int &score, sf::Text &textScore)
{
    std::optional<Tetromino> nextTetromino = newTetromino(bag[0]);
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
        textScore.setString("Score: " + std::to_string(score));
    }
    if (nextTetromino)
    {
        tetromino = *nextTetromino;
        bag.erase(bag.begin());
    }
    if (bag.size() <= 0)
        bag = generateBag();
}

void clearRows(int &score, sf::Text &textScore)
{
    int writeRow = GRID_HEIGHT - 1;
    for (int i = GRID_HEIGHT - 1; i >= 0; i--)
    {
        bool fullRow = true;
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
            score += 100;
            textScore.setString("Score: " + std::to_string(score));
        }
    }

    for (int i = writeRow; i >= 0; i--)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            screenState[i][j] = EMPTY;
        }
    }
}

void printTetromino(sf::RenderWindow &window, const Tetromino &tetromino, float startX, float startY)
{
    auto rectangle = sf::RectangleShape({COLOR_SIZE, COLOR_SIZE});
    for (int i = 0; i < tetromino.squareSize; i++)
    {
        for (int j = 0; j < tetromino.squareSize; j++)
        {
            if (tetromino.piece[i][j] == EMPTY)
                continue;
            const float posX = startX + (tetromino.gridX + j) * CELL_SIZE;
            const float posY = startY + (tetromino.gridY + i) * CELL_SIZE;
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
    const float previewBoxX = startX + GRID_WIDTH * CELL_SIZE + CELL_SIZE * 3;
    const float previewBoxY = startY + CELL_SIZE * 5;
    const float previewBoxSize = CELL_SIZE * 6;

    auto boxBg = sf::RectangleShape({previewBoxSize, previewBoxSize});
    boxBg.setPosition({previewBoxX, previewBoxY});
    boxBg.setFillColor(enumToColor(EMPTY));
    boxBg.setOutlineColor(enumToColor(DARK_PURPLE));
    boxBg.setOutlineThickness(3.0f);
    window.draw(boxBg);

    auto label = sf::Text(roboto, "NEXT", 36);
    label.setPosition({previewBoxX + 75, previewBoxY - 50});
    window.draw(label);

    auto rectangle = sf::RectangleShape({COLOR_SIZE, COLOR_SIZE});

    const float pieceWidth = tetromino.squareSize * CELL_SIZE;
    const float pieceHeight = tetromino.squareSize * CELL_SIZE;

    const float offsetX = previewBoxX + (previewBoxSize - pieceWidth) / 2.0f;
    float offsetYDenominator = (tetromino.id != 'O') ? 1.5f : 2.0f;
    const float offsetY = previewBoxY + (previewBoxSize - pieceHeight) / offsetYDenominator;

    for (int i = 0; i < tetromino.squareSize; i++)
    {
        for (int j = 0; j < tetromino.squareSize; j++)
        {
            if (tetromino.piece[i][j] == EMPTY)
                continue;
            const float posX = offsetX + j * CELL_SIZE;
            const float posY = offsetY + i * CELL_SIZE;

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
    constexpr float TOTAL_GRID_WIDTH = GRID_WIDTH * CELL_SIZE;
    constexpr float TOTAL_GRID_HEIGHT = GRID_HEIGHT * CELL_SIZE;

    static const float START_X = (window.getSize().x - TOTAL_GRID_WIDTH) / 2.0f;
    static const float START_Y = (window.getSize().y - TOTAL_GRID_HEIGHT) / 2.0f;

    startX = START_X;
    startY = START_Y;

    auto gridBg = sf::RectangleShape({TOTAL_GRID_WIDTH, TOTAL_GRID_HEIGHT});
    gridBg.setPosition({START_X, START_Y});
    gridBg.setFillColor(enumToColor(EMPTY));
    gridBg.setOutlineColor(enumToColor(DARK_PURPLE));
    gridBg.setOutlineThickness(3.0f);
    window.draw(gridBg);

    auto rectangle = sf::RectangleShape({COLOR_SIZE, COLOR_SIZE});
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            if (screenState[i][j] == EMPTY)
                continue;
            const float posX = START_X + j * CELL_SIZE;
            const float posY = START_Y + i * CELL_SIZE;

            rectangle.setPosition({posX, posY});
            rectangle.setFillColor(enumToColor(screenState[i][j]));
            rectangle.setOutlineThickness(RECTANGLE_OUTLINE_SIZE);
            rectangle.setOutlineColor(enumToColor(DARK_PURPLE));
            window.draw(rectangle);
        }
    }
}