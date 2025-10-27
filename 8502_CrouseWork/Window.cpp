#include "Window.h"
#include <iostream>

// 构造函数
Window::Window(int width, int height, const std::string& title)
    : m_Width(width), m_Height(height), m_Title(title), m_Window(nullptr)
{
}

// 析构函数
Window::~Window()
{
    // 清理GLFW资源
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
    }
    glfwTerminate();
}

// 初始化窗口和OpenGL上下文
bool Window::Initialize()
{
    // 步骤1：初始化GLFW库
    if (!glfwInit())
    {
        std::cerr << "错误：无法初始化GLFW" << std::endl;
        return false;
    }

    // 步骤2：配置GLFW
    // 告诉GLFW我们要使用OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // 使用核心模式（Core Profile）- 只使用现代OpenGL功能
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // 窗口大小不可改变（可选）
    // glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // 步骤3：创建窗口
    m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), nullptr, nullptr);
    if (!m_Window)
    {
        std::cerr << "错误：无法创建GLFW窗口" << std::endl;
        glfwTerminate();
        return false;
    }

    // 步骤4：将窗口的上下文设置为当前线程的主上下文
    glfwMakeContextCurrent(m_Window);

    // 步骤5：初始化GLAD（必须在创建OpenGL上下文之后）
    // GLAD用于加载所有OpenGL函数指针
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "错误：无法初始化GLAD" << std::endl;
        return false;
    }

    // 步骤6：设置视口（告诉OpenGL渲染窗口的尺寸）
    glViewport(0, 0, m_Width, m_Height);

    // 步骤7：注册窗口大小改变的回调函数
    glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);

    // 步骤8：启用深度测试（3D渲染必需）
    glEnable(GL_DEPTH_TEST);

    // 打印OpenGL版本信息
    std::cout << "OpenGL 版本: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL 版本: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "显卡: " << glGetString(GL_RENDERER) << std::endl;

    return true;
}

// 检查窗口是否应该关闭
bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}

// 交换前后缓冲
void Window::SwapBuffers()
{
    glfwSwapBuffers(m_Window);
}

// 处理事件
void Window::PollEvents()
{
    glfwPollEvents();
}

// 窗口大小改变回调函数
void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    // 当窗口大小改变时，更新视口
    glViewport(0, 0, width, height);
    std::cout << "窗口大小改变: " << width << "x" << height << std::endl;
}
