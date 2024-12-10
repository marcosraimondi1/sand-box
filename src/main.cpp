#include "Vector2D.hpp"
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Window.hpp>
#include <iostream>
#include <tuple>
#include <utility>

#define MAX_HEIGHT 500
#define MAX_WIDTH 500

class Component {
  public:
    bool exists = false; // if entity contains component, defaults to false
};

class CTransform : public Component {
  public:
    Vector2D position = {0, 0};
    Vector2D velocity = {0, 0};
    Vector2D acceleration = {0, 0};
    CTransform() {};
    CTransform(const Vector2D &p, const Vector2D &v, const Vector2D &a)
        : position(p), velocity(v), acceleration(a) {};
};

class CShape : public Component {
  public:
    sf::RectangleShape shape;
    CShape() {};
};

using ComponentTuple = std::tuple<CTransform, CShape>;

class Entity {
    ComponentTuple m_components;
    bool m_alive = true; // if false delete entity

  public:
    Entity() {};

    template <typename T, typename... TArgs> T &add(TArgs &&...mArgs) {
        // TArgs allows for forwarding the constructor parameters
        auto &component = get<T>();
        component = T(std::forward<TArgs>(mArgs)...);
        component.exists = true;
        return component;
    };
    template <typename T> T &get() { return std::get<T>(m_components); };
    template <typename T> bool has() { return get<T>().exists; };
    template <typename T> void remove() {
        get<T>() = T(); // defaults exists to false
    };
};

class Systems {
  public:
    static void sMovement(std::vector<Entity> &entities, float elapsed) {
        for (auto &e : entities) {
            if (!e.has<CTransform>() || !e.has<CShape>())
                continue;
            auto &t = e.get<CTransform>();
            auto s = e.get<CShape>();
            t.velocity += t.acceleration * elapsed;
            t.position += t.velocity * elapsed;

            if (t.position.m_y >= MAX_HEIGHT - s.shape.getSize().y)
                t.position.m_y = MAX_HEIGHT - s.shape.getSize().y;
        }
    };

    static void sDraw(std::vector<Entity> &entities, sf::RenderWindow &window) {
        window.clear();
        for (auto &e : entities) {
            if (!e.has<CTransform>() || !e.has<CShape>())
                continue;
            auto t = e.get<CTransform>();
            auto s = e.get<CShape>();
            s.shape.setPosition({t.position.m_x, t.position.m_y});
            window.draw(s.shape);
        }
        window.display();
    };
};

int main() {

    sf::ContextSettings settings;
    settings.antialiasingLevel = 8;
    auto window =
        sf::RenderWindow({MAX_WIDTH, MAX_HEIGHT}, "CMake SFML Project",
                         sf::Style::Close, settings);
    window.setFramerateLimit(60);

    sf::Clock clock;

    Systems sys;
    std::vector<Entity> entities;

    bool isMousePressed = false;
    while (window.isOpen()) {
        for (auto event = sf::Event(); window.pollEvent(event);) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isMousePressed = true;
                }
            } else if (event.type == sf::Event::MouseButtonReleased) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    isMousePressed = false;
                }
            } else if (event.type == sf::Event::MouseMoved) {
                if (isMousePressed) {
                    Entity grain = Entity();
                    auto &s = grain.add<CShape>();
                    auto &t = grain.add<CTransform>();
                    s.shape.setSize({10, 10});
                    s.shape.setFillColor(
                        sf::Color(rand() % 255, rand() % 255, rand() % 255));
                    t.position = Vector2D(event.mouseMove.x, event.mouseMove.y);
                    t.acceleration = Vector2D(0, 100);

                    entities.push_back(grain);
                }
            }
        }

        sf::Time elapsed = clock.restart();

        // Update here
        sys.sMovement(entities, elapsed.asSeconds());

        // Draw here
        sys.sDraw(entities, window);
    }
}
