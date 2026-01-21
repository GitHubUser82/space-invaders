#include <glad/gl.h>
#include <stdint.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdlib.h>

#include <array>

/// @brief Minimum width of the program's window.
static constexpr int WindowMinWidth = 640;
/// @brief Minimum height of the program's window.
static constexpr int WindowMinHeight = 480;

/// @brief The emulator's native screen width (distinct from user window width).
static constexpr int EmulatorFramebufferWidth = 256;
/// @brief The emulator's native screen height (distinct from user window height).
static constexpr int EmulatorFramebufferHeight = 224;

struct Pixel {
    uint8_t r, g, b, a;
};

/// @brief The emulator's framebuffer.
std::array<Pixel, EmulatorFramebufferWidth * EmulatorFramebufferHeight> framebuffer;

static const struct {
    float x, y;
    float u, v; // texture coordinates
} framebufferQuad[4] = {
    {-1.f, -1.f, 0.f, 1.f}, {-1.f, 1.f, 0.f, 0.f}, {1.f, 1.f, 1.f, 0.f}, {1.f, -1.f, 1.f, 1.f}};

const unsigned int indices[] = {0, 1, 3, 1, 2, 3};

static const char *vertex_shader_text = "#version 330 core\n"
                                        "layout (location = 0) in vec2 aPos;\n"
                                        "layout (location = 1) in vec2 aTexCoord;\n"
                                        "out vec2 texCoord;\n"
                                        "void main() {\n"
                                        "   gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);\n"
                                        "   texCoord=aTexCoord;\n"
                                        "}";

static const char *fragment_shader_text = "#version 330 core\n"
                                          "in vec2 texCoord;\n"
                                          "out vec4 fragColor;\n"
                                          "uniform sampler2D tex;\n"
                                          "void main() {\n"
                                          "   fragColor = texture(tex, texCoord);\n"
                                          "}";

static void error_callback(int error, const char *description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    /* This variable determines whether the screen black bands (that exist because the window's
     * dimensions ratio may be different from the framebuffer's ratio) are horizontal or vertical.
     */
    float scale = std::min((float)width / EmulatorFramebufferWidth,
                           (float)height / EmulatorFramebufferHeight);

    int viewportWidth = int(EmulatorFramebufferWidth * scale);
    int viewportHeight = int(EmulatorFramebufferHeight * scale);

    int viewportX = (width - viewportWidth) / 2;
    int viewportY = (height - viewportHeight) / 2;

    glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
}

int main(void) {
    GLFWwindow *window;
    GLuint vertex_array, vertex_buffer, element_array, texture, vertex_shader, fragment_shader,
        program;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(WindowMinWidth, WindowMinHeight, "Simple example", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL(glfwGetProcAddress);
    glfwSwapInterval(1);

    int w, h;
    glfwGetFramebufferSize(window, &w, &h);
    framebuffer_size_callback(window, w, h);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glClearColor(1, 0, 0, 1);

    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(framebufferQuad), framebufferQuad, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (const void *)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (const void *)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &element_array);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_array);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, EmulatorFramebufferWidth, EmulatorFramebufferHeight, 0,
                 GL_RGBA, GL_UNSIGNED_BYTE, framebuffer.data());

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glUseProgram(program);

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();
    exit(EXIT_SUCCESS);
}