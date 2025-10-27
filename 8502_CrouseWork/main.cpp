#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Window.h"
#include "Shader.h"
#include "Camera.h"
#include "Texture.h"
// 窗口尺寸
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

// 相机对象（全局变量，供回调函数使用）
Camera camera(glm::vec3(0.0f, 2.0f, 5.0f));

// 时间变量
float deltaTime = 0.0f;  // 当前帧与上一帧的时间差
float lastFrame = 0.0f;  // 上一帧的时间

// 鼠标变量
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

// 鼠标回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;  // 反转：y坐标从下到上
    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// 鼠标滚轮回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}

// 处理键盘输入
void processInput(GLFWwindow* window)
{
    // ESC键退出
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // WASD移动相机
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    // 空格和左Shift上下移动
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.ProcessKeyboard(UP, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
        camera.ProcessKeyboard(DOWN, deltaTime);
}

// 主函数
int main()
{
    // ===== 步骤1：创建窗口 =====
    std::cout << "=== CSC8502 课程作业：实时图形渲染 ===" << std::endl;
    std::cout << "主题：冬季森林 <-> 夏季森林场景转换" << std::endl;
    std::cout << "正在初始化..." << std::endl;

    Window window(WINDOW_WIDTH, WINDOW_HEIGHT, "CSC8502 - 冬季/夏季森林场景");

    if (!window.Initialize())
    {
        std::cerr << "错误：窗口初始化失败！" << std::endl;
        return -1;
    }

    std::cout << "窗口创建成功！" << std::endl;

    // 设置鼠标回调
    glfwSetCursorPosCallback(window.GetGLFWWindow(), mouse_callback);
    glfwSetScrollCallback(window.GetGLFWWindow(), scroll_callback);

    // 捕获鼠标（隐藏并锁定鼠标）
    glfwSetInputMode(window.GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // ===== 步骤2：加载着色器 =====
    Shader basicShader("Shaders/basicVertex.glsl", "Shaders/basicFragment.glsl");

    if (basicShader.GetProgram() == 0)
    {
        std::cerr << "错误：着色器加载失败！" << std::endl;
        return -1;
    }
    // 加载纹理着色器
    Shader textureShader("Shaders/textureVertex.glsl", "Shaders/textureFragment.glsl");
    // 加载测试纹理
    Texture testTexture("Textures/test.jpg");

    if (!testTexture.IsLoaded())
    {
        std::cerr << "警告：纹理加载失败，将使用纯色渲染" << std::endl;
    }

    // ===== 步骤3：创建一个立方体（替代三角形，可以看到3D效果） =====
    float verticesWithTexture[] = {
        // 位置              // 颜色            // 纹理坐标
        // 后面
        -0.5f, -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.5f, 0.0f, 0.0f,  0.0f, 0.0f,

        // 前面
        -0.5f, -0.5f,  0.5f,  0.0f, 0.5f, 0.0f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.5f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 0.0f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 0.0f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.5f, 0.0f,  0.0f, 0.0f,

        // 左面
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 0.5f,  1.0f, 0.0f,

        // 右面
         0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.0f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  0.5f, 0.5f, 0.0f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.0f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.5f, 0.5f, 0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.5f, 0.5f, 0.0f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  0.5f, 0.5f, 0.0f,  1.0f, 0.0f,

         // 底面
         -0.5f, -0.5f, -0.5f,  0.5f, 0.0f, 0.5f,  0.0f, 1.0f,
          0.5f, -0.5f, -0.5f,  0.5f, 0.0f, 0.5f,  1.0f, 1.0f,
          0.5f, -0.5f,  0.5f,  0.5f, 0.0f, 0.5f,  1.0f, 0.0f,
          0.5f, -0.5f,  0.5f,  0.5f, 0.0f, 0.5f,  1.0f, 0.0f,
         -0.5f, -0.5f,  0.5f,  0.5f, 0.0f, 0.5f,  0.0f, 0.0f,
         -0.5f, -0.5f, -0.5f,  0.5f, 0.0f, 0.5f,  0.0f, 1.0f,

         // 顶面
         -0.5f,  0.5f, -0.5f,  0.0f, 0.5f, 0.5f,  0.0f, 1.0f,
          0.5f,  0.5f, -0.5f,  0.0f, 0.5f, 0.5f,  1.0f, 1.0f,
          0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 0.5f,  1.0f, 0.0f,
          0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 0.5f,  1.0f, 0.0f,
         -0.5f,  0.5f,  0.5f,  0.0f, 0.5f, 0.5f,  0.0f, 0.0f,
         -0.5f,  0.5f, -0.5f,  0.0f, 0.5f, 0.5f,  0.0f, 1.0f
    };

    // 创建VAO和VBO
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // 绑定VAO
    glBindVertexArray(VAO);

    // 绑定并填充VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesWithTexture), verticesWithTexture, GL_STATIC_DRAW);

    // 配置顶点属性
    // 位置属性（location = 0）
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 颜色属性（location = 1）
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // 纹理坐标属性
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // 解绑VAO
    glBindVertexArray(0);

    std::cout << "立方体创建成功！" << std::endl;

    std::cout << "\n=== 开始渲染循环 ===" << std::endl;
    std::cout << "\n控制说明：" << std::endl;
    std::cout << "  WASD - 前后左右移动" << std::endl;
    std::cout << "  空格 - 向上移动" << std::endl;
    std::cout << "  左Shift - 向下移动" << std::endl;
    std::cout << "  鼠标 - 环顾四周" << std::endl;
    std::cout << "  滚轮 - 缩放视野" << std::endl;
    std::cout << "  ESC - 退出程序\n" << std::endl;

    // ===== 渲染循环 =====
    while (!window.ShouldClose())
    {
        // 计算deltaTime
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 处理输入
        processInput(window.GetGLFWWindow());
        window.PollEvents();

        // 清除缓冲
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // 使用着色器
        basicShader.Use();

        // 创建变换矩阵
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

        glm::mat4 view = camera.GetViewMatrix();

        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom),
            (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT,
            0.1f,
            100.0f
        );

        // 传递矩阵到着色器
        basicShader.SetMat4("model", model);
        basicShader.SetMat4("view", view);
        basicShader.SetMat4("projection", projection);

        // 使用纹理着色器
        textureShader.Use();

        // 绑定纹理
        testTexture.Bind(0);  // 绑定到纹理单元0

        // 设置uniform变量
        textureShader.SetInt("textureSampler", 0);      // 纹理单元0
        textureShader.SetBool("useTexture", true);      // 使用纹理

        // 传递矩阵
        textureShader.SetMat4("model", model);
        textureShader.SetMat4("view", view);
        textureShader.SetMat4("projection", projection);

        // 绘制立方体
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        // 交换缓冲
        window.SwapBuffers();
    }

    // 清理资源
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    std::cout << "\n程序正常退出" << std::endl;
    return 0;
}
