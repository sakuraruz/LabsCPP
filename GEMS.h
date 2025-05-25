#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <queue>
using namespace std;


const int TILE_SIZE = 64;
const int BOARD_WIDTH = 8;
const int BOARD_HEIGHT = 8;
const int COLORS = 5;
const int CELL_SIZE = 50;
const int GAP = 10;

namespace Colors {
    const int COLORS = 5;
    const sf::Color palette[COLORS] = {
        sf::Color::Red, sf::Color::Green, sf::Color::Blue,
        sf::Color::Yellow, sf::Color::Magenta
    };
}

enum class GameState {
    IDLE,
    DESTROYING,
    FALLING,
    CHECKING
};

class Cell {
protected:
    sf::Color color;
    bool toBeDestroyed;
    sf::Vector2i position;
    sf::RectangleShape shape;

public:
    Cell();

    virtual bool isBonus() const;

    virtual ~Cell() = default;

    virtual void randomizeColor();

    virtual void draw(sf::RenderWindow& window);

    virtual void setPosition(int x, int y);

    sf::Vector2i getPosition() const;

    virtual sf::Color getColor() const;
    virtual void setColor(const sf::Color& c);

    bool isToBeDestroyed() const;
    void markToDestroy();
    void clearDestroyMark();

    void setSelected(int x, int y, bool selected);

};

class Bonus : public Cell {
protected:
    bool activated = false;
public:
    bool isBonus() const override;
    Bonus();
    bool isActivated() const;
    virtual void activate(vector<vector<Cell*>>& grid) = 0;
    virtual ~Bonus() = default;
};

class Bomb : public Bonus {
public:
    Bomb();

    void activate(vector<vector<Cell*>>& grid) override;
};

class Recolor : public Bonus {
private:
    sf::Color sourceColor;
    sf::CircleShape shape;
public:
    Recolor(const sf::Color& originalColor);

    void activate(vector<vector<Cell*>>& grid) override;

    void setPosition(int x, int y) override;

    void setColor(const sf::Color& color) override;

    sf::Color getColor() const override;

    void draw(sf::RenderWindow& window) override;

private:
    bool isNeighbor(sf::Vector2i a, sf::Vector2i b);
};

class Board {
private:
    vector<vector<Cell*>> grid;
    int width, height;
    sf::RenderWindow& window;
    sf::Vector2i selected{ -1, -1 };
    sf::Vector2i lastClick = { -1, -1 };
    const float doubleClickThreshold = 0.3f;
    GameState currentState = GameState::IDLE;
    sf::Clock stateClock;
    vector<sf::Vector2i> cellsToDestroy;
    vector<vector<Cell*>> gridCopy;
    sf::Clock clickClock;
public:
    Board(int width, int height, sf::RenderWindow& window);

    ~Board();

    void draw();

    void checkAndActivateBonuses();


    void refillGrid();

    void handleClick(int mouseX, int mouseY);

    void destroyGroups();

    void update();

    bool findAndMarkGroups();
};
