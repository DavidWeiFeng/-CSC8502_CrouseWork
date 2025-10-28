# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

This is a CSC8502 coursework project: a real-time graphics demo using C++ and OpenGL that renders a scene transitioning between two different time periods (winter forest ↔ summer forest). The project demonstrates core rendering techniques including terrain rendering, lighting, environment mapping, skeletal animation, and post-processing effects.

**Deadline:** November 14th (demonstrations in game lab)
**Target:** Windows platform with Visual Studio 2022

## Build Commands

### Building the Project
```bash
# Open solution in Visual Studio
start 8502_CrouseWork.sln

# Or build from command line (requires VS build tools in PATH)
msbuild 8502_CrouseWork.sln /p:Configuration=Debug /p:Platform=x64
msbuild 8502_CrouseWork.sln /p:Configuration=Release /p:Platform=x64
```

### Running the Project
```bash
# Debug build
./x64/Debug/8502_CrouseWork.exe

# Release build
./x64/Release/8502_CrouseWork.exe
```

**Note:** The executable must be run from the repository root directory to correctly locate shader files and textures.

## Project Structure

```
8502_CrouseWork/              # Main project directory
├── Dependencies/             # Third-party libraries (vendored)
│   ├── GLAD/                # OpenGL loader
│   ├── GLFW/                # Windowing and input
│   ├── GLM/                 # Mathematics library
│   └── STB/                 # Image loading (stb_image.h)
├── Shaders/                 # GLSL shader files
│   ├── basicVertex.glsl
│   ├── basicFragment.glsl
│   ├── textureVertex.glsl
│   └── textureFragment.glsl
├── Textures/                # Texture assets
├── main.cpp                 # Entry point and render loop
├── Window.h/cpp            # Window management wrapper
├── Shader.h/cpp            # Shader loading and compilation
├── Camera.h/cpp            # First-person camera controller
└── Texture.h/cpp           # Texture loading (STB-based)
```

## Architecture & Key Systems

### Core Rendering Pipeline
1. **Window System** (`Window` class): GLFW wrapper that manages window creation, OpenGL context, and viewport resizing
2. **Shader System** (`Shader` class): Loads GLSL shaders from disk, compiles them, and provides uniform setters for mat4, vec3, bool, int, float
3. **Camera System** (`Camera` class): First-person camera with WASD movement, mouse look, and scroll zoom
4. **Texture System** (`Texture` class): Uses STB_image to load JPG/PNG textures and create OpenGL texture objects

### Current Implementation Status
- ✅ Window management with GLFW
- ✅ Shader loading and compilation
- ✅ First-person camera with keyboard (WASD + Space/Shift for up/down) and mouse controls
- ✅ Basic texture loading and rendering
- ✅ 3D transformations (model, view, projection matrices)

### Planned Systems (from 下一步实现计划.md)
- Terrain generation from heightmaps
- Phong lighting model (ambient + diffuse + specular)
- Skybox with cubemap textures
- OBJ model loading for trees and rocks
- Scene graph for efficient rendering
- Automatic camera path system
- Environment mapping for reflections
- Post-processing framework (FBO-based)
- Time transition effects between winter/summer scenes
- Advanced features: shadow mapping, particle systems, or skeletal animation

## Dependencies & Library Configuration

### Include Paths (already configured in .vcxproj)
- `$(ProjectDir)Dependencies\GLFW\include`
- `$(ProjectDir)Dependencies\GLAD\include`
- `$(ProjectDir)Dependencies\GLM`
- `$(ProjectDir)Dependencies\STB`

### Library Paths
- `$(ProjectDir)Dependencies\GLFW\lib-vc2022`

### Linked Libraries
- `glfw3.lib` (static GLFW library)
- `opengl32.lib` (Windows OpenGL library)

### Compiler Flags
- `/utf-8` flag enabled for UTF-8 source code support (Chinese comments)

## Shader Conventions

### Shader File Organization
- Place all shaders in `8502_CrouseWork/Shaders/` directory
- Use `.glsl` extension for shader files
- Naming convention: `{purpose}{Type}.glsl` (e.g., `terrainVertex.glsl`, `lightingFragment.glsl`)

### Standard Vertex Shader Inputs
```glsl
layout(location = 0) in vec3 aPos;      // Position
layout(location = 1) in vec3 aColor;    // Vertex color (optional)
layout(location = 2) in vec2 aTexCoord; // Texture coordinates
layout(location = 3) in vec3 aNormal;   // Normal vector (for lighting)
```

### Standard Uniforms
```glsl
uniform mat4 model;         // Model transformation matrix
uniform mat4 view;          // View (camera) matrix
uniform mat4 projection;    // Projection matrix
```

