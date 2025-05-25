#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "Arcanoid.h"

using namespace sf;
using namespace std;

GameObject::GameObject() : shape(nullptr), active(true) {}

GameObject::~GameObject() = default;


void GameObject::setPosition(const Vector2f& pos) {
        if (shape)
            shape->setPosition(pos);
    }

Vector2f GameObject::getPosition() const {
        return shape ? shape->getPosition() : Vector2f(0.f, 0.f);
    }

    bool GameObject::isActive() const { return active; }
    void GameObject::setActive(bool a) { active = a; }

void GameObject::draw(RenderWindow& window) {
        if (shape)
            window.draw(*shape);
    }


    bool GameObject::checkCollision(const GameObject& obj) const {
        Vector2f pos1 = getPosition();
        Vector2f size1 = getSize();
        Vector2f pos2 = obj.getPosition();
        Vector2f size2 = obj.getSize();

        return (pos1.x < pos2.x + size2.x &&
            pos1.x + size1.x > pos2.x &&
            pos1.y < pos2.y + size2.y &&
            pos1.y + size1.y > pos2.y);
    }



Ball::Ball(const Vector2f& position, float radius)
        : shapeObj(radius), velocity(0.f, 0.f), sticky(true)
    {
        active = true;
        shapeObj.setPosition(position);
        shapeObj.setFillColor(Color::White);
    }

    void Ball::reset(const Vector2f& pos, float r) {
        position = pos;
        radius = r;
        shapeObj.setRadius(radius);
        shapeObj.setOrigin(radius, radius);
        shapeObj.setPosition(position);

        velocity = Vector2f(0.f, 0.f);
        sticky = false;
    }

    void Ball::setPosition(const Vector2f& pos) {
        shapeObj.setPosition(pos);
    }

    Vector2f Ball::getPosition() const {
        return shapeObj.getPosition();
    }

    Vector2f Ball::getSize() const {
        float diameter = shapeObj.getRadius() * 2.f;
        return { diameter, diameter };
    }

    void Ball::setSize(const Vector2f& size) {
        shapeObj.setRadius(size.x / 2.f);
    }

    void Ball::setVelocity(const Vector2f& vel) { velocity = vel; }
    const Vector2f& Ball::getVelocity() const { return velocity; }

    void Ball::setSticky(bool s) { sticky = s; }
    bool Ball::isSticky() const { return sticky; }

    void Ball::reverseX() { velocity.x = -velocity.x; }
    void Ball::reverseY() { velocity.y = -velocity.y; }

    void Ball::increaseSpeed() {
        velocity.x *= 1.2f;
        velocity.y *= 1.2f;
    }

    void Ball::decreaseSpeed() {
        velocity.x *= 0.8f;
        velocity.y *= 0.8f;
    }

    void Ball::launch() {
        if (sticky) {
            velocity = Vector2f(
                (rand() % 2 == 0) ? BALL_SPEED : -BALL_SPEED,
                -BALL_SPEED
            );
            sticky = false;
        }
    }

    void Ball::update(float deltaTime) {
        if (!sticky) {
            Vector2f newPos = shapeObj.getPosition() + velocity * deltaTime;

            if (newPos.x <= 0 || newPos.x >= WIDTH - shapeObj.getRadius() * 2.f) {
                reverseX();
                newPos.x = (newPos.x <= 0) ? 0 : WIDTH - shapeObj.getRadius() * 2.f;
            }

            if (newPos.y <= 0) {
                reverseY();
                newPos.y = 0;
            }

            if (newPos.y >= HEIGHT) {
                setActive(false);
            }

            shapeObj.setPosition(newPos);
        }
    }

    void Ball::draw(RenderWindow& window)  {
        if (active) window.draw(shapeObj);
    }

    void Ball::randomizeDirection() {
        float angle = static_cast<float>(rand() % 360) * 3.14159f / 180.f;
        float speed = sqrt(velocity.x * velocity.x + velocity.y * velocity.y);

        velocity.x = speed * cos(angle);
        velocity.y = speed * sin(angle);

        if (abs(velocity.y) < 50.f) {
            velocity.y = (velocity.y < 0 ? -1.f : 1.f) * 50.f;
        }
    }




