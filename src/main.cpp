#include <iostream>
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

constexpr int GRID_WIDTH = 10;
constexpr int GRID_HEIGHT = 20;

constexpr float BLOCK_SIZE = 25.0f;
constexpr float SPACING = 0.0f;
constexpr float CELL_SIZE = BLOCK_SIZE + SPACING;

Block screenState[GRID_HEIGHT][GRID_WIDTH];

std::optional<sf::Color> enumToColor(Block choice);

int main()
{
    auto window = sf::RenderWindow(sf::VideoMode({1280u, 720u}), "Tetris");
    window.setFramerateLimit(60);
    sf::RectangleShape rectangle({BLOCK_SIZE, BLOCK_SIZE});

    constexpr float TOTAL_GRID_WIDTH = GRID_WIDTH * CELL_SIZE;
    constexpr float TOTAL_GRID_HEIGHT = GRID_HEIGHT * CELL_SIZE;

    const float START_X = (window.getSize().x - TOTAL_GRID_WIDTH) / 2.0f;
    const float START_Y = (window.getSize().y - TOTAL_GRID_HEIGHT) / 2.0f;

    const auto onClose = [&window](const sf::Event::Closed &)
    {
        window.close();
    };

    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            window.handleEvents(onClose);
        }

        window.clear();
        for (int i = 0; i < GRID_HEIGHT; i++)
        {
            for (int j = 0; j < GRID_WIDTH; j++)
            {
                const float posX = START_X + j * CELL_SIZE;
                const float posY = START_Y + i * CELL_SIZE;

                rectangle.setPosition({posX, posY});

                int choice = rand() % 8;

                if (auto color = enumToColor((Block)choice))
                {
                    rectangle.setFillColor(*color);
                }
                window.draw(rectangle);
            }
        }

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