### Shader Loading
Shaders are loaded using the `Shader` class constructor:
```cpp
Shader shader("Shaders/vertexShader.glsl", "Shaders/fragmentShader.glsl");
```

Paths are relative to the executable's working directory (repository root).

## Rendering Loop Pattern

The main rendering loop in `main.cpp` follows this structure:
```cpp
while (!window.ShouldClose()) {
    // 1. Calculate delta time
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    // 2. Process input
    processInput(window.GetGLFWWindow());
    window.PollEvents();

    // 3. Clear buffers
    glClearColor(r, g, b, a);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 4. Activate shader and set uniforms
    shader.Use();
    shader.SetMat4("model", model);
    shader.SetMat4("view", camera.GetViewMatrix());
    shader.SetMat4("projection", projection);

    // 5. Bind textures and render geometry
    texture.Bind(0);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);

    // 6. Swap buffers
    window.SwapBuffers();
}
```

## Camera Controls

### Default Controls
- **WASD**: Move forward/left/backward/right
- **Space**: Move up
- **Left Shift**: Move down
- **Mouse**: Look around (cursor is captured and hidden)
- **Mouse Scroll**: Zoom (adjust FOV)
- **ESC**: Exit application

### Camera Configuration
Default camera settings in `Camera.h`:
- Initial position: `(0, 2, 5)`
- Speed: `5.0f` units/second
- Mouse sensitivity: `0.1f`
- Default FOV: `45.0f` degrees
- Yaw: `-90.0f` (facing -Z)
- Pitch: `0.0f` (looking horizontally)

## Coursework Requirements

### Core Features (Must Implement)
1. Landscape rendered with at least one texture in fragment shader
2. Lighting using at least one light source (ambient + diffuse + specular components)
3. Environment mapping demonstration
4. Automatic camera movement through the scene
5. Scene graph for efficient rendering
6. Simple meshes and textures combined into scene elements

### Advanced Features (Choose Several)
- Post-processing effects (blur, color correction, HDR bloom, lens flare)
- Advanced lighting (spotlights, projective lighting, multiple lights)
- Shadow mapping (basic, multiple maps, cascaded, or omnidirectional)
- Split-screen multiple viewpoints
- Animated objects (skeletal animation, procedural animation, vertex displacement)
- Environmental effects (rain, atmosphere, lightning, fire)

### Deliverables
1. Source code and Visual Studio solution (zip without .vs, .git, x64 folders)
2. Document with:
   - At least 4 screenshots with descriptions
   - List of key/mouse controls
   - YouTube video link (3-5 minutes demonstrating features)

## Development Workflow

### Adding New C++ Classes
1. Create `.h` and `.cpp` files in `8502_CrouseWork/` directory
2. Add to project via Visual Studio Solution Explorer, or manually edit `.vcxproj` and `.vcxproj.filters`
3. Include necessary headers (GLAD before GLFW, GLM as needed)

### Adding New Shaders
1. Create `.glsl` files in `8502_CrouseWork/Shaders/`
2. Add to project filters (optional, for organization in VS)
3. Load in code using `Shader` class constructor

### Adding Textures
1. Place image files (JPG/PNG) in `8502_CrouseWork/Textures/`
2. Load using `Texture` class: `Texture tex("Textures/filename.jpg");`
3. Bind before rendering: `tex.Bind(textureUnit);`

## Chinese Comments

This codebase contains Chinese comments and console output. The project is configured with `/utf-8` compiler flag to handle UTF-8 encoding correctly. When adding new code, maintain consistency:
- Use Chinese for detailed implementation comments
- Use English for API documentation and class/function names
- Console output can be in Chinese for user-facing messages

## Common Pitfalls

1. **Shader Path Issues**: Shaders must be loaded with paths relative to the working directory. Always run the executable from the repository root.

2. **Depth Testing**: Remember to enable depth testing for 3D rendering:
   ```cpp
   glEnable(GL_DEPTH_TEST);
   ```

3. **Texture Binding**: Textures must be bound to the correct texture unit and the corresponding sampler uniform must be set:
   ```cpp
   texture.Bind(0);  // Bind to unit 0
   shader.SetInt("textureSampler", 0);  // Set sampler to use unit 0
   ```

4. **GLAD/GLFW Order**: Always include GLAD before GLFW:
   ```cpp
   #include <glad/glad.h>
   #include <GLFW/glfw3.h>
   ```

5. **Normal Calculations**: For lighting to work correctly, vertex normals must be properly calculated and normalized.

## Resource References

The project plan document (下一步实现计划.md) recommends these resources:
- LearnOpenGL (Chinese): learnopengl-cn.github.io
- Free textures: Poly Haven, ambientCG, OpenGameArt
- Free skybox textures: Search "free skybox textures"
- Free models: Sketchfab (CC License), Free3D.com
