#include <iostream>
#include <vector>
#include <SFML/Graphics.hpp>

enum Block
{
    EMPTY,
    CYAN,
    BLUE,
    ORANGE,
    YELLOW,
    GREEN,
    PURPLE,
    RED
};

struct Tetromino
{
    int height;
    int width;
    Block color;
    char id;

    int gridX = 0;
    int gridY = 0;

    std::vector<std::vector<Block>> piece;

    Tetromino(char _id)
        : id(_id) {}
};

std::optional<sf::Color> enumToColor(Block choice);
void initializeTetrominoes();
Tetromino rotatedTetromino(const Tetromino &currentTetromino, int iterations = 1);
Tetromino newTetromino(const Tetromino &tetromino);
bool isValidPosition(const Tetromino &tetromino);
void handleCollision(const Tetromino &tetromino);
void printTetromino(sf::RenderWindow &window, const Tetromino &tetromino, float startX, float startY);
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

constexpr float BLOCK_SIZE = 25.0f;
constexpr float SPACING = 0.0f;
constexpr float CELL_SIZE = BLOCK_SIZE + SPACING;

Block screenState[GRID_HEIGHT * 2][GRID_WIDTH];

int main()
{
    srand(time(NULL));
    auto window = sf::RenderWindow(sf::VideoMode({1280u, 720u}), "Tetris", sf::State::Windowed);
    window.setFramerateLimit(60);

    sf::Clock clock;

    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            screenState[i][j] = EMPTY;
        }
    }

    initializeTetrominoes();
    Tetromino currentTetromino = newTetromino(tetrominoes[rand() % 7]);
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
                currentTetromino = newTetromino(tetrominoes[rand() % 7]);
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
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Up)
                {
                    Tetromino rotated = rotatedTetromino(currentTetromino);
                    if (isValidPosition(rotated))
                    {
                        currentTetromino = rotated;
                    }
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Right)
                {
                    Tetromino next = currentTetromino;
                    next.gridX += 1;
                    if (isValidPosition(next))
                    {
                        currentTetromino.gridX = next.gridX;
                    }
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Down)
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
                        currentTetromino = newTetromino(tetrominoes[rand() % 7]);
                    }
                }
                else if (keyPressed->scancode == sf::Keyboard::Scancode::Left)
                {
                    Tetromino next = currentTetromino;
                    next.gridX -= 1;
                    if (isValidPosition(next))
                    {
                        currentTetromino.gridX = next.gridX;
                    }
                }
            }
        }

        float gridStartX, gridStartY;

        window.clear();
        printGrid(window, gridStartX, gridStartY);
        printTetromino(window, currentTetromino, gridStartX, gridStartY);
        std::cout << currentTetromino.gridX << " " << currentTetromino.gridY << "\n";
        window.display();
    }
}

