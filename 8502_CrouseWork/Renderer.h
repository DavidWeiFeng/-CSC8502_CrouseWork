#pragma once

#include "nclgl/OGLRenderer.h"
#include "Camera.h"
#include "Terrain.h"
#include "Skybox.h"
#include "WaterPlane.h"

/*
 * Renderer - 主渲染器类
 * 继承自 OGLRenderer，负责管理整个场景的渲染
 */
class Renderer : public OGLRenderer {
public:
    Renderer(Window& parent);
    virtual ~Renderer();

    // OGLRenderer 必须实现的虚函数
    virtual void RenderScene() override;
    virtual void UpdateScene(float msec) override;

protected:
    // 场景渲染子函数
    void RenderSkybox();
    void RenderTerrain();
    void RenderWater();

    // 辅助函数 - 设置着色器 uniform
    void SetShaderLight(Shader* s);

private:
    // 场景对象
    Camera* camera;
    Terrain* terrain;
    Skybox* skybox;
    WaterPlane* water;

    // 着色器
    Shader* terrainShader;
    Shader* skyboxShader;
    Shader* waterShader;

    // 光照参数
    Vector3 lightPosition;
    Vector4 lightColor;

    // 相机参数
    float cameraSpeed;
    float mouseSensitivity;
};
