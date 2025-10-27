#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <string>

// Texture类：负责加载和管理OpenGL纹理
class Texture
{
public:
    // 构造函数：加载纹理
    // path: 纹理文件路径
    // generateMipmap: 是否生成Mipmap（默认true）
    Texture(const std::string& path, bool generateMipmap = true);

    // 析构函数：释放纹理
    ~Texture();

    // 绑定纹理到指定纹理单元
    // unit: 纹理单元（0-31），默认为0
    void Bind(unsigned int unit = 0) const;

    // 解绑纹理
    void Unbind() const;

    // 获取纹理ID
    GLuint GetID() const { return m_TextureID; }

    // 获取纹理尺寸
    int GetWidth() const { return m_Width; }
    int GetHeight() const { return m_Height; }
    int GetChannels() const { return m_Channels; }

    // 检查纹理是否加载成功
    bool IsLoaded() const { return m_TextureID != 0; }

private:
    GLuint m_TextureID;      // OpenGL纹理ID
    int m_Width;             // 纹理宽度
    int m_Height;            // 纹理高度
    int m_Channels;          // 颜色通道数（3=RGB, 4=RGBA）
    std::string m_FilePath;  // 文件路径（用于调试）

    // 辅助函数：从文件加载纹理
    bool LoadFromFile(const std::string& path, bool generateMipmap);
};

#endif // TEXTURE_H
