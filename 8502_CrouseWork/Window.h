#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>      // GLAD必须在GLFW之前包含
#include <GLFW/glfw3.h>
#include <string>

// Window类：负责创建和管理OpenGL窗口
class Window
{
public:
    // 构造函数：创建窗口
    // width: 窗口宽度
    // height: 窗口高度
    // title: 窗口标题
    Window(int width, int height, const std::string& title);

    // 析构函数：清理资源
    ~Window();

    // 初始化窗口和OpenGL上下文
    // 返回值：成功返回true，失败返回false
    bool Initialize();

    // 检查窗口是否应该关闭（用户点击了关闭按钮）
    bool ShouldClose() const;

    // 交换前后缓冲（双缓冲机制）
    void SwapBuffers();

    // 处理窗口事件（键盘、鼠标等）
    void PollEvents();

    // 获取窗口宽度和高度
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }

    // 获取GLFW窗口指针（供其他模块使用）
    GLFWwindow* GetGLFWWindow() const { return m_Window; }

private:
    int m_Width;              // 窗口宽度
    int m_Height;             // 窗口高度
    std::string m_Title;      // 窗口标题
    GLFWwindow* m_Window;     // GLFW窗口指针

    // 静态回调函数：窗口大小改变时调用
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
};

#endif // WINDOW_H
