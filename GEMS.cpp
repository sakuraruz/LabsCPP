#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <queue>
#include "GEMS.h"
using namespace std;


Cell::Cell() {
        
        shape.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));
        shape.setOutlineThickness(1);
        shape.setOutlineColor(sf::Color::Black);
        randomizeColor();
    }

bool Cell::isBonus() const { return false; }

Cell::~Cell() = default;

void Cell::randomizeColor() {
        int idx = rand() % Colors::COLORS;
        color = Colors::palette[idx];
        toBeDestroyed = false;
        shape.setFillColor(color);
    }
void Cell::draw(sf::RenderWindow& window) {
        window.draw(shape);
    }

void Cell::setPosition(int x, int y) {
        shape.setPosition(x * (CELL_SIZE + GAP), y * (CELL_SIZE + GAP));
        position = { x, y };
    }

    sf::Vector2i Cell::getPosition() const {
        return position;
    }

sf::Color Cell::getColor() const { return color; }
void Cell::setColor(const sf::Color& c) { color = c;  shape.setFillColor(color);}

    bool Cell::isToBeDestroyed() const { return toBeDestroyed; }
    void Cell::markToDestroy() { toBeDestroyed = true; }
    void Cell::clearDestroyMark() { toBeDestroyed = false; }

    void Cell::setSelected(int x, int y, bool selected) {
        if (selected)
        {
            shape.setOrigin(CELL_SIZE / 2.f, CELL_SIZE / 2.f);

            shape.setPosition(x * (CELL_SIZE + GAP) + CELL_SIZE / 2.f,
                y * (CELL_SIZE + GAP) + CELL_SIZE / 2.f);
            shape.setScale(1.2f, 1.2f);
        }
        else
        {
            shape.setOrigin(0.f, 0.f);
            shape.setPosition(x * (CELL_SIZE + GAP), y * (CELL_SIZE + GAP));
            shape.setScale(1.f, 1.f);
        }
    }