std::optional<sf::Color> enumToColor(Block choice)
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
    default:
        return {};
    }
}
void initializeTetrominoes()
{
    for (Tetromino &tetromino : tetrominoes)
    {
        switch (tetromino.id)
        {
        case 'O':
            tetromino.height = 2;
            tetromino.width = 2;
            tetromino.piece.resize(tetromino.height);
            for (int i = 0; i < tetromino.height; i++)
            {
                tetromino.piece[i].resize(tetromino.width, EMPTY);
            }
            tetromino.color = YELLOW;
            tetromino.piece[0][0] = YELLOW;
            tetromino.piece[0][1] = YELLOW;
            tetromino.piece[1][0] = YELLOW;
            tetromino.piece[1][1] = YELLOW;
            break;
        case 'I':
            tetromino.height = 4;
            tetromino.width = 4;
            tetromino.piece.resize(tetromino.height);
            for (int i = 0; i < tetromino.height; i++)
            {
                tetromino.piece[i].resize(tetromino.width, EMPTY);
            }
            tetromino.color = CYAN;
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[2][1] = tetromino.color;
            tetromino.piece[3][1] = tetromino.color;
            break;
        case 'S':
            tetromino.height = 3;
            tetromino.width = 3;
            tetromino.piece.resize(tetromino.height);
            for (int i = 0; i < tetromino.height; i++)
            {
                tetromino.piece[i].resize(tetromino.width, EMPTY);
            }
            tetromino.color = GREEN;
            tetromino.piece[0][0] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[2][1] = tetromino.color;
            break;
        case 'Z':
            tetromino.height = 3;
            tetromino.width = 3;
            tetromino.piece.resize(tetromino.height);
            for (int i = 0; i < tetromino.height; i++)
            {
                tetromino.piece[i].resize(tetromino.width, EMPTY);
            }
            tetromino.color = RED;
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[2][0] = tetromino.color;
            break;
        case 'L':
            tetromino.height = 3;
            tetromino.width = 3;
            tetromino.piece.resize(tetromino.height);
            for (int i = 0; i < tetromino.height; i++)
            {
                tetromino.piece[i].resize(tetromino.width, EMPTY);
            }
            tetromino.color = ORANGE;
            tetromino.piece[0][0] = tetromino.color;
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[2][1] = tetromino.color;
            break;
        case 'J':
            tetromino.height = 3;
            tetromino.width = 3;
            tetromino.piece.resize(tetromino.height);
            for (int i = 0; i < tetromino.height; i++)
            {
                tetromino.piece[i].resize(tetromino.width, EMPTY);
            }
            tetromino.color = BLUE;
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[2][0] = tetromino.color;
            tetromino.piece[2][1] = tetromino.color;
            break;
        case 'T':
            tetromino.height = 3;
            tetromino.width = 3;
            tetromino.piece.resize(tetromino.height);
            for (int i = 0; i < tetromino.height; i++)
            {
                tetromino.piece[i].resize(tetromino.width, EMPTY);
            }
            tetromino.color = PURPLE;
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[2][1] = tetromino.color;
            break;
        }
    }
}

Tetromino rotatedTetromino(const Tetromino &currentTetromino, int iterations)
{
    Tetromino rotatedTetromino = currentTetromino;

    iterations = ((iterations % 4) + 4) % 4;

    for (int it = 0; it < iterations; it++)
    {
        Tetromino temp = rotatedTetromino;
        rotatedTetromino.height = temp.width;
        rotatedTetromino.width = temp.height;

        for (size_t i = 0; i < temp.height; i++)
        {
            for (size_t j = 0; j < temp.width; j++)
            {
                rotatedTetromino.piece[rotatedTetromino.height - 1 - j][i] = temp.piece[i][j];
            }
        }
    }

    return rotatedTetromino;
}

Tetromino newTetromino(const Tetromino &tetromino)
{
    Tetromino temp = tetromino;
    temp.gridX = (GRID_WIDTH - temp.width) / 2 + ((tetromino.id != 'O') ? 1 : 0);
    temp.gridY = 0;
    return temp;
}

bool isValidPosition(const Tetromino &tetromino)
{
    for (int i = 0; i < tetromino.height; i++)
    {
        for (int j = 0; j < tetromino.width; j++)
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
    for (int i = 0; i < tetromino.height; i++)
    {
        for (int j = 0; j < tetromino.width; j++)
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

void printTetromino(sf::RenderWindow &window, const Tetromino &tetromino, float startX, float startY)
{
    auto rectangle = sf::RectangleShape({BLOCK_SIZE, BLOCK_SIZE});
    for (int i = 0; i < tetromino.height; i++)
    {
        for (int j = 0; j < tetromino.width; j++)
        {
            if (tetromino.piece[i][j] == EMPTY)
                continue;
            float posX = startX + (tetromino.gridX + j) * CELL_SIZE;
            float posY = startY + (tetromino.gridY + i) * CELL_SIZE;
            rectangle.setPosition({posX, posY});
            rectangle.setFillColor(*enumToColor(tetromino.piece[i][j]));
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

    auto rectangle = sf::RectangleShape({BLOCK_SIZE, BLOCK_SIZE});
    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            const float posX = START_X + j * CELL_SIZE;
            const float posY = START_Y + i * CELL_SIZE;

            rectangle.setPosition({posX, posY});

            if (auto color = enumToColor(screenState[i][j]))
            {
                rectangle.setFillColor(*color);
            }
            window.draw(rectangle);
        }
    }
}