Paddle::Paddle(const Vector2f& position)
        : shapeObj({ PADDLE_WIDTH, PADDLE_HEIGHT }), speed(500.f)
    {
        active = true;
        shapeObj.setFillColor(Color::Green);
        shapeObj.setPosition(position);
    }

    void Paddle::moveLeft(float deltaTime) {
        shapeObj.move(-speed * deltaTime, 0);
        if (shapeObj.getPosition().x < 0)
            shapeObj.setPosition(0, shapeObj.getPosition().y);
    }

    void Paddle::moveRight(float deltaTime) {
        shapeObj.move(speed * deltaTime, 0);
        float maxX = WIDTH - shapeObj.getSize().x;
        if (shapeObj.getPosition().x > maxX)
            shapeObj.setPosition(maxX, shapeObj.getPosition().y);
    }

    void Paddle::setWidth(float newWidth) {
        float clampedWidth = clamp(newWidth, 50.f, 200.f);
        Vector2f pos = shapeObj.getPosition();
        shapeObj.setSize({ clampedWidth, PADDLE_HEIGHT });

        float maxX = WIDTH - clampedWidth;
        if (pos.x > maxX) pos.x = maxX;

        shapeObj.setPosition(pos);
    }

    Vector2f Paddle::getPosition() const {
        return shapeObj.getPosition();
    }

    Vector2f Paddle::getSize() const{
        return shapeObj.getSize();
    }

    void Paddle::setSize(const Vector2f& size) {
        shapeObj.setSize(size);
    }

    void Paddle::update(float deltaTime) {
    }

    void Paddle::draw(RenderWindow& window) {
        if (active)
            window.draw(shapeObj);
    }


Block::Block(const Vector2f& position, BlockType t, int hp = 1, bool bonus = false)
        : type(t), health(hp), maxHealth(hp), hasBonus(bonus), shapeObj({ BRICK_WIDTH, BRICK_HEIGHT })
    {
        active = true;
        shapeObj.setPosition(position);
        updateColor();
    }

Block::Block(const Block& other)
        :
        type(other.type),
        health(other.health),
        maxHealth(other.maxHealth),
        hasBonus(other.hasBonus),
        shapeObj(other.shapeObj)
    {
    }


    Block& Block::operator=(const Block& other) {
        if (this == &other) return *this;


        type = other.type;
        health = other.health;
        maxHealth = other.maxHealth;
        hasBonus = other.hasBonus;
        shapeObj = other.shapeObj;

        return *this;
    }


    BlockType Block::getType() const { return type; }
    int Block::getHealth() const { return health; }

    bool Block::hit() {
        if (type == UNBREAKABLE) return false;

        health--;
        updateColor();
        if (health <= 0) {
            active = false;
            return true;
        }
        return false;
    }

    bool Block::hasBonusInside() const { return hasBonus && active; }
    void Block::setBonus(bool b) { hasBonus = b; }

    Vector2f Block::getPosition() const {
        return shapeObj.getPosition();
    }

    Vector2f Block::getSize() const {
        return shapeObj.getSize();
    }

    void Block::setSize(const Vector2f& size) {
        shapeObj.setSize(size);
    }

    void Block::draw(RenderWindow& window) {
        if (active) {
            window.draw(shapeObj);
        }
    }

    void Block::update(float deltaTime) {}

    void Block::updateColor() {
        switch (type) {
        case UNBREAKABLE:
            shapeObj.setFillColor(Color(100, 100, 100));
            break;
        case REGULAR:
            if (maxHealth == 1)
                shapeObj.setFillColor(Color::Red);
            else if (maxHealth == 2)
                shapeObj.setFillColor(Color(255, 165, 0));
            else
                shapeObj.setFillColor(Color::Yellow);
            break;
        case BONUS:
            shapeObj.setFillColor(Color::Blue);
            break;
        case SPEED_CHANGER:
            shapeObj.setFillColor(Color::Magenta);
            break;
        }
    }




