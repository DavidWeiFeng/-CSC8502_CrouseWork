#include "Renderer.h"
#include <iostream>

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
    // 初始化场景对象指针为 nullptr
    camera = nullptr;
    terrain = nullptr;
    skybox = nullptr;
    water = nullptr;

    terrainShader = nullptr;
    skyboxShader = nullptr;
    waterShader = nullptr;

    // 初始化参数
    lightPosition = Vector3(100.0f, 100.0f, 100.0f);
    lightColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
    cameraSpeed = 20.0f;
    mouseSensitivity = 0.1f;

    // 设置OpenGL状态
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    std::cout << "\n========================================" << std::endl;
    std::cout << "初始化 Renderer..." << std::endl;
    std::cout << "========================================" << std::endl;

    // ========================================
    // 创建相机
    // ========================================
    std::cout << "\n[1/5] 创建相机..." << std::endl;
    camera = new Camera(Vector3(0.0f, 10.0f, 20.0f));  // 初始位置
    camera->MovementSpeed = cameraSpeed;
    camera->MouseSensitivity = mouseSensitivity;
    std::cout << "✓ 相机创建成功" << std::endl;

    // ========================================
    // 加载着色器
    // ========================================
    std::cout << "\n[2/5] 加载着色器..." << std::endl;

    terrainShader = new Shader("Shaders/terrainVertex.glsl", "Shaders/terrainFragment.glsl");
    if (!terrainShader->LoadSuccess()) {
        std::cerr << "✗ 地形着色器加载失败！" << std::endl;
        init = false;
        return;
    }
    std::cout << "✓ 地形着色器加载成功" << std::endl;

    skyboxShader = new Shader("Shaders/skyboxVertex.glsl", "Shaders/skyboxFragment.glsl");
    if (!skyboxShader->LoadSuccess()) {
        std::cerr << "✗ 天空盒着色器加载失败！" << std::endl;
        init = false;
        return;
    }
    std::cout << "✓ 天空盒着色器加载成功" << std::endl;

    waterShader = new Shader("Shaders/waterVertex.glsl", "Shaders/waterFragment.glsl");
    if (!waterShader->LoadSuccess()) {
        std::cerr << "✗ 水面着色器加载失败！" << std::endl;
        init = false;
        return;
    }
    std::cout << "✓ 水面着色器加载成功" << std::endl;

    // ========================================
    // 创建地形（暂时注释，因为可能没有高度图）
    // ========================================
    std::cout << "\n[3/5] 创建地形..." << std::endl;
    std::cout << "  (暂时跳过 - 需要高度图纹理)" << std::endl;
    // terrain = new Terrain("Textures/heightmap.png", 100.0f, 10.0f);

    // ========================================
    // 创建天空盒（暂时注释，因为可能没有天空盒纹理）
    // ========================================
    std::cout << "\n[4/5] 创建天空盒..." << std::endl;
    std::cout << "  (暂时跳过 - 需要天空盒纹理)" << std::endl;
    // std::vector<std::string> faces = {
    //     "Textures/skybox/right.jpg",
    //     "Textures/skybox/left.jpg",
    //     "Textures/skybox/top.jpg",
    //     "Textures/skybox/bottom.jpg",
    //     "Textures/skybox/front.jpg",
    //     "Textures/skybox/back.jpg"
    // };
    // skybox = new Skybox(faces);

    // ========================================
    // 创建水面（暂时注释，等其他对象就绪后再启用）
    // ========================================
    std::cout << "\n[5/5] 创建水面..." << std::endl;
    std::cout << "  (暂时跳过 - 等地形和天空盒就绪)" << std::endl;
    // water = new WaterPlane(0.0f, 100.0f, 100);

    // 初始化成功
    init = true;

    std::cout << "\n========================================" << std::endl;
    std::cout << "✓ Renderer 初始化完成" << std::endl;
    std::cout << "========================================\n" << std::endl;
}

Renderer::~Renderer() {
    // 清理场景对象
    if (camera) delete camera;
    if (terrain) delete terrain;
    if (skybox) delete skybox;
    if (water) delete water;

    // 清理着色器
    if (terrainShader) delete terrainShader;
    if (skyboxShader) delete skyboxShader;
    if (waterShader) delete waterShader;

    std::cout << "Renderer destroyed" << std::endl;
}

void Renderer::UpdateScene(float msec) {
    if (!camera) return;

    // 将毫秒转换为秒
    float deltaTime = msec / 1000.0f;

    // ========================================
    // 处理键盘输入 - 相机移动
    // ========================================
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_W)) {
        camera->ProcessKeyboard(FORWARD, deltaTime);
    }
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_S)) {
        camera->ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_A)) {
        camera->ProcessKeyboard(LEFT, deltaTime);
    }
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_D)) {
        camera->ProcessKeyboard(RIGHT, deltaTime);
    }
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_SPACE)) {
        camera->ProcessKeyboard(UP, deltaTime);
    }
    if (Window::GetKeyboard()->KeyDown(KEYBOARD_SHIFT)) {
        camera->ProcessKeyboard(DOWN, deltaTime);
    }

    // ========================================
    // 处理鼠标输入 - 相机旋转
    // ========================================
    // nclgl Mouse 提供了相对移动量
    if (Window::GetMouse()->ButtonDown(MOUSE_LEFT)) {
        // 获取鼠标相对移动量
        float xoffset = Window::GetMouse()->GetRelativePosition().x;
        float yoffset = -Window::GetMouse()->GetRelativePosition().y; // Y坐标反转

        camera->ProcessMouseMovement(xoffset, yoffset);
    }
}

