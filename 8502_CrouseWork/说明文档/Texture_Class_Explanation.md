# 🧠 Texture 类详解文档

本文件讲解 `Texture` 类的实现细节与使用方法，帮助理解如何在 OpenGL 项目中加载并管理 2D 纹理（使用 `stb_image`）。

---

## 🧱 一、Texture 类的职责

`Texture` 类封装了从磁盘加载图像、创建 OpenGL 纹理对象、设置纹理参数、上传数据到 GPU、以及清理资源的完整流程。主要功能包括：

- 从文件读取图像（使用 `stb_image`）
- 处理图像通道与像素对齐
- 创建并配置 OpenGL 纹理对象（Wrapping、Filtering）
- 上传像素数据并（可选）生成 Mipmap
- 绑定/解绑纹理以及释放资源

---

## ⚙️ 二、构造与析构

```cpp
Texture::Texture(const std::string& path, bool generateMipmap)
    : m_TextureID(0), m_Width(0), m_Height(0), m_Channels(0), m_FilePath(path)
{
    if (!LoadFromFile(path, generateMipmap))
    {
        std::cerr << "错误：纹理加载失败: " << path << std::endl;
    }
}

Texture::~Texture()
{
    if (m_TextureID != 0)
    {
        glDeleteTextures(1, &m_TextureID);
    }
}
```

- 构造函数接受纹理文件路径 `path` 与是否生成 Mipmap 的布尔参数 `generateMipmap`。  
- 构造期间调用 `LoadFromFile` 加载并初始化纹理；失败则打印错误。  
- 析构函数释放 OpenGL 纹理对象（`glDeleteTextures`），防止 GPU 资源泄漏。

---

## 🔗 三、绑定与解绑

```cpp
void Texture::Bind(unsigned int unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, m_TextureID);
}

void Texture::Unbind() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
```
- `Bind(unit)`：激活纹理单元 `GL_TEXTURE0 + unit` 并绑定当前纹理到 `GL_TEXTURE_2D`。在使用 shader 的 `sampler2D` 前必须先绑定到正确的纹理单元。  
- `Unbind()`：解绑当前绑定（绑定到 0）。

---

## 🖼️ 四、从文件加载纹理：`LoadFromFile`（核心）

```cpp
bool Texture::LoadFromFile(const std::string& path, bool generateMipmap)
```
主要步骤：

1. **设置 STB Y 轴翻转**  
   `stbi_set_flip_vertically_on_load(true);`  
   许多图像文件的像素坐标原点在左上角，而 OpenGL 的纹理坐标原点在左下角。加载时翻转可以让图像方向与 OpenGL 约定一致。

2. **调用 stbi_load 加载图像数据**  
   ```cpp
   unsigned char* data = stbi_load(path.c_str(), &m_Width, &m_Height, &m_Channels, 0);
   ```
   - 成功时返回像素数据指针 `data`，并写回宽度、长度和通道数。  
   - 失败时使用 `stbi_failure_reason()` 打印错误并返回 `false`。

3. **生成 OpenGL 纹理对象并绑定**  
   ```cpp
   glGenTextures(1, &m_TextureID);
   glBindTexture(GL_TEXTURE_2D, m_TextureID);
   ```

4. **设置纹理参数（Wrapping 与 Filtering）**  
   ```cpp
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, generateMipmap ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
   glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
   ```
   - Wrapping：超出 [0,1] 的纹理坐标将重复（`GL_REPEAT`）。可替换为 `GL_CLAMP_TO_EDGE` 等。  
   - Filtering：决定纹理缩放时如何采样。若生成 Mipmap，`MIN_FILTER` 使用三线性过滤 `GL_LINEAR_MIPMAP_LINEAR`，否则使用 `GL_LINEAR`。

5. **根据通道数选择内部格式与数据格式，并设置像素对齐**  

   STB 返回的 `m_Channels` 会是常见值：1（灰度）、3（RGB）、4（RGBA）。

   - 对于 RGB（3 通道），调用：
     ```cpp
     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
     internalFormat = GL_RGB;
     dataFormat = GL_RGB;
     ```
     这是因为 OpenGL 默认的行对齐为 4 字节，RGB 行宽可能不是 4 的倍数，需要将 `GL_UNPACK_ALIGNMENT` 设置为 1 来避免读取时的行对齐错误。

   - 对于 RGBA（4 通道），使用默认 4 字节对齐：
     ```cpp
     glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
     internalFormat = GL_RGBA;
     dataFormat = GL_RGBA;
     ```

   - 对于单通道（RED），设置为 RED：
     ```cpp
     glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
     internalFormat = GL_RED;
     dataFormat = GL_RED;
     ```

   若通道数不是 1/3/4，会打印错误并返回失败。

6. **上传像素数据到 GPU**  
   ```cpp
   glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, m_Width, m_Height, 0, dataFormat, GL_UNSIGNED_BYTE, data);
   ```
   - `level=0`：基础 Mipmap 层；`internalFormat` 指定 GPU 存储格式；`dataFormat` 指定像素提交格式。

7. **（可选）生成 Mipmap**  
   ```cpp
   if (generateMipmap) glGenerateMipmap(GL_TEXTURE_2D);
   ```

8. **释放 CPU 端像素数据并解绑纹理**  
   ```cpp
   stbi_image_free(data);
   glBindTexture(GL_TEXTURE_2D, 0);
   ```

---

## 🔍 五、注意事项与常见问题

- **像素对齐问题（关键）**：当使用 `GL_RGB`（每像素 3 字节）时，OpenGL 默认 `GL_UNPACK_ALIGNMENT=4` 会按 4 字节对齐每行，可能导致纹理上传时出现颜色错位或图像畸变。将对齐设置为 1 可以避免此类问题。  
- **图像翻转**：如果发现纹理上下颠倒，可调整 `stbi_set_flip_vertically_on_load` 的参数。  
- **Mipmap**：生成 Mipmap 可以提高缩小纹理时的显示质量，但会增加显存占用与生成时间。对于需要多级缩放的纹理推荐开启。  
- **纹理单元**：在 shader 中，`sampler2D` 需要与纹理单元绑定（`glActiveTexture` + `glBindTexture`），并在 shader 中设置整数 uniform 指向该纹理单元。  
- **通道数不支持**：若遇到 CMYK 或其他特殊格式，`stb_image` 可能无法正确解析或会返回不在 1/3/4 范围内的值；需在预处理或转换图像格式。

---

## ✅ 六、示例：如何在渲染中使用

```cpp
// 创建纹理对象（例如在初始化阶段）
Texture diffuse("assets/wood.png", true);

// 在渲染循环中使用
shader.Use();
diffuse.Bind(0); // 绑定到纹理单元 0
shader.SetInt("diffuseTexture", 0); // 将 sampler2D 绑定到单元 0
// 绘制调用...
diffuse.Unbind();
```

---

## 🔁 七、可能的扩展与改进建议

- 支持更多纹理参数（如边缘采样模式、各向异性过滤）。  
- 支持从内存加载或从多种图像库选择（SOIL、lodepng 等）。  
- 支持压缩纹理格式（如 `GL_COMPRESSED_RGBA_S3TC_DXT1_EXT`）以节省显存。  
- 提供加载失败时的占位纹理（1x1 像素）以避免渲染异常。

---

## 📌 八、总结

`Texture` 类完成了纹理从文件到 GPU 的完整生命周期管理。掌握关键点后（像素对齐、通道格式、Mipmap、绑定到纹理单元），你就能在 OpenGL 项目中稳定地使用各种图像资源。

---
