#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>

// 构造函数：加载和编译着色器
Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    // 1. 从文件读取着色器源码
    std::string vertexSource = LoadShaderSource(vertexPath);
    std::string fragmentSource = LoadShaderSource(fragmentPath);

    if (vertexSource.empty() || fragmentSource.empty())
    {
        std::cerr << "错误：着色器源码为空" << std::endl;
        m_Program = 0;
        return;
    }

    // 2. 编译顶点着色器
    GLuint vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
    if (vertexShader == 0)
    {
        m_Program = 0;
        return;
    }

    // 3. 编译片段着色器
    GLuint fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
    if (fragmentShader == 0)
    {
        glDeleteShader(vertexShader);
        m_Program = 0;
        return;
    }

    // 4. 链接着色器程序
    m_Program = LinkProgram(vertexShader, fragmentShader);

    // 5. 删除着色器对象（已经链接到程序中，不再需要）
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (m_Program != 0)
    {
        std::cout << "着色器程序创建成功！ID: " << m_Program << std::endl;
    }
}

// 析构函数
Shader::~Shader()
{
    if (m_Program != 0)
    {
        glDeleteProgram(m_Program);
    }
}

// 使用着色器程序
void Shader::Use() const
{
    if (m_Program != 0)
    {
        glUseProgram(m_Program);
    }
}

// 从文件加载着色器源码
std::string Shader::LoadShaderSource(const std::string& filepath)
{
    std::ifstream file(filepath);
    if (!file.is_open())
    {
        std::cerr << "错误：无法打开着色器文件: " << filepath << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();

    std::cout << "成功加载着色器: " << filepath << std::endl;
    return buffer.str();
}

// 编译着色器
GLuint Shader::CompileShader(const std::string& source, GLenum type)
{
    // 创建着色器对象
    GLuint shader = glCreateShader(type);

    // 将源码附加到着色器对象
    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);

    // 编译着色器
    glCompileShader(shader);

    // 检查编译错误
    std::string typeStr = (type == GL_VERTEX_SHADER) ? "顶点着色器" : "片段着色器";
    if (!CheckCompileErrors(shader, typeStr))
    {
        glDeleteShader(shader);
        return 0;
    }

    std::cout << typeStr << "编译成功！" << std::endl;
    return shader;
}

// 链接着色器程序
GLuint Shader::LinkProgram(GLuint vertexShader, GLuint fragmentShader)
{
    // 创建着色器程序对象
    GLuint program = glCreateProgram();

    // 附加着色器
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // 链接程序
    glLinkProgram(program);

    // 检查链接错误
    if (!CheckCompileErrors(program, "着色器程序"))
    {
        glDeleteProgram(program);
        return 0;
    }

    std::cout << "着色器程序链接成功！" << std::endl;
    return program;
}

// 检查编译/链接错误
bool Shader::CheckCompileErrors(GLuint shader, const std::string& type)
{
    GLint success;
    GLchar infoLog[1024];

    if (type != "着色器程序")
    {
        // 检查着色器编译错误
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "错误：" << type << "编译失败\n" << infoLog << std::endl;
            return false;
        }
    }
    else
    {
        // 检查程序链接错误
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
            std::cerr << "错误：" << type << "链接失败\n" << infoLog << std::endl;
            return false;
        }
    }

    return true;
}

// === Uniform工具函数 ===

void Shader::SetBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(m_Program, name.c_str()), (int)value);
}

void Shader::SetInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_Program, name.c_str()), value);
}

void Shader::SetFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_Program, name.c_str()), value);
}

void Shader::SetVec3(const std::string& name, const glm::vec3& value) const
{
    glUniform3fv(glGetUniformLocation(m_Program, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetVec4(const std::string& name, const glm::vec4& value) const
{
    glUniform4fv(glGetUniformLocation(m_Program, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::SetMat4(const std::string& name, const glm::mat4& value) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_Program, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}
