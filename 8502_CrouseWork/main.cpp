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
#include "Terrain.h"     // ← 地形类
#include "Skybox.h"      // ← 天空盒类
#include "WaterPlane.h"  // ← 水面类
#include <windows.h>

// ========================================
// 窗口尺寸
// ========================================
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

// ========================================
// 相机对象（全局变量，供回调函数使用）
// ========================================
// 修改初始位置：
// - 提高高度（y=50）以便俯视地形
// - 向前移动（z=50）以便看到整个地形
Camera camera(glm::vec3(20.0f, 0.0f, 40.0f));

// ========================================
// 时间变量
// ========================================
float deltaTime = 0.0f;  // 当前帧与上一帧的时间差
float lastFrame = 0.0f;  // 上一帧的时间

// ========================================
// 鼠标变量
// ========================================
float lastX = WINDOW_WIDTH / 2.0f;
float lastY = WINDOW_HEIGHT / 2.0f;
bool firstMouse = true;

// ========================================
// 鼠标回调函数
// ========================================
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

// ========================================
// 鼠标滚轮回调函数
// ========================================
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}

// ========================================
// 处理键盘输入
// ========================================
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

// ========================================
// 主函数
// ========================================
int main()
{
    // 设置控制台UTF-8编码（支持中文）
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // ========================================
    // 步骤1：创建窗口
    // ========================================
    std::cout << "========================================" << std::endl;
    std::cout << "CSC8502 课程作业：实时图形渲染" << std::endl;
    std::cout << "主题：冬季森林 <-> 夏季森林场景转换" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n正在初始化窗口系统..." << std::endl;

    Window window(WINDOW_WIDTH, WINDOW_HEIGHT, "CSC8502 - 地形渲染系统");

    if (!window.Initialize())
    {
        std::cerr << "错误：窗口初始化失败！" << std::endl;
        return -1;
    }

    std::cout << "✓ 窗口创建成功！" << std::endl;

    // ========================================
    // 步骤2：启用深度测试
    // ========================================
    // 深度测试：确保近的物体遮挡远的物体
    // 3D场景必须启用！
    std::cout << "\n正在配置OpenGL..." << std::endl;
    glEnable(GL_DEPTH_TEST);
    std::cout << "✓ 深度测试已启用" << std::endl;

    // ========================================
    // 禁用背面剔除（修复断层问题）
    // ========================================
    // 背面剔除会隐藏"背对相机"的三角形
    // 如果三角形绕序不一致，会出现断层
    // 暂时禁用，确保所有三角形都能渲染
    glDisable(GL_CULL_FACE);
    std::cout << "✓ 背面剔除已禁用（双面渲染）" << std::endl;

    // ========================================
    // 启用混合（支持半透明）
    // ========================================
    // 水面需要半透明效果，必须启用混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    std::cout << "✓ 混合已启用（支持半透明）" << std::endl;

    // ========================================
    // 步骤3：设置鼠标回调
    // ========================================
    glfwSetCursorPosCallback(window.GetGLFWWindow(), mouse_callback);
    glfwSetScrollCallback(window.GetGLFWWindow(), scroll_callback);

    // 捕获鼠标（隐藏并锁定鼠标）
    glfwSetInputMode(window.GetGLFWWindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    std::cout << "✓ 鼠标输入已配置" << std::endl;

    // ========================================
    // 步骤4：加载着色器
    // ========================================
    std::cout << "\n正在加载着色器..." << std::endl;

    // 地形着色器
    Shader terrainShader("Shaders/terrainVertex.glsl", "Shaders/terrainFragment.glsl");
    if (terrainShader.GetProgram() == 0)
    {
        std::cerr << "错误：地形着色器加载失败！" << std::endl;
        return -1;
    }
    std::cout << "✓ 地形着色器加载成功" << std::endl;

    // 天空盒着色器
    Shader skyboxShader("Shaders/skyboxVertex.glsl", "Shaders/skyboxFragment.glsl");
    if (skyboxShader.GetProgram() == 0)
    {
        std::cerr << "错误：天空盒着色器加载失败！" << std::endl;
        return -1;
    }
    std::cout << "✓ 天空盒着色器加载成功" << std::endl;

    // 水面着色器
    Shader waterShader("Shaders/waterVertex.glsl", "Shaders/waterFragment.glsl");
    if (waterShader.GetProgram() == 0)
    {
        std::cerr << "错误：水面着色器加载失败！" << std::endl;
        return -1;
    }
    std::cout << "✓ 水面着色器加载成功" << std::endl;

    // ========================================
    // 步骤5：加载地形纹理
    // ========================================
    std::cout << "\n正在加载纹理..." << std::endl;

    // 尝试加载草地纹理
    // 如果没有grass.jpg，会使用test.jpg作为替代
    Texture grassTexture("Textures/grass.jpg");
    if (!grassTexture.IsLoaded())
    {
        std::cout << "警告：grass.jpg未找到，尝试使用test.jpg..." << std::endl;
        grassTexture = Texture("Textures/test.jpg");

        if (!grassTexture.IsLoaded())
        {
            std::cerr << "错误：无法加载任何纹理！" << std::endl;
            std::cerr << "请确保Textures文件夹中有grass.jpg或test.jpg" << std::endl;
            return -1;
        }
    }
    std::cout << "✓ 地形纹理加载成功" << std::endl;

    // ========================================
    // 步骤6：创建地形对象
    // ========================================
    std::cout << "\n正在创建地形..." << std::endl;

    // 尝试加载高度图
    // 参数说明：
    //   "Textures/heightmap.png" - 高度图路径
    //   100.0f - 地形大小（世界坐标单位）
    //   20.0f  - 高度缩放（降低以配合水面，范围约-10到+10）
    Texture heightmapTest("Textures/heightmap.png");
    if (!heightmapTest.IsLoaded())
    {
        std::cerr << "\n========================================" << std::endl;
        std::cerr << "错误：找不到高度图！" << std::endl;
        std::cerr << "========================================" << std::endl;
        std::cerr << "\n请按照以下步骤准备高度图：" << std::endl;
        std::cerr << "\n1. 创建一张256x256的灰度图像" << std::endl;
        std::cerr << "   - 白色/浅灰 = 山峰（高处）" << std::endl;
        std::cerr << "   - 黑色/深灰 = 山谷（低处）" << std::endl;
        std::cerr << "\n2. 命名为 heightmap.png" << std::endl;
        std::cerr << "\n3. 放到 Textures 文件夹中" << std::endl;
        std::cerr << "\n详细说明请参考：" << std::endl;
        std::cerr << "   8502_CrouseWork/如何准备高度图.md" << std::endl;
        std::cerr << "\n========================================\n" << std::endl;
        return -1;
    }

    Terrain terrain("Textures/heightmap.png", 100.0f, 10.0f);
    // terrain对象创建时会自动打印详细的创建过程

    // ========================================
    // 步骤7：创建天空盒
      // ========================================
    std::cout << "\n正在加载天空盒..." << std::endl;

    // 天空盒纹理路径（6张图：右、左、上、下、前、后）
    // 注意：顺序必须是 +X, -X, +Y, -Y, +Z, -Z
    std::vector<std::string> skyboxFaces = {
        "Textures/skybox/right.jpg",   // +X (右)
        "Textures/skybox/left.jpg",    // -X (左)
        "Textures/skybox/top.jpg",     // +Y (上)
        "Textures/skybox/bottom.jpg",  // -Y (下)
        "Textures/skybox/front.jpg",   // +Z (前)
        "Textures/skybox/back.jpg"     // -Z (后)
    };

    // 创建天空盒对象
    // 如果纹理文件不存在，会在控制台显示错误信息
    Skybox skybox(skyboxFaces);
    std::cout << "✓ 天空盒创建完成" << std::endl;

    // ========================================
    // 步骤8：创建水面
    // ========================================
    std::cout << "\n正在创建水面..." << std::endl;

    // 水面参数：
    //   水面高度：0.0f - 与地形偏移对齐（地形高度范围 -10 到 +10）
    //   水面大小：100.0f - 覆盖整个地形（与地形大小相同）
    //   网格分辨率：100 - 足够细分以显示波浪效果
    float waterLevel = 0.2f;      // 水面高度（Y坐标）
    float waterSize = 500.0f;     // 水面大小
    int waterResolution = 100;    // 网格分辨率

    WaterPlane waterPlane(waterLevel, waterSize, waterResolution);
    std::cout << "✓ 水面创建完成" << std::endl;

    // ========================================
    // 步骤9：设置光照参数
    // ========================================
    // 光源位置：从侧面上方照射（制造更明显的阴影）
    // 之前：(50, 80, 50) - 在正上方，阴影不明显
    // 现在：从左前方照射，能看到明显的明暗对比
    glm::vec3 lightPos(20.0f, 60.0f, 80.0f);

    // 光源颜色：稍微增强亮度
    glm::vec3 lightColor(1.2f, 1.2f, 1.0f);  // 稍微偏暖色，更自然

    // ========================================
    // 渲染前的说明信息
    // ========================================
    std::cout << "\n========================================" << std::endl;
    std::cout << "初始化完成！开始渲染..." << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n控制说明：" << std::endl;
    std::cout << "  WASD      - 前后左右移动" << std::endl;
    std::cout << "  空格      - 向上移动" << std::endl;
    std::cout << "  左Shift   - 向下移动" << std::endl;
    std::cout << "  鼠标移动  - 环顾四周" << std::endl;
    std::cout << "  滚轮      - 缩放视野" << std::endl;
    std::cout << "  ESC       - 退出程序" << std::endl;
    std::cout << "\n提示：" << std::endl;
    std::cout << "  - 相机初始位置在地形上方" << std::endl;
    std::cout << "  - 向下看可以看到整个地形" << std::endl;
    std::cout << "  - 使用WASD飞行，探索地形" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // ========================================
    // 主渲染循环
    // ========================================
    while (!window.ShouldClose())
    {
        // ────────────────────────────────────
        // 1. 更新时间
        // ────────────────────────────────────
        float currentFrame = (float)glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // ────────────────────────────────────
        // 2. 处理输入
        // ────────────────────────────────────
        processInput(window.GetGLFWWindow());
        window.PollEvents();

        // ────────────────────────────────────
        // 3. 清除缓冲
        // ────────────────────────────────────
        // 清除颜色缓冲和深度缓冲
        // 天空颜色：淡蓝色（模拟天空）
        glClearColor(0.53f, 0.81f, 0.92f, 1.0f);  // 天蓝色
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ────────────────────────────────────
        // 4. 激活地形着色器
        // ────────────────────────────────────
        terrainShader.Use();

        // ────────────────────────────────────
        // 5. 设置变换矩阵
        // ────────────────────────────────────

        // 5.1 模型矩阵（地形不需要旋转或平移，使用单位矩阵）
        glm::mat4 model = glm::mat4(1.0f);

        // 5.2 视图矩阵（相机）
        glm::mat4 view = camera.GetViewMatrix();

        // 5.3 投影矩阵（透视投影）
        glm::mat4 projection = glm::perspective(
            glm::radians(camera.Zoom),                  // 视野角度（FOV）
            (float)WINDOW_WIDTH / (float)WINDOW_HEIGHT, // 宽高比
            0.1f,                                       // 近裁剪面
            500.0f                                      // 远裁剪面（增大以便看到远处地形）
        );

        // ────────────────────────────────────
        // 6. 传递矩阵到着色器
        // ────────────────────────────────────
        terrainShader.SetMat4("model", model);
        terrainShader.SetMat4("view", view);
        terrainShader.SetMat4("projection", projection);

        // ────────────────────────────────────
        // 7. 传递光照参数到着色器
        // ────────────────────────────────────
        terrainShader.SetVec3("lightPos", lightPos);
        terrainShader.SetVec3("lightColor", lightColor);
        terrainShader.SetVec3("viewPos", camera.Position);

        // ────────────────────────────────────
        // 8. 绑定纹理
        // ────────────────────────────────────
        grassTexture.Bind(0);  // 绑定到纹理单元0
        terrainShader.SetInt("terrainTexture", 0);  // 告诉着色器使用纹理单元0

        // ────────────────────────────────────
        // 9. 渲染地形
        // ────────────────────────────────────
        terrain.Render();

        // ────────────────────────────────────
        // 10. 渲染水面（反射水面）
        // ────────────────────────────────────
        // 激活水面着色器
        waterShader.Use();

        // 设置变换矩阵
        waterShader.SetMat4("model", model);
        waterShader.SetMat4("view", view);
        waterShader.SetMat4("projection", projection);

        // 设置水面参数
        waterShader.SetVec3("viewPos", camera.Position);
        waterShader.SetFloat("time", (float)glfwGetTime());
        waterShader.SetVec3("waterColor", glm::vec3(0.1f, 0.3f, 0.5f));  // 深海蓝

        // 绑定天空盒立方体贴图（用于反射）
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox.GetCubemapID());
        waterShader.SetInt("skybox", 0);

        // 渲染水面
        waterPlane.Render();

        // ────────────────────────────────────
        // 11. 渲染天空盒（最后渲染）
        // ────────────────────────────────────
        // 修改深度测试函数，允许深度值为1.0通过
        // 天空盒的深度值永远是1.0（最远），需要GL_LEQUAL
        glDepthFunc(GL_LEQUAL);
        skybox.Draw(skyboxShader, view, projection);
        glDepthFunc(GL_LESS);  // 恢复默认深度测试

        // ────────────────────────────────────
        // 12. 交换缓冲
        // ────────────────────────────────────
        window.SwapBuffers();
    }

    // ========================================
    // 程序退出
    // ========================================
    std::cout << "\n========================================" << std::endl;
    std::cout << "程序正常退出" << std::endl;
    std::cout << "========================================" << std::endl;

    // 注意：Terrain, Shader, Texture对象会在析构函数中自动清理资源
    return 0;
}
