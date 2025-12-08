#pragma once
#include <SFML/Graphics.hpp>
#include <string_view>

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

// SRS offset data for all tetrominos except I
constexpr std::array<std::array<Position, 5>, 4> offsetData{{{{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}},
                                                             {{{0, 0}, {1, 0}, {1, -1}, {0, 2}, {1, 2}}},
                                                             {{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}}},
                                                             {{{0, 0}, {-1, 0}, {-1, -1}, {0, 2}, {-1, 2}}}}};
// SRS kick table for the I tetromino
constexpr std::array<std::array<Position, 5>, 4> kickTableICW{{{{{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}}},
                                                               {{{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}}},
                                                               {{{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}}},
                                                               {{{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}}}}};

constexpr std::array<std::array<Position, 5>, 4> kickTableICCW{{{{{0, 0}, {-1, 0}, {2, 0}, {-1, 2}, {2, -1}}},
                                                                {{{0, 0}, {2, 0}, {-1, 0}, {2, 1}, {-1, -2}}},
                                                                {{{0, 0}, {1, 0}, {-2, 0}, {1, -2}, {-2, 1}}},
                                                                {{{0, 0}, {-2, 0}, {1, 0}, {-2, -1}, {1, 2}}}}};

constexpr uint8_t GRID_WIDTH{10};
constexpr uint8_t GRID_HEIGHT{20};

constexpr uint16_t WINDOW_WIDTH{1152};
constexpr uint16_t WINDOW_HEIGHT{648};
constexpr uint16_t TARGET_WIDTH{1920};
constexpr uint16_t TARGET_HEIGHT{1080};
constexpr uint8_t FRAME_RATE{60};
constexpr std::string_view WINDOW_TITLE{"Tetris"};

constexpr float DELAY{1.0f};
constexpr float LOCK_DELAY{0.5f};
constexpr uint8_t LOCK_LIMIT{10};

constexpr float COLOR_SIZE{40.0f};
constexpr float SPACING{0.0f};
constexpr float CELL_SIZE{COLOR_SIZE + SPACING};
constexpr float RECTANGLE_OUTLINE_SIZE{-1.5f};

sf::Color enumToColor(Color choice);