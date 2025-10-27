# 🧠 Shader 类详解文档

本文件讲解 `Shader` 类的结构与作用，帮助理解 OpenGL 项目中着色器的加载、编译与使用流程。

---

## 🧱 一、Shader 类的作用

在 OpenGL 中，**着色器（Shader）** 是运行在 GPU 上的小程序，用于控制图形渲染流程。  
常见的两种着色器类型：

- **顶点着色器（Vertex Shader）**：负责顶点位置变换。
- **片段着色器（Fragment Shader）**：负责像素颜色计算。

`Shader` 类的功能是将这些 `.vert` 和 `.frag` 文件：

1. 从磁盘读取源码；
2. 交给 OpenGL 编译；
3. 链接成完整的 GPU 程序；
4. 在渲染时加载使用；
5. 提供接口设置 uniform 变量。

---

## ⚙️ 二、构造函数工作流程

```cpp
Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
```

整个构造过程分为五步：

1. **读取文件源码**
   ```cpp
   std::string vertexSource = LoadShaderSource(vertexPath);
   std::string fragmentSource = LoadShaderSource(fragmentPath);
   ```
   从文件中读取 GLSL 源码。

2. **编译顶点着色器**
   ```cpp
   GLuint vertexShader = CompileShader(vertexSource, GL_VERTEX_SHADER);
   ```

3. **编译片段着色器**
   ```cpp
   GLuint fragmentShader = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);
   ```

4. **链接为着色器程序**
   ```cpp
   m_Program = LinkProgram(vertexShader, fragmentShader);
   ```

5. **清理资源**
   ```cpp
   glDeleteShader(vertexShader);
   glDeleteShader(fragmentShader);
   ```

---

## 📂 三、从文件读取源码

```cpp
std::string Shader::LoadShaderSource(const std::string& filepath)
```

使用 `ifstream` 读取文件内容并存入 `stringstream`：
```cpp
std::ifstream file(filepath);
std::stringstream buffer;
buffer << file.rdbuf();
return buffer.str();
```

输出效果：
```
成功加载着色器: vertex.glsl
```

---

## 🔧 四、编译着色器

```cpp
GLuint Shader::CompileShader(const std::string& source, GLenum type)
```

核心流程：

1. 创建着色器对象  
   ```cpp
   GLuint shader = glCreateShader(type);
   ```
2. 绑定源码  
   ```cpp
   glShaderSource(shader, 1, &src, nullptr);
   ```
3. 编译着色器  
   ```cpp
   glCompileShader(shader);
   ```
4. 检查编译结果  
   ```cpp
   CheckCompileErrors(shader, "顶点着色器");
   ```

成功时打印：
```
顶点着色器编译成功！
```

---

## 🔗 五、链接着色器程序

```cpp
GLuint Shader::LinkProgram(GLuint vertexShader, GLuint fragmentShader)
```

将两个着色器附加并链接为一个完整的程序：

```cpp
GLuint program = glCreateProgram();
glAttachShader(program, vertexShader);
glAttachShader(program, fragmentShader);
glLinkProgram(program);
```

检查链接结果：
```cpp
CheckCompileErrors(program, "着色器程序");
```

输出：
```
着色器程序链接成功！
```

---

## 🧩 六、错误检查机制

```cpp
bool Shader::CheckCompileErrors(GLuint shader, const std::string& type)
```

- 如果是单个着色器：
  ```cpp
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
  glGetShaderInfoLog(shader, 1024, nullptr, infoLog);
  ```
- 如果是完整程序：
  ```cpp
  glGetProgramiv(shader, GL_LINK_STATUS, &success);
  glGetProgramInfoLog(shader, 1024, nullptr, infoLog);
  ```

错误信息会输出到控制台，例如：
```
错误：片段着色器编译失败
0(5): error: syntax error, unexpected identifier
```

---

## 🎮 七、激活 Shader 程序

```cpp
void Shader::Use() const
{
    glUseProgram(m_Program);
}
```

激活当前着色器，使后续渲染使用它。

---

## 🎚 八、Uniform 设置函数

在 GLSL 中常用 `uniform` 变量来从 CPU 向 GPU 传值，例如：

```glsl
uniform mat4 model;
uniform vec3 lightColor;
```

C++ 端对应接口如下：

| 函数名 | 类型 | 示例 |
|--------|------|------|
| `SetBool` | `bool` | `shader.SetBool("useLight", true);` |
| `SetInt` | `int` | `shader.SetInt("textureID", 0);` |
| `SetFloat` | `float` | `shader.SetFloat("intensity", 0.8f);` |
| `SetVec3` | `glm::vec3` | `shader.SetVec3("color", glm::vec3(1.0,0.5,0.2));` |
| `SetVec4` | `glm::vec4` | `shader.SetVec4("clipPlane", glm::vec4(0,1,0,0));` |
| `SetMat4` | `glm::mat4` | `shader.SetMat4("model", modelMatrix);` |

所有函数内部调用：
```cpp
glUniform*(glGetUniformLocation(m_Program, name.c_str()), ...);
```

---

## 🧠 九、函数与功能总览

| 功能 | 函数 | 说明 |
|------|------|------|
| 读取文件 | `LoadShaderSource()` | 读取 GLSL 源码 |
| 编译着色器 | `CompileShader()` | GLSL → GPU 可执行对象 |
| 链接程序 | `LinkProgram()` | 顶点 + 片段 → 完整程序 |
| 激活使用 | `Use()` | 启用当前着色器 |
| 错误检查 | `CheckCompileErrors()` | 输出详细错误信息 |
| 设置 Uniform | `Set*()` 系列 | CPU 向 GPU 传值 |

---

## 🎨 十、完整使用示例

**vertex.glsl**
```glsl
#version 330 core
layout(location = 0) in vec3 aPos;
uniform mat4 model;
void main() {
    gl_Position = model * vec4(aPos, 1.0);
}
```

**fragment.glsl**
```glsl
#version 330 core
out vec4 FragColor;
uniform vec3 color;
void main() {
    FragColor = vec4(color, 1.0);
}
```

**C++ 使用**
```cpp
Shader shader("vertex.glsl", "fragment.glsl");
shader.Use();
shader.SetVec3("color", glm::vec3(1.0, 0.5, 0.2));
shader.SetMat4("model", modelMatrix);
```

---

## 🗺️ 十一、Shader 流程图（逻辑）

```
[加载文件] → [编译顶点着色器]
                ↓
           [编译片段着色器]
                ↓
           [链接为程序对象]
                ↓
           [删除单个着色器]
                ↓
           [使用/设置 uniform]
```

---

## ✅ 十二、总结

`Shader` 类将繁琐的着色器编译与管理过程封装成简洁的接口，方便使用。  
使用该类可以让 OpenGL 代码更模块化、更易维护。

核心要点：
- GLSL 文件需要单独保存；
- 编译、链接时必须检查错误；
- 使用 `Use()` 激活后才能设置 uniform；
- 程序结束时记得释放资源。

---
