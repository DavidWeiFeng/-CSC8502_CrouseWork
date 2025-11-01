/*
 * CSC8502 Coursework - Graphics Demo
 * 使用 nclgl 框架实现的实时渲染场景
 *
 * 控制说明：
 *   WASD      - 前后左右移动
 *   空格      - 向上移动
 *   左Shift   - 向下移动
 *   鼠标移动  - 环顾四周
 *   滚轮      - 缩放视野
 *   ESC       - 退出程序
 */

#include <iostream>
#include "nclgl/Window.h"
#include "Renderer.h"

// 窗口尺寸
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;

/*
 * 主函数
 */
int main() {
    // 设置控制台输出为 UTF-8
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    std::cout << "========================================" << std::endl;
    std::cout << "CSC8502 Coursework - Graphics Demo" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // ========================================
    // 创建窗口
    // ========================================
    std::cout << "正在创建窗口..." << std::endl;
    Window w("CSC8502 Coursework", WINDOW_WIDTH, WINDOW_HEIGHT, false);

    if (!w.HasInitialised()) {
        std::cerr << "错误：窗口初始化失败！" << std::endl;
        return -1;
    }
    std::cout << "✓ 窗口创建成功 (" << WINDOW_WIDTH << "x" << WINDOW_HEIGHT << ")" << std::endl;

    // ========================================
    // 创建渲染器
    // ========================================
    std::cout << "\n正在初始化渲染器..." << std::endl;
    Renderer renderer(w);

    if (!renderer.HasInitialised()) {
        std::cerr << "错误：渲染器初始化失败！" << std::endl;
        return -1;
    }
    std::cout << "✓ 渲染器初始化成功" << std::endl;

    // 将渲染器设置到窗口
    w.SetRenderer(&renderer);

    // ========================================
    // 显示控制说明
    // ========================================
    std::cout << "\n========================================" << std::endl;
    std::cout << "初始化完成！开始渲染..." << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "\n控制说明：" << std::endl;
    std::cout << "  WASD      - 前后左右移动" << std::endl;
    std::cout << "  空格      - 向上移动" << std::endl;
    std::cout << "  左Shift   - 向下移动" << std::endl;
    std::cout << "  鼠标移动  - 环顾四周" << std::endl;
    std::cout << "  滚轮      - 缩放视野" << std::endl;
    std::cout << "  ESC       - 退出程序" << std::endl;
    std::cout << "========================================\n" << std::endl;

    // ========================================
    // 主循环
    // ========================================
    // nclgl Window 的 UpdateWindow() 会：
    // 1. 检查窗口是否应该关闭
    // 2. 处理 Windows 消息
    // 3. 更新输入设备（Keyboard/Mouse）
    // 4. 调用 renderer.UpdateScene()
    // 5. 调用 renderer.RenderScene()
    //
    // 返回 false 表示窗口应该关闭
    while (w.UpdateWindow()) {
        // 获取时间增量（毫秒）
        float msec = w.GetTimer()->GetTimeDeltaSeconds() * 1000.0f;

        // 更新场景
        renderer.UpdateScene(msec);

        // 渲染场景
        renderer.RenderScene();
    }

    // ========================================
    // 程序退出
    // ========================================
    std::cout << "\n========================================" << std::endl;
    std::cout << "程序正常退出" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}