bool Bonus::isBonus() const { return true; }
Bonus::Bonus() : Cell() {
        clearDestroyMark();
    }
    bool Bonus::isActivated() const { return activated; };


    Bomb::Bomb() {
        setColor(sf::Color::White);
    }

    void Bomb::activate(vector<vector<Cell*>>& grid)  {
        activated = true;
        vector<Cell*> allCells;
        sf::Vector2i center = getPosition();
        for (auto& row : grid)
            for (auto& cell : row)
                if (cell && !dynamic_cast<Bonus*>(cell))
                    allCells.push_back(cell);

        random_shuffle(allCells.begin(), allCells.end());

        int count = min(5, (int)allCells.size());
        for (int i = 0; i < count; ++i) {
            auto pos = allCells[i]->getPosition();
            delete grid[pos.y][pos.x];
            grid[pos.y][pos.x] = nullptr;
        }
    }


    Recolor::Recolor(const sf::Color& originalColor) : sourceColor(originalColor) {
        setColor(originalColor);
        shape.setRadius(CELL_SIZE / 2.f);
    }

    void Recolor::activate(vector<vector<Cell*>>& grid)  {
        activated = true;
        sf::Vector2i center = getPosition();
        vector<Cell*> candidates;

        for (int dy = -3; dy <= 3; ++dy) {
            for (int dx = -3; dx <= 3; ++dx) {
                int nx = center.x + dx;
                int ny = center.y + dy;
                if (nx >= 0 && ny >= 0 && ny < grid.size() && nx < grid[0].size()) {
                    Cell* target = grid[ny][nx];
                    if (target && target != this && !isNeighbor(center, target->getPosition())) {
                        candidates.push_back(target);
                    }
                }
            }
        }

        random_shuffle(candidates.begin(), candidates.end());
        int recolored = 0;
        for (auto* c : candidates) {
            c->setColor(sourceColor);
            if (++recolored >= 2) break;
        }

        setColor(sourceColor);
        delete grid[center.y][center.x];
        grid[center.y][center.x] = nullptr;
    }

    void Recolor::setPosition(int x, int y) {
        shape.setPosition(x * (CELL_SIZE + GAP), y * (CELL_SIZE + GAP));
        position = { x, y };
    }

    void Recolor::setColor(const sf::Color& color)  {
        sourceColor = color;
        shape.setFillColor(color);
    }

    sf::Color Recolor::getColor() const  {
        return shape.getFillColor();
    }

    void Recolor::draw(sf::RenderWindow& window)  {
        window.draw(shape);
    }

    bool Recolor::isNeighbor(sf::Vector2i a, sf::Vector2i b) {
        return abs(a.x - b.x) + abs(a.y - b.y) == 1;
    }

    Board::Board(int width, int height, sf::RenderWindow& window)
        : width(width), height(height), window(window) {

        grid.resize(height, vector<Cell*>(width, nullptr));

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                grid[y][x] = new Cell();
                grid[y][x]->setPosition(x, y);
            }
        }
    }

    Board::~Board() {
        for (auto& row : grid) {
            for (auto* cell : row) {
                delete cell;
            }
        }
    }

    void Board::draw() {
        // Рисуем основную сетку
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (grid[y][x]) {
                    grid[y][x]->draw(window);
                }
            }
        }

        // Дополнительные эффекты анимации можно добавить здесь
        if (currentState == GameState::DESTROYING) {
            float progress = stateClock.getElapsedTime().asSeconds() / 0.5f;
            // Можно добавить эффекты исчезновения
        }
    }

    void Board::checkAndActivateBonuses() {
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (grid[y][x]->isToBeDestroyed()) {
                    if (Bonus* bonus = dynamic_cast<Bonus*>(grid[y][x])) {
                        bonus->activate(grid);
                    }
                    grid[y][x] = nullptr;
                }
            }
        }

        refillGrid();
    }


    void Board::refillGrid() {
        for (int x = 0; x < width; ++x) {
            int emptyY = height - 1;

            for (int y = height - 1; y >= 0; --y) {
                if (grid[y][x]) {
                    if (y != emptyY) {
                        grid[emptyY][x] = grid[y][x];
                        grid[emptyY][x]->setPosition(x, emptyY);
                        grid[y][x] = nullptr;
                    }
                    --emptyY;
                }
            }

            for (int y = emptyY; y >= 0; --y) {
                Cell* newCell = new Cell();
                newCell->setPosition(x, y);
                newCell->randomizeColor();
                grid[y][x] = newCell;
            }
        }
    }

    void Board::handleClick(int mouseX, int mouseY) {
        if (currentState != GameState::IDLE) return;

        int x = mouseX / (CELL_SIZE + GAP);
        int y = mouseY / (CELL_SIZE + GAP);

        if (x < 0 || x >= width || y < 0 || y >= height) return;

        Cell* cell = grid[y][x];
        if (!cell) return;

        if (Bonus* bonus = dynamic_cast<Bonus*>(cell)) {
            float elapsed = clickClock.getElapsedTime().asSeconds(); 
            if (lastClick == sf::Vector2i(x, y) && elapsed < doubleClickThreshold) {
                bonus->activate(grid);
                delete grid[y][x];
                grid[y][x] = nullptr;
                currentState = GameState::FALLING;
                stateClock.restart();
                lastClick = { -1, -1 };
                return;
            }
        }

        lastClick = { x, y };
        clickClock.restart();

        if (selected.x == -1) {
            if (!dynamic_cast<Bonus*>(grid[y][x])) {
                selected = { x, y };
                grid[y][x]->setSelected(x, y, true);
            }
        }
        else {
            int dx = abs(selected.x - x);
            int dy = abs(selected.y - y);
            bool isNeighbor = (dx + dy == 1);

            if (isNeighbor) {
                swap(grid[y][x], grid[selected.y][selected.x]);

                grid[y][x]->setPosition(x, y);
                grid[selected.y][selected.x]->setPosition(selected.x, selected.y);
                grid[y][x]->setSelected(x, y, false);
            }

            grid[selected.y][selected.x]->setSelected(selected.x, selected.y, false);
            selected = { -1, -1 };
        }
        if (selected.x == -1 && findAndMarkGroups()) {
            currentState = GameState::DESTROYING;
            stateClock.restart();

            for (auto& pos : cellsToDestroy) {
                int x = pos.x, y = pos.y;
                if (grid[y][x]) {
                    grid[y][x]->setColor(sf::Color::Black);
                }
            }
        }
    }

    void Board::destroyGroups() {
        if (currentState != GameState::IDLE) return;

        if (findAndMarkGroups()) {
            currentState = GameState::DESTROYING;
            stateClock.restart();


            for (auto& pos : cellsToDestroy) {
                int x = pos.x, y = pos.y;
                if (grid[y][x]) {
                    grid[y][x]->setColor(sf::Color::Black);
                }
            }
        }
    }

    void Board::update() {
        switch (currentState) {
        case GameState::IDLE:
            break;

        case GameState::DESTROYING:
            if (stateClock.getElapsedTime().asSeconds() > 0.5f) {
                for (auto& pos : cellsToDestroy) {
                    int x = pos.x, y = pos.y;
                    if (grid[y][x] && !dynamic_cast<Bonus*>(grid[y][x])) {
                        delete grid[y][x];
                        grid[y][x] = nullptr;
                    }
                }
                cellsToDestroy.clear();
                currentState = GameState::FALLING;
                stateClock.restart();
            }
            break;

        case GameState::FALLING:
            if (stateClock.getElapsedTime().asSeconds() > 0.3f) {
                refillGrid();
                currentState = GameState::CHECKING;
                stateClock.restart();
            }
            break;

        case GameState::CHECKING:
            if (stateClock.getElapsedTime().asSeconds() > 0.1f) {
                if (findAndMarkGroups()) {
                    currentState = GameState::DESTROYING;
                }
                else {
                    currentState = GameState::IDLE;
                }
                stateClock.restart();
            }
            break;
        }
    }

    bool Board::findAndMarkGroups() {
        cellsToDestroy.clear();
        vector<vector<bool>> visited(height, vector<bool>(width, false));
        bool foundGroups = false;

        const int dx[] = { 0, 0, -1, 1 };
        const int dy[] = { -1, 1, 0, 0 };

        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (grid[y][x] == nullptr || dynamic_cast<Bonus*>(grid[y][x]) || visited[y][x])
                    continue;

                sf::Color targetColor = grid[y][x]->getColor();
                queue<sf::Vector2i> q;
                vector<sf::Vector2i> groupCells;

                q.push(sf::Vector2i(x, y));
                visited[y][x] = true;

                while (!q.empty()) {
                    auto current = q.front();
                    q.pop();
                    groupCells.push_back(current);

                    for (int dir = 0; dir < 4; ++dir) {
                        int nx = current.x + dx[dir];
                        int ny = current.y + dy[dir];

                        if (nx < 0 || ny < 0 || nx >= width || ny >= height) continue;
                        if (visited[ny][nx] || grid[ny][nx] == nullptr) continue;
                        if (dynamic_cast<Bonus*>(grid[ny][nx])) continue;
                        if (grid[ny][nx]->getColor() != targetColor) continue;

                        visited[ny][nx] = true;
                        q.push(sf::Vector2i(nx, ny));
                    }
                }

                if (groupCells.size() >= 3) {
                    foundGroups = true;
                    cellsToDestroy.insert(cellsToDestroy.end(), groupCells.begin(), groupCells.end());

                    if ((rand() % 100) < 30) {
                        sf::Vector2i center = groupCells[groupCells.size() / 2];
                        vector<sf::Vector2i> possiblePositions;

                        // Собираем все клетки в радиусе 3
                        for (int dy = -3; dy <= 3; ++dy) {
                            for (int dx = -3; dx <= 3; ++dx) {
                                int nx = center.x + dx;
                                int ny = center.y + dy;

                                if (nx >= 0 && ny >= 0 && nx < width && ny < height) {
                                    bool isInGroup = std::any_of(
                                        groupCells.begin(), groupCells.end(),
                                        [nx, ny](const sf::Vector2i& p) {
                                            return p.x == nx && p.y == ny;
                                        });

                                    if (!isInGroup) {
                                        possiblePositions.emplace_back(nx, ny);
                                    }
                                }
                            }
                        }

                        if (!possiblePositions.empty()) {
                            auto bonusPos = possiblePositions[rand() % possiblePositions.size()];

                            if (grid[bonusPos.y][bonusPos.x]) {
                                delete grid[bonusPos.y][bonusPos.x];
                            }

                            if (rand() % 2 == 0) {
                                grid[bonusPos.y][bonusPos.x] = new Bomb();
                                grid[bonusPos.y][bonusPos.x]->setColor(sf::Color::White);
                            }
                            else {
                                sf::Color originalColor = grid[groupCells[0].y][groupCells[0].x]->getColor();
                                grid[bonusPos.y][bonusPos.x] = new Recolor(originalColor);
                            }
                            grid[bonusPos.y][bonusPos.x]->setPosition(bonusPos.x, bonusPos.y);
                        }
                    }
                }
            }
        }

        return foundGroups;
    }


int main(void) {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Match-3 Game");
    Board board(10, 10, window);
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                int mouseX = event.mouseButton.x;
                int mouseY = event.mouseButton.y;
                board.handleClick(mouseX, mouseY);
            }
        }

        // Обновление состояния игры
        board.update();

        // Отрисовка
        window.clear();
        board.draw();
        window.display();
    }

    return 0;
}