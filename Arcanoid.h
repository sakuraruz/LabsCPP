#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace sf;
using namespace std;

template<typename T>
T clamp(T val, T minVal, T maxVal) {
    if (val < minVal) return minVal;
    if (val > maxVal) return maxVal;
    return val;
}

const int WIDTH = 800;
const int HEIGHT = 600;
const int PADDLE_WIDTH = 100;
const int PADDLE_HEIGHT = 20;
const float BALL_RADIUS = 10.f;
const float BALL_SPEED = 300.f;
const float BONUS_SPEED = 3.f;
const int BRICK_WIDTH = 60;
const int BRICK_HEIGHT = 20;
const int BRICK_ROWS = 5;
const int BRICK_COLS = 10;

enum BlockType {
    UNBREAKABLE,
    REGULAR,
    BONUS,
    SPEED_CHANGER
};


enum BonusType {
    PADDLE_SIZE_INC,
    PADDLE_SIZE_DEC,
    BALL_SPEED_INC,
    BALL_SPEED_DEC,
    STICKY_BALL,
    EXTRA_LIFE,
    RANDOM_DIRECTION
};


#include <memory>

class GameObject {
protected:
    unique_ptr<Shape> shape;
    bool active;

public:
    GameObject();

    virtual ~GameObject() = default;


    virtual void setPosition(const Vector2f& pos);

    virtual Vector2f getPosition() const;

    virtual Vector2f getSize() const = 0;
    virtual void setSize(const Vector2f& size) = 0;

    bool isActive() const;
    void setActive(bool a);

    virtual void draw(RenderWindow& window);

    virtual void update(float deltaTime) = 0;

    bool checkCollision(const GameObject& obj) const;
};



class Ball : public GameObject {
private:
    Vector2f position;
    float radius;
    CircleShape shapeObj;
    Vector2f velocity;
    bool sticky;

public:
    Ball(const Vector2f& position, float radius);

    void reset(const Vector2f& pos, float r);

    void setPosition(const Vector2f& pos) override;

    Vector2f getPosition() const override;

    Vector2f getSize() const override;
    void setSize(const Vector2f& size) override;

    void setVelocity(const Vector2f& vel);
    const Vector2f& getVelocity() const;

    void setSticky(bool s);
    bool isSticky() const;

    void reverseX();
    void reverseY();

    void increaseSpeed();

    void decreaseSpeed();

    void launch();

    void update(float deltaTime) override;

    void draw(RenderWindow& window) override;

    void randomizeDirection();
};



class Paddle : public GameObject {
private:
    RectangleShape shapeObj;
    float speed;

public:
    Paddle(const Vector2f& position);

    void moveLeft(float deltaTime);

    void moveRight(float deltaTime);

    void setWidth(float newWidth);

    Vector2f getPosition() const override;

    Vector2f getSize() const override;

    void setSize(const Vector2f& size) override;

    void update(float deltaTime) override;

    void draw(RenderWindow& window) override;
};


class Block : public GameObject {
private:
    BlockType type;
    int health;
    int maxHealth;
    bool hasBonus;
    RectangleShape shapeObj;

public:
    Block(const Vector2f& position, BlockType t, int hp = 1, bool bonus = false);

    Block() = delete;

    Block(const Block& other);


    Block& operator=(const Block& other);

    BlockType getType() const;
    int getHealth() const;

    bool hit();

    bool hasBonusInside() const;
    void setBonus(bool b);

    Vector2f getPosition() const override;

    Vector2f getSize() const override;

    void setSize(const Vector2f& size) override;

    void draw(RenderWindow& window) override;

    void update(float deltaTime) override;

private:
    void updateColor();
};



class Bonus : public GameObject {
private:
    BonusType type;
    float fallSpeed;
    Vector2f velocity;
    CircleShape shapeObj;

public:
    Bonus(const Vector2f& position, BonusType t);


    Bonus(const Bonus& other);

    Bonus& operator=(const Bonus& other);

    void update(float deltaTime) override;

    void draw(RenderWindow& window) override;

    string getTypeSymbol() const;

    BonusType getType() const;
    void setColor(Color color);

    Vector2f getPosition() const override;

    Vector2f getSize() const override;

    void setSize(const Vector2f& size) override;
};


class ArcanoidGame {
private:
    RenderWindow window;
    Paddle paddle;
    Ball ball;
    vector<Block> blocks;
    vector<Bonus> bonuses;
    int score;
    int lives;
    bool gameOver;
    bool bottomProtection;
    bool paused;
    Font font;
    Text scoreText;
    Text livesText;
    Text messageText;
    Clock gameClock;

    void generateLevel();

    void checkCollisions();

    void checkPaddleCollision();

    void checkBlocksCollision();

    void spawnBonus(const Block& block);

    void checkBottomCollision();

    void resetBall();

    void checkBonusesCollision();

    void applyBonus(BonusType type);


    void updateUI();

public:
    ArcanoidGame();

    void run();

    void processInput(float deltaTime);

    void update(float deltaTime);
    void render();
};
