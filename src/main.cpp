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

    std::vector<std::vector<Block>> piece;

    Tetromino(char _id)
        : id(_id) {}
};

std::optional<sf::Color> enumToColor(Block choice);
void initializeTetrominoes();
void printTetromino(sf::RenderWindow &window);
void printGrid(sf::RenderWindow &window);

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

Block screenState[GRID_HEIGHT][GRID_WIDTH];

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1280u, 720u}), "Tetris", sf::State::Windowed);
    window.setFramerateLimit(60);

    for (int i = 0; i < GRID_HEIGHT; i++)
    {
        for (int j = 0; j < GRID_WIDTH; j++)
        {
            screenState[i][j] = EMPTY;
        }
    }

    initializeTetrominoes();

    while (window.isOpen())
    {
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
            }
        }

        window.clear();
        printGrid(window);
        printTetromino(window);
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
            tetromino.width = 1;
            tetromino.piece.resize(tetromino.height);
            for (int i = 0; i < tetromino.height; i++)
            {
                tetromino.piece[i].resize(tetromino.width, EMPTY);
            }
            tetromino.color = CYAN;
            tetromino.piece[0][0] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[2][0] = tetromino.color;
            tetromino.piece[3][0] = tetromino.color;
            break;
        case 'S':
            tetromino.height = 2;
            tetromino.width = 3;
            tetromino.piece.resize(tetromino.height);
            for (int i = 0; i < tetromino.height; i++)
            {
                tetromino.piece[i].resize(tetromino.width, EMPTY);
            }
            tetromino.color = RED;
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[0][2] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            break;
        case 'Z':
            tetromino.height = 2;
            tetromino.width = 3;
            tetromino.piece.resize(tetromino.height);
            for (int i = 0; i < tetromino.height; i++)
            {
                tetromino.piece[i].resize(tetromino.width, EMPTY);
            }
            tetromino.color = GREEN;
            tetromino.piece[0][0] = tetromino.color;
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            tetromino.piece[1][2] = tetromino.color;
            break;
        case 'L':
            tetromino.height = 3;
            tetromino.width = 2;
            tetromino.piece.resize(tetromino.height);
            for (int i = 0; i < tetromino.height; i++)
            {
                tetromino.piece[i].resize(tetromino.width, EMPTY);
            }
            tetromino.color = ORANGE;
            tetromino.piece[0][0] = tetromino.color;
            tetromino.piece[1][0] = tetromino.color;
            tetromino.piece[2][0] = tetromino.color;
            tetromino.piece[2][1] = tetromino.color;
            break;
        case 'J':
            tetromino.height = 3;
            tetromino.width = 2;
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
            tetromino.height = 2;
            tetromino.width = 3;
            tetromino.piece.resize(tetromino.height);
            for (int i = 0; i < tetromino.height; i++)
            {
                tetromino.piece[i].resize(tetromino.width, EMPTY);
            }
            tetromino.color = PURPLE;
            tetromino.piece[0][0] = tetromino.color;
            tetromino.piece[0][1] = tetromino.color;
            tetromino.piece[0][2] = tetromino.color;
            tetromino.piece[1][1] = tetromino.color;
            break;
        }
    }
}

void printTetromino(sf::RenderWindow &window)
{
    auto rectangle = sf::RectangleShape({BLOCK_SIZE, BLOCK_SIZE});
    float startX = 50.0f;
    float startY = 50.0f;
    float spacing = BLOCK_SIZE;

    for (Tetromino &tetromino : tetrominoes)
    {
        for (int i = 0; i < tetromino.height; i++)
        {
            for (int j = 0; j < tetromino.width; j++)
            {

                float posX = startX + j * CELL_SIZE;
                float posY = startY + i * CELL_SIZE;

                rectangle.setPosition({posX, posY});
                rectangle.setFillColor(*enumToColor(tetromino.piece[i][j]));
                window.draw(rectangle);
            }
        }

        startX += tetromino.width * CELL_SIZE + spacing;
    }
}

void printGrid(sf::RenderWindow &window)
{
    constexpr float TOTAL_GRID_WIDTH = GRID_WIDTH * CELL_SIZE;
    constexpr float TOTAL_GRID_HEIGHT = GRID_HEIGHT * CELL_SIZE;

    static const float START_X = (window.getSize().x - TOTAL_GRID_WIDTH) / 2.0f;
    static const float START_Y = (window.getSize().y - TOTAL_GRID_HEIGHT) / 2.0f;

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