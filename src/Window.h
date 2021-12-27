#pragma once

#include<functional>

struct GLFWwindow;

class Window
{
public:
    Window(int width, int height, const char *title);
    ~Window();

    void Show();

    bool ShouldClose() const;
    void SwapBuffers();

    void SetKeyCallback(const std::function<void(int, int)> &callback);

private:
    GLFWwindow *m_window;
    std::function<void(int, int)> m_key_callback;
};