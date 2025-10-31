#include "Texture.h"
#include <iostream>

// 使用 SOIL 图像加载库
#include "SOIL/SOIL.h"

// 构造函数：从文件加载
Texture::Texture(const std::string& path, bool generateMipmap)
    : m_TextureID(0), m_Width(0), m_Height(0), m_Channels(0), m_FilePath(path)
{
    if (!LoadFromFile(path, generateMipmap))
    {
        std::cerr << "错误：纹理加载失败: " << path << std::endl;
    }
}

// 构造函数：从内存缓冲区加载
Texture::Texture(const unsigned char* data, int size, const std::string& name, bool generateMipmap)
    : m_TextureID(0), m_Width(0), m_Height(0), m_Channels(0), m_FilePath(name)
{
    if (!LoadFromMemory(data, size, name, generateMipmap))
    {
        std::cerr << "错误：从内存加载纹理失败: " << name << std::endl;
    }
}

// 析构函数
Texture::~Texture()
{
    if (m_TextureID != 0)
    {
        glDeleteTextures(1, &m_TextureID);
    }
}

// 绑定纹理
void Texture::Bind(unsigned int unit) const
{
    // 激活纹理单元
    glActiveTexture(GL_TEXTURE0 + unit);
    // 绑定纹理
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

// 解绑纹理
void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

// 从文件加载纹理
bool Texture::LoadFromFile(const std::string& path, bool generateMipmap)
{
    // 使用 SOIL 加载图像（保持原始格式）
    // SOIL_load_image 参数：文件名，宽度指针，高度指针，通道数指针，强制通道数（0表示自动）
    unsigned char* data = SOIL_load_image(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);

    if (!data)
    {
        std::cerr << "错误：SOIL无法加载图像: " << path << std::endl;
        return false;
    }

    std::cout << "成功加载纹理: " << path << std::endl;
    std::cout << "  尺寸: " << m_Width << "x" << m_Height << std::endl;
    std::cout << "  通道数: " << m_Channels << std::endl;

    // 生成OpenGL纹理
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    // 设置纹理参数
    // 纹理环绕方式（Wrapping）
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // 纹理过滤方式（Filtering）
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 根据通道数选择格式
    GLenum internalFormat;
    GLenum dataFormat;

    // 设置像素对齐（对于RGB非常重要！）
    if (m_Channels == 3)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);  // RGB需要1字节对齐
        internalFormat = GL_RGB;
        dataFormat = GL_RGB;
    }
    else if (m_Channels == 4)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);  // RGBA使用4字节对齐
        internalFormat = GL_RGBA;
        dataFormat = GL_RGBA;
    }
    else if (m_Channels == 1)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        internalFormat = GL_RED;
        dataFormat = GL_RED;
    }
    else
    {
        std::cerr << "错误：不支持的通道数: " << m_Channels << std::endl;
        SOIL_free_image_data(data);
        return false;
    }

    std::cout << "  使用格式: " << (m_Channels == 3 ? "RGB" : (m_Channels == 4 ? "RGBA" : "RED")) << std::endl;

    // 上传纹理数据到GPU
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

    // 生成Mipmap
    if (generateMipmap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "  Mipmap已生成" << std::endl;
    }

    // 释放图像数据
    SOIL_free_image_data(data);

    // 解绑纹理
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}

// 从内存缓冲区加载纹理
bool Texture::LoadFromMemory(const unsigned char* buffer, int bufferSize, const std::string& name, bool generateMipmap)
{
    // 使用 SOIL 从内存缓冲区加载图像
    unsigned char* data = SOIL_load_image_from_memory(buffer, bufferSize, &m_Width, &m_Height, &m_Channels, 0);

    if (!data)
    {
        std::cerr << "错误：SOIL无法从内存加载图像: " << name << std::endl;
        return false;
    }

    std::cout << "成功从内存加载纹理: " << name << std::endl;
    std::cout << "  尺寸: " << m_Width << "x" << m_Height << std::endl;
    std::cout << "  通道数: " << m_Channels << std::endl;

    // 生成OpenGL纹理
    glGenTextures(1, &m_TextureID);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);

    // 设置纹理参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 根据通道数选择格式
    GLenum internalFormat;
    GLenum dataFormat;

    if (m_Channels == 3)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        internalFormat = GL_RGB;
        dataFormat = GL_RGB;
    }
    else if (m_Channels == 4)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        internalFormat = GL_RGBA;
        dataFormat = GL_RGBA;
    }
    else if (m_Channels == 1)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        internalFormat = GL_RED;
        dataFormat = GL_RED;
    }
    else
    {
        std::cerr << "错误：不支持的通道数: " << m_Channels << std::endl;
        SOIL_free_image_data(data);
        return false;
    }

    std::cout << "  使用格式: " << (m_Channels == 3 ? "RGB" : (m_Channels == 4 ? "RGBA" : "RED")) << std::endl;

    // 上传纹理数据到GPU
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);

    // 生成Mipmap
    if (generateMipmap)
    {
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "  Mipmap已生成" << std::endl;
    }

    // 释放图像数据
    SOIL_free_image_data(data);

    // 解绑纹理
    glBindTexture(GL_TEXTURE_2D, 0);

    return true;
}
