#include "Renderer.h"
#include <iostream>

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
    // 初始化成员变量
    currentShader = nullptr;
    lightPosition = Vector3(100.0f, 100.0f, 100.0f);
    lightColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

    // 设置OpenGL状态
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    // 检查初始化状态
    init = true;

    std::cout << "Renderer initialized successfully" << std::endl;
}

Renderer::~Renderer() {
    // 清理资源
    // 后续添加场景对象的清理
    std::cout << "Renderer destroyed" << std::endl;
}

void Renderer::UpdateScene(float msec) {
    // 更新场景逻辑
    // 后续添加：
    // - 相机更新
    // - 动画更新
    // - 物理更新等
}

void Renderer::RenderScene() {
    // 清除颜色和深度缓冲
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.2f, 0.3f, 0.4f, 1.0f);

    // 后续添加场景渲染：
    // RenderSkybox();
    // RenderTerrain();
    // RenderWater();

    // 交换缓冲（由 Window::UpdateWindow 调用）
    SwapBuffers();
}

void Renderer::RenderSkybox() {
    // 后续实现
}

void Renderer::RenderTerrain() {
    // 后续实现
}

void Renderer::RenderWater() {
    // 后续实现
}

void Renderer::SetShaderLight(Shader* s) {
    if (!s) return;

    // 设置光照 uniform
    glUniform3fv(glGetUniformLocation(s->GetProgram(), "lightPos"),
                 1, (float*)&lightPosition);
    glUniform4fv(glGetUniformLocation(s->GetProgram(), "lightColor"),
                 1, (float*)&lightColor);
}
