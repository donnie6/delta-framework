#include <delta/graphics/graphics.hpp>
#include <delta/utils/readFile.hpp>
#include <glad/gl.h>
#include <print>
#include <delta/delta.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <array>

namespace Delta {

Renderer::Renderer(const int viewportWidth, const int viewportHeight) {
    // Vertex Shader
    const std::string vertexCode = readFile("delta/shader/shader.vert");
    const char* vertexShaderCode = vertexCode.c_str();

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderCode, nullptr);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::println(stderr, "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n{}", infoLog);
    }

    // Fragment Shader
    const std::string fragmentCode = readFile("delta/shader/shader.frag");
    const char* fragmentShaderCode = fragmentCode.c_str();

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderCode, nullptr);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLog);
        std::println(stderr, "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n{}", infoLog);
    }

    // Shader Program
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glGetShaderiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::println(stderr, "ERROR::SHADER::PROGRAM::COMPILATION_FAILED\n{}", infoLog);
    }

    glUseProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    std::array<glm::vec2, 4> rectVertices {{
        {0.0f, 0.0f}, // top-left
        {1.0f, 0.0f}, // top-right
        {1.0f, 1.0f}, // bottom-right
        {0.0f, 1.0f}, // bottom-left
    }};
    std::array<uint16_t, 6> indices {0, 1, 2, 2, 3, 0};

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, rectVertices.size() * sizeof(glm::vec2), rectVertices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint16_t), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(0);

    glViewport(0, 0, viewportWidth, viewportHeight);
    projection = glm::ortho(0.0f, static_cast<float>(viewportWidth), static_cast<float>(viewportHeight), 0.0f);

    uTransform = glGetUniformLocation(shaderProgram, "uTransform");
    uColor = glGetUniformLocation(shaderProgram, "uColor");
}

Renderer::~Renderer() {
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteProgram(shaderProgram);
}

void Renderer::clear(Color color) {
    glClearColor(color.red, color.green, color.blue, color.alpha);
    glClear(GL_COLOR_BUFFER_BIT);
};

void Renderer::drawRect(const Rect& rect) {
    glm::mat4 model = {1.0f};
    model = glm::translate(model, {rect.position, 0.0f});
    model = glm::scale(model, {rect.width, rect.height, 1.0f});
    glm::mat4 transform = projection * model;

    glUseProgram(shaderProgram);
    glUniformMatrix4fv(uTransform, 1, GL_FALSE, value_ptr(transform));
    glUniform4f(uColor, rect.tint.red, rect.tint.green, rect.tint.blue, rect.tint.alpha);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}
} // namespace Delta
