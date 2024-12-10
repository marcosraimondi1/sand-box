#pragma once
#include <cmath>

class Vector2D {

  public:
    float m_x = 0;
    float m_y = 0;

    Vector2D(float x, float y) : m_x(x), m_y(y) {};
    Vector2D() {};

    float lenght() { return sqrt(m_x * m_x + m_y * m_y); }

    Vector2D &add(const Vector2D &vec) {
        m_x += vec.m_x;
        m_y += vec.m_y;
        return *this; // reference to this instance, allows chaining operations
    };

    Vector2D &scale(float scale) {
        m_x *= scale;
        m_y *= scale;
        return *this; // reference to this instance, allows chaining operations
    };

    void operator+=(const Vector2D &vec) {
        m_x += vec.m_x;
        m_y += vec.m_y;
    };

    void operator-=(const Vector2D &vec) {
        m_x -= vec.m_x;
        m_y -= vec.m_y;
    };

    Vector2D operator+(const Vector2D &vec) {
        return Vector2D(m_x + vec.m_x, m_y + vec.m_y);
    };

    Vector2D operator*(float scale) {
        return Vector2D(m_x * scale, m_y * scale);
    };
};