void Renderer::RenderScene() {
    // 清除颜色和深度缓冲
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.5f, 0.7f, 0.9f, 1.0f);  // 天空蓝色背景

    if (!camera) {
        SwapBuffers();
        return;
    }

    // ========================================
    // 设置投影矩阵和视图矩阵
    // ========================================
    Matrix4 viewMatrix = camera->GetViewMatrix();
    Matrix4 projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
                                               (float)width / (float)height,
                                               camera->Zoom);

    // ========================================
    // 1. 渲染天空盒（最先渲染，深度测试设为 LEQUAL）
    // ========================================
    if (skybox && skyboxShader) {
        glDepthFunc(GL_LEQUAL);  // 允许天空盒在最远处
        RenderSkybox();
        glDepthFunc(GL_LESS);    // 恢复默认深度测试
    }

    // ========================================
    // 2. 渲染地形
    // ========================================
    if (terrain && terrainShader) {
        RenderTerrain();
    }

    // ========================================
    // 3. 渲染水面（最后渲染，使用混合）
    // ========================================
    if (water && waterShader) {
        RenderWater();
    }

    // 交换缓冲
    SwapBuffers();
}

void Renderer::RenderSkybox() {
    if (!skybox || !skyboxShader || !camera) return;

    // 获取视图和投影矩阵
    Matrix4 viewMatrix = camera->GetViewMatrix();
    Matrix4 projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
                                               (float)width / (float)height,
                                               camera->Zoom);

    // 渲染天空盒（Skybox::Draw 会处理 uniform 设置）
    skybox->Draw(*skyboxShader, viewMatrix, projMatrix);
}

void Renderer::RenderTerrain() {
    if (!terrain || !terrainShader || !camera) return;

    // 激活地形着色器
    glUseProgram(terrainShader->GetProgram());

    // 设置变换矩阵
    Matrix4 modelMatrix; // 单位矩阵（地形在原点）
    Matrix4 viewMatrix = camera->GetViewMatrix();
    Matrix4 projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
                                               (float)width / (float)height,
                                               camera->Zoom);

    glUniformMatrix4fv(glGetUniformLocation(terrainShader->GetProgram(), "modelMatrix"),
                       1, false, (float*)&modelMatrix);
    glUniformMatrix4fv(glGetUniformLocation(terrainShader->GetProgram(), "viewMatrix"),
                       1, false, (float*)&viewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(terrainShader->GetProgram(), "projMatrix"),
                       1, false, (float*)&projMatrix);

    // 设置光照
    SetShaderLight(terrainShader);

    // 设置相机位置（用于高光计算）
    glUniform3fv(glGetUniformLocation(terrainShader->GetProgram(), "cameraPos"),
                 1, (float*)&camera->Position);

    // 渲染地形
    terrain->Render();
}

void Renderer::RenderWater() {
    if (!water || !waterShader || !camera) return;

    // 激活水面着色器
    glUseProgram(waterShader->GetProgram());

    // 设置变换矩阵
    Matrix4 modelMatrix; // 单位矩阵
    Matrix4 viewMatrix = camera->GetViewMatrix();
    Matrix4 projMatrix = Matrix4::Perspective(1.0f, 10000.0f,
                                               (float)width / (float)height,
                                               camera->Zoom);

    glUniformMatrix4fv(glGetUniformLocation(waterShader->GetProgram(), "modelMatrix"),
                       1, false, (float*)&modelMatrix);
    glUniformMatrix4fv(glGetUniformLocation(waterShader->GetProgram(), "viewMatrix"),
                       1, false, (float*)&viewMatrix);
    glUniformMatrix4fv(glGetUniformLocation(waterShader->GetProgram(), "projMatrix"),
                       1, false, (float*)&projMatrix);

    // 设置时间（用于波浪动画）
    // 注意：需要一个时间累加器，暂时使用静态变量
    static float time = 0.0f;
    time += 0.016f; // 假设 60 FPS
    glUniform1f(glGetUniformLocation(waterShader->GetProgram(), "time"), time);

    // 设置相机位置
    glUniform3fv(glGetUniformLocation(waterShader->GetProgram(), "cameraPos"),
                 1, (float*)&camera->Position);

    // 如果有天空盒，绑定它用于反射
    if (skybox) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->GetCubemapID());
        glUniform1i(glGetUniformLocation(waterShader->GetProgram(), "skybox"), 0);
    }

    // 渲染水面
    water->Render();
}

void Renderer::SetShaderLight(Shader* s) {
    if (!s) return;

    // 设置光照 uniform
    glUniform3fv(glGetUniformLocation(s->GetProgram(), "lightPos"),
                 1, (float*)&lightPosition);
    glUniform4fv(glGetUniformLocation(s->GetProgram(), "lightColor"),
                 1, (float*)&lightColor);
}