Bonus::Bonus(const Vector2f& position, BonusType t)
        : type(t), fallSpeed(100.f), velocity({ 20.f, 20.f }), shapeObj(10.f) 
    {
        active = true;
        shapeObj.setPosition(position);
        shapeObj.setOutlineThickness(2);
        shapeObj.setOutlineColor(Color::Black);

        switch (type) {
        case PADDLE_SIZE_INC:  shapeObj.setFillColor(Color::Cyan); break;
        case PADDLE_SIZE_DEC:  shapeObj.setFillColor(Color::Magenta); break;
        case BALL_SPEED_INC:   shapeObj.setFillColor(Color::White); break;
        case BALL_SPEED_DEC:   shapeObj.setFillColor(Color(200, 200, 200)); break;
        case STICKY_BALL:      shapeObj.setFillColor(Color::Green); break;
        case EXTRA_LIFE:       shapeObj.setFillColor(Color(255, 215, 0)); break;
        case RANDOM_DIRECTION: shapeObj.setFillColor(Color(255, 100, 100)); break;
        default:               shapeObj.setFillColor(Color::Yellow);
        }
    }
Bonus::Bonus(const Bonus& other)
        : type(other.type), fallSpeed(other.fallSpeed), velocity(other.velocity),
        shapeObj(other.shapeObj.getRadius())
    {
        active = other.active;
        shapeObj.setPosition(other.shapeObj.getPosition());
        shapeObj.setOutlineThickness(other.shapeObj.getOutlineThickness());
        shapeObj.setOutlineColor(other.shapeObj.getOutlineColor());
        shapeObj.setFillColor(other.shapeObj.getFillColor());
    }

    Bonus& Bonus::operator=(const Bonus& other) {
        if (this == &other) return *this;

        type = other.type;
        fallSpeed = other.fallSpeed;
        velocity = other.velocity;
        active = other.active;

        shapeObj = CircleShape(other.shapeObj.getRadius());
        shapeObj.setPosition(other.shapeObj.getPosition());
        shapeObj.setOutlineThickness(other.shapeObj.getOutlineThickness());
        shapeObj.setOutlineColor(other.shapeObj.getOutlineColor());
        shapeObj.setFillColor(other.shapeObj.getFillColor());

        return *this;
    }

    void Bonus::update(float deltaTime) {
        if (!active) return;

        shapeObj.move(0.f, fallSpeed * deltaTime);

        if (getPosition().y > HEIGHT) {
            active = false;
        }
    }

    void Bonus::draw(RenderWindow& window) {
        if (!active) return;

        window.draw(shapeObj);

        static Font font;
        if (font.getInfo().family.empty()) font.loadFromFile("roboto.ttf");

        Text bonusText;
        bonusText.setFont(font);
        bonusText.setCharacterSize(12);
        bonusText.setString(getTypeSymbol());
        bonusText.setFillColor(Color::Black);
        bonusText.setPosition(
            shapeObj.getPosition().x + shapeObj.getRadius() - 5.f,
            shapeObj.getPosition().y + shapeObj.getRadius() - 8.f
        );
        window.draw(bonusText);
        
    }

    string Bonus::getTypeSymbol() const {
        switch (type) {
        case PADDLE_SIZE_INC: return "W+";
        case PADDLE_SIZE_DEC: return "W-";
        case BALL_SPEED_INC: return "S+";
        case BALL_SPEED_DEC: return "S-";
        case STICKY_BALL: return "STK";
        case EXTRA_LIFE: return "L+";
        case RANDOM_DIRECTION: return "DIR";
        default: return "?";
        }
    }

    BonusType Bonus::getType() const { return type; }
    void Bonus::setColor(Color color) { shapeObj.setFillColor(color); }

    Vector2f Bonus::getPosition() const {
        return shapeObj.getPosition();
    }

    Vector2f Bonus::getSize() const {
        float radius = shapeObj.getRadius();
        return { radius, radius };
    }

    void Bonus::setSize(const Vector2f& size) {
        shapeObj.setRadius(size.x);
    }



    void ArcanoidGame::generateLevel() {
        blocks.clear();

        for (int row = 0; row < BRICK_ROWS; row++) {
            for (int col = 0; col < BRICK_COLS; col++) {
                float x = col * (BRICK_WIDTH + 5) + 50;
                float y = row * (BRICK_HEIGHT + 5) + 50;

                BlockType type;
                int health = 1;
                bool hasBonus = false;

                int r = rand() % 10;
                if (r < 1) {
                    type = UNBREAKABLE;
                }
                else if (r < 3) {
                    type = SPEED_CHANGER;
                }
                else if (r < 6) {
                    type = BONUS;
                    hasBonus = true;
                }
                else {
                    type = REGULAR;
                    health = 2 + rand() % 2;
                }

                blocks.emplace_back(Vector2f(x, y), type, health, hasBonus);
    
            }
        }
    }

    void ArcanoidGame::checkCollisions() {
        if (!ball.isActive()) return;

        checkPaddleCollision();
        checkBlocksCollision();
        checkBottomCollision();
        checkBonusesCollision();
    }

    void ArcanoidGame::checkPaddleCollision() {
        if (!ball.checkCollision(paddle)) return;

        float paddleCenter = paddle.getPosition().x + paddle.getSize().x / 2;
        float ballCenter = ball.getPosition().x + BALL_RADIUS;
        float hitPos = (ballCenter - paddleCenter) / (paddle.getSize().x / 2);
        hitPos = clamp(hitPos, -1.0f, 1.0f);

        float angle = hitPos * (3.14f / 3.0f);

        float speed = max(
            200.0f,
            sqrt(
                ball.getVelocity().x * ball.getVelocity().x +
                ball.getVelocity().y * ball.getVelocity().y
            ) * 1.05f);

        Vector2f newVelocity(sin(angle) * speed, -cos(angle) * speed);
        ball.setVelocity(newVelocity);

        ball.setPosition({ ball.getPosition().x, paddle.getPosition().y - BALL_RADIUS * 2 - 1 });
    }

    void ArcanoidGame::checkBlocksCollision() {
        for (auto& block : blocks) {
            if (!block.isActive()) continue;

            if (ball.checkCollision(block)) {
                bool destroyed = block.hit();
                if (destroyed && block.getType() != UNBREAKABLE) {
                    score += 100;
                }

                ball.reverseY();

                if (block.getType() == SPEED_CHANGER) {
                    ball.increaseSpeed();
                }

                if (destroyed && block.hasBonusInside()) {
                    spawnBonus(block);
                }

                break;
            }
        }
    }

    void ArcanoidGame::spawnBonus(const Block& block) {
        BonusType type = static_cast<BonusType>(rand() % 7);
        Vector2f bonusPos(
            block.getPosition().x + block.getSize().x / 2 - 10,
            block.getPosition().y + block.getSize().y
        );
        bonuses.emplace_back(bonusPos, type);
    }

    void ArcanoidGame::checkBottomCollision() {
        if (ball.getPosition().y < HEIGHT) return;

        if (bottomProtection) {
            ball.reverseY();
            bottomProtection = false;
            return;
        }

        ball.setActive(false);
        lives--;

        if (lives <= 0) {
            gameOver = true;
        }
        else {
            resetBall();
        }
    }

    void ArcanoidGame::resetBall() {
        ball.reset(paddle.getPosition() + Vector2f(paddle.getSize().x / 2 - BALL_RADIUS, -BALL_RADIUS * 2), BALL_RADIUS);
        ball.setSticky(true);
    }

    void ArcanoidGame::checkBonusesCollision() {
        for (auto& bonus : bonuses) {
            if (bonus.checkCollision(paddle)) {
                applyBonus(bonus.getType());
                bonus.setActive(false);
            }
        }

        bonuses.erase(
            remove_if(bonuses.begin(), bonuses.end(),
                [](const Bonus& b) { return !b.isActive(); }),
            bonuses.end()
        );
    }

    void ArcanoidGame::applyBonus(BonusType type) {
        switch (type) {
        case PADDLE_SIZE_INC:
            paddle.setWidth(paddle.getSize().x + 30);
            break;
        case PADDLE_SIZE_DEC:
            paddle.setWidth(max(30.0f, paddle.getSize().x - 30));
            break;
        case BALL_SPEED_INC:
            ball.increaseSpeed();
            break;
        case BALL_SPEED_DEC:
            ball.decreaseSpeed();
            break;
        case STICKY_BALL:
            ball.setSticky(true);
            break;
        case EXTRA_LIFE:
            lives++;
            break;
        case RANDOM_DIRECTION:
            ball.randomizeDirection();
            break;
        default:
            break;
        }
    }


    void ArcanoidGame::updateUI() {
        scoreText.setString("Score: " + to_string(score));
        livesText.setString("Lives: " + to_string(lives));
    }

    ArcanoidGame::ArcanoidGame()
        : window(VideoMode(WIDTH, HEIGHT), "Arcanoid"),
        paddle(Vector2f(WIDTH / 2 - PADDLE_WIDTH / 2, HEIGHT - 40)),
        score(0), lives(3), gameOver(false),
        bottomProtection(false), paused(false), ball({ paddle.getPosition().x + paddle.getSize().x / 2 - BALL_RADIUS,
            paddle.getPosition().y - BALL_RADIUS * 2 }, BALL_RADIUS) {

        window.setFramerateLimit(60);

        if (!font.loadFromFile("roboto.ttf")) {
            font = Font();
        }

        scoreText.setFont(font);
        scoreText.setCharacterSize(24);
        scoreText.setFillColor(Color::White);
        scoreText.setPosition(10, 10);

        livesText.setFont(font);
        livesText.setCharacterSize(24);
        livesText.setFillColor(Color::White);
        livesText.setPosition(WIDTH - 150, 10);

        messageText.setFont(font);
        messageText.setCharacterSize(40);
        messageText.setFillColor(Color::White);
        messageText.setPosition(WIDTH / 2 - 100, HEIGHT / 2 - 50);

        ball.setSticky(true);

        generateLevel();
    }

    void ArcanoidGame::run() {
        while (window.isOpen()) {
            float deltaTime = gameClock.restart().asSeconds();
            processInput(deltaTime);

            if (!paused && !gameOver) {
                update(deltaTime);
            }

            render();
        }
    }

    void ArcanoidGame::processInput(float deltaTime) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }
            else if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Escape) {
                    window.close();
                }
                else if (event.key.code == Keyboard::Space) {
                    if (paused) {
                        paused = false;
                    }
                    else {
                        paused = true;
                    }
                        if (ball.isSticky()) {
                            ball.launch();
                        }
                    
                }
            }
        }

        if (paused || gameOver) return;

        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            paddle.moveLeft(deltaTime);
                if (ball.isSticky()) {
                    ball.setPosition(Vector2f(paddle.getPosition().x + paddle.getSize().x / 2 - BALL_RADIUS,
                        ball.getPosition().y));
                }
        }
        if (Keyboard::isKeyPressed(Keyboard::Right)) {
            paddle.moveRight(deltaTime);
                if (ball.isSticky()) {
                    ball.setPosition(Vector2f(paddle.getPosition().x + paddle.getSize().x / 2 - BALL_RADIUS,
                        ball.getPosition().y));
                }
        }
    }

    void ArcanoidGame::update(float deltaTime) {
        paddle.update(deltaTime);
        ball.update(deltaTime);
        for (auto& block : blocks) block.update(deltaTime);
        for (auto& bonus : bonuses) bonus.update(deltaTime);

        checkCollisions();


        bool levelComplete = true;
        for (const auto& block : blocks) {
            if (block.isActive() && block.getType() != UNBREAKABLE) {
                levelComplete = false;
                break;
            }
        }

        if (levelComplete) {
            score += 1000;
            generateLevel();
            ball.setPosition({ paddle.getPosition().x + paddle.getSize().x / 2 - BALL_RADIUS,
            paddle.getPosition().y - BALL_RADIUS * 2 });
            ball.setSize({ BALL_RADIUS,BALL_RADIUS });
            ball.setSticky(true);
        }

        updateUI();
    }

    void ArcanoidGame::render() {
        window.clear(Color::Black);


        paddle.draw(window);
        ball.draw(window);
        for (auto& block : blocks) block.draw(window);
        for (auto& bonus : bonuses) bonus.draw(window);

        // Отрисовка UI
        window.draw(scoreText);
        window.draw(livesText);

        if (gameOver) {
            messageText.setString("GAME OVER!\nFinal score: " + to_string(score));
            FloatRect textRect = messageText.getLocalBounds();
            messageText.setOrigin(textRect.left + textRect.width / 2.0f,
                textRect.top + textRect.height / 2.0f);
            messageText.setPosition(WIDTH / 2.0f, HEIGHT / 2.0f);
            window.draw(messageText);
        }
        else if (paused) {
            messageText.setString("PAUSED\nPress SPACE to continue");
            FloatRect textRect = messageText.getLocalBounds();
            messageText.setOrigin(textRect.left + textRect.width / 2.0f,
                textRect.top + textRect.height / 2.0f);
            messageText.setPosition(WIDTH / 2.0f, HEIGHT / 2.0f);
            window.draw(messageText);
        }

        window.display();
    }

int main() {
    srand(static_cast<unsigned>(time(nullptr)));
    ArcanoidGame game;
    game.run();
    return 0;
}