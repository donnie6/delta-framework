#pragma once

#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

namespace Delta {
struct Color {
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;
    float alpha = 1.0f;
};

struct Rect {
    glm::vec2 position = {0.0f, 0.0f};
    float width = 0.0f;
    float height = 0.0f;
    Color tint;
};

struct Renderer {
    Renderer(int viewportWidth, int viewportHeight);
    ~Renderer();
    void clear(Color color);
    void drawRect(const Rect& rect);

private:
    unsigned int shaderProgram = 0;

    unsigned int VBO = 0;
    unsigned int EBO = 0;
    unsigned int VAO = 0;

    int uTransform = -1;
    int uColor = -1;
    glm::mat4 projection = {1.0f};
};
} // namespace Delta
