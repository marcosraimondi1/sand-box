#include <SFML/Graphics.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Window/WindowStyle.hpp>
#include <cstdint>
#include <iostream>

#define MAX_HEIGHT 500
#define MAX_WIDTH 500

class Vector2D {

  public:
    float m_x;
    float m_y;

    Vector2D(float x, float y) : m_x(x), m_y(y) {};
    Vector2D() : m_x(0), m_y(0) {};

    Vector2D operator+(const Vector2D &vec) {
        return Vector2D(m_x + vec.m_x, m_y + vec.m_y);
    };

    Vector2D operator*(float num) { return Vector2D(m_x * num, m_y * num); };
};

class SandGrain {
    Vector2D m_velocity;
    Vector2D m_position;
    Vector2D m_acceleration;
    static const int m_size = 10;

  public:
    SandGrain(Vector2D initialPosition) {
        m_position = initialPosition;
        m_acceleration = Vector2D(0, 10 * 100);
    };

    void update(float elapsed) {
        m_velocity = m_velocity + m_acceleration * elapsed;
        m_position = m_position + m_velocity * elapsed;

        if (m_position.m_y >= MAX_HEIGHT - m_size)
            m_position.m_y = MAX_HEIGHT - m_size;

        if (m_position.m_x >= MAX_WIDTH - m_size)
            m_position.m_x = MAX_WIDTH - m_size;

        if (m_position.m_x <= 0)
            m_position.m_x = 0;
    };

    void draw(sf::RenderWindow &window) {
        sf::RectangleShape shape(sf::Vector2f(m_size, m_size));
        shape.setPosition(sf::Vector2f(m_position.m_x, m_position.m_y));
        window.draw(shape);
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

    std::vector<SandGrain> sand;
    sand.push_back(SandGrain({0, 0}));

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
                if (isMousePressed)
                    sand.push_back(
                        SandGrain({static_cast<float>(event.mouseMove.x),
                                   static_cast<float>(event.mouseMove.y)}));
            }
        }

        sf::Time elapsed = clock.restart();
        // Update here
        for (SandGrain &grain : sand) {
            grain.update(elapsed.asSeconds());
        }
        window.clear();

        // Draw here
        for (SandGrain &grain : sand) {
            grain.draw(window);
        }
        window.display();
    }
}
