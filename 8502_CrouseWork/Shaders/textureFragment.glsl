#version 330 core

// 片段着色器 - 支持纹理

// 输入：从顶点着色器传来
in vec3 fragColor;
in vec2 fragTexCoord;

// 输出：最终颜色
out vec4 finalColor;

// Uniform变量
uniform sampler2D textureSampler;  // 纹理采样器
uniform bool useTexture;           // 是否使用纹理

void main()
{
    if (useTexture)
    {
        // 使用纹理
        vec4 texColor = texture(textureSampler, fragTexCoord);

        // 可以选择混合纹理和顶点颜色
        // finalColor = texColor * vec4(fragColor, 1.0);

        // 或者只使用纹理颜色
        finalColor = texColor;
    }
    else
    {
        // 不使用纹理，只用顶点颜色
        finalColor = vec4(fragColor, 1.0);
    }
}
