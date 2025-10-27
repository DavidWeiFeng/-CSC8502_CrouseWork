#version 330 core

// 片段着色器 - 给每个像素上色

// 输入：从顶点着色器传来的数据
in vec3 fragColor;  // 顶点颜色（已经过光栅化插值）

// 输出：最终像素颜色
out vec4 finalColor;

void main()
{
    // 简单地使用顶点颜色作为最终颜色
    // vec4的第4个分量是alpha（透明度），1.0 = 完全不透明
    finalColor = vec4(fragColor, 1.0);
}
