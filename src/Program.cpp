#include "Program.h"

#include <iostream>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "utils/thread_pool.h"


namespace RTWeekend {
void Program::Run() {
    // Init program and texture
    Init();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture);

    Utils::ThreadPool thread_pool(7);
    Renderer renderer(m_width, m_height, m_bufferData);

    // Set projection matrix for fullscreen
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(m_width),
        static_cast<float>(m_height), 0.0f, -1.0f, 1.0f);
    // Scale texture to match fullscreen
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(m_width, m_height, 1.0f));

    // Run rendering
    while (!glfwWindowShouldClose(window)) {
        if (m_shouldRender && !m_shouldStop) {
            // Allocate buffer
            memset(m_bufferData, 0, m_width * m_height * 3);
            renderer.StartRender();
            m_shouldRender = false;
        }
        
        if (m_shouldStop) {
            renderer.StopRender();
            m_shouldStop = false;
        }

        glUniformMatrix4fv(glGetUniformLocation(m_program, "model"), 1, false, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(m_program, "projection"), 1, false, glm::value_ptr(projection));

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_bufferData);
        glUseProgram(m_program);

        glBindVertexArray(m_VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void Program::Init() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_CORE_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // Disable window resize

    window = glfwCreateWindow(m_width, m_height, "Ray Tracing in a Weekend", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    glfwSetKeyCallback(window, keyCallBack);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw std::runtime_error("Failed to initalize GLAD!\n");
    }

    glViewport(0, 0, m_width, m_height);

    // Compile shaders
    m_program = glCreateProgram();
    CompileShaders();

    // Allocate buffer
    // Width * height * 3 colors
    m_bufferData = new unsigned char[m_width * m_height * 3]{ 0 };

    // Create fullscreen texture
    glGenTextures(1, &m_texture);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_bufferData);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Positions for fullscreen texture
    float vertices[] = {
        // pos      // tex
        -1.0f, 1.0f, -1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f, -1.0f,

        -1.0f, 1.0f, -1.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, -1.0f, 1.0f, -1.0f
    };


    // Bind buffers
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindVertexArray(m_VAO);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Program::CompileShaders() {
    // Shaders
    const char* vertex_shader =
        "#version 330 core\n"
        "layout(location = 0) in vec4 vertex;\n"
        "uniform mat4 model;\n"
        "uniform mat4 projection;"
        "out vec2 texture_coords;\n"
        "void main() {\n"
            "texture_coords = vertex.zw;"
            "gl_Position = projection * model * vec4(vertex.xy, 0.0, 1.0);"
        "}\0";
    const char* fragment_shader =
        "#version 330 core\n"
        "in vec2 texture_coords;\n"
        "out vec4 color;\n"
        "uniform sampler2D image;"
        "void main() {"
            "color = vec4(1.0) * texture(image, texture_coords);"
        "}\0";

    GLuint sVertex, sFragment;

    sVertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(sVertex, 1, &vertex_shader, 0);
    glCompileShader(sVertex);

    int success = -1;
    char log[1024];
    // Check vertex shader compilation
    glGetShaderiv(m_program, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(m_program, 1024, NULL, log);
        printf("VERTEX SHADER compile error: %s\n", log);
    }

    sFragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(sFragment, 1, &fragment_shader, 0);
    glCompileShader(sFragment);

    // Check fragment shader compilation
    glGetShaderiv(m_program, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(m_program, 1024, NULL, log);
        printf("FRAGMENT SHADER compile error: %s\n", log);
    }

    glAttachShader(m_program, sVertex);
    glAttachShader(m_program, sFragment);
    glLinkProgram(m_program);

    // Check program linkage
    glGetProgramiv(m_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(m_program, 1024, NULL, log);
        printf("Program link error: %s\n", log);
    }

    // Delete shaders, because they are linked above
    glDeleteShader(sVertex);
    glDeleteShader(sFragment);
}

void Program::keyCallBack(GLFWwindow* window, int key, int scan_code, int action, int mode) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Start rendering
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        m_shouldRender = true;
    // Stop rendering
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        m_shouldStop = true;
}

Program::~Program() {
    glfwDestroyWindow(window);
    delete[] m_bufferData;
}
} // namespace RTWeekend