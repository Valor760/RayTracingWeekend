#ifndef PROGRAM_H
#define PROGRAM_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "Renderer.h"


namespace RTWeekend {
class Program {
public:
    GLFWwindow* window = nullptr;

public:
    Program(int _width, int _height) : m_width(_width), m_height(_height) {}
    ~Program();
    void Run();
    
private:
    Program();
    void CompileShaders();
    void Init();
    static void keyCallBack(GLFWwindow* window, int key, int scan_code, int action, int mode);

private:
    const int m_width;
    const int m_height;
    unsigned char* m_bufferData = nullptr;
    GLuint m_VBO;
    GLuint m_VAO;
    GLuint m_texture;
    GLuint m_program;
    static inline RenderState m_RenderState = RenderState::kRenderStop;
};
} // namespace RTWeekend
#endif