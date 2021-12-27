#include"Window.h"

#define GLFW_INCLUDE_NONE
#include<GLFW/glfw3.h>
#include<glad/glad.h>

#include<stdexcept>

Window::Window(int width, int height, const char *title)
{
    if (!glfwInit())
        throw std::runtime_error("Error initializing GLFW");
    
    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    glfwSetWindowUserPointer(m_window, this);
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
        throw std::runtime_error("Error loading OpenGL extentions");
}

Window::~Window()
{
    glfwTerminate();
}

void Window::Show()
{
    glfwShowWindow(m_window);
}

bool Window::ShouldClose() const { return glfwWindowShouldClose(m_window); }

void Window::SwapBuffers()
{
    glfwPollEvents();
    glfwSwapBuffers(m_window);
}

void Window::SetKeyCallback(const std::function<void(int, int)> &callback)
{
    m_key_callback = callback;
    glfwSetKeyCallback(m_window, [](GLFWwindow *window, int key, int scancode, int action, int mods)
    {
        Window *w = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        w->m_key_callback(key, action);
    });
}