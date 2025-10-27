#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <string>
#include <glm/glm.hpp>

// Shader类：负责加载、编译和管理着色器程序
class Shader
{
public:
    // 构造函数：加载并编译着色器
    // vertexPath: 顶点着色器文件路径
    // fragmentPath: 片段着色器文件路径
    Shader(const std::string& vertexPath, const std::string& fragmentPath);

    // 析构函数
    ~Shader();

    // 使用/激活着色器程序
    void Use() const;

    // 获取着色器程序ID
    GLuint GetProgram() const { return m_Program; }

    // Uniform工具函数（用于向着色器传递数据）
    void SetBool(const std::string& name, bool value) const;
    void SetInt(const std::string& name, int value) const;
    void SetFloat(const std::string& name, float value) const;
    void SetVec3(const std::string& name, const glm::vec3& value) const;
    void SetVec4(const std::string& name, const glm::vec4& value) const;
    void SetMat4(const std::string& name, const glm::mat4& value) const;

private:
    GLuint m_Program;  // 着色器程序ID

    // 辅助函数：从文件读取着色器源码
    std::string LoadShaderSource(const std::string& filepath);

    // 辅助函数：编译单个着色器
    GLuint CompileShader(const std::string& source, GLenum type);

    // 辅助函数：链接着色器程序
    GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader);

    // 辅助函数：检查着色器编译错误
    bool CheckCompileErrors(GLuint shader, const std::string& type);
};

#endif // SHADER_H
