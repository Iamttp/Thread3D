#ifndef OPENGLGAME_GL2DUTIL_H
#define OPENGLGAME_GL2DUTIL_H

// GUI主控件高度，防止同层控件绘制不清
#define ZGUI 0.01
// GUI主控件比例
#define RATE 1

struct MyPos {
    float x, y, z;
};

/**
 * 参考博客： https://blog.csdn.net/sac761/article/details/52179585
 */
MyPos screen2world(int x, int y) {
    int viewport[4];
    double modelview[16];
    double projection[16];
    float winX, winY, winZ;
    double posX, posY, posZ;
    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);
    winX = (float) x;
    winY = (float) viewport[3] - (float) y;
    // 把已经绘制好的像素（它可能已经被保存到显卡的显存中）读取到内存
    // GL_DEPTH_COMPONENT 为深度读取
    glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);
    gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);
    return {static_cast<float>(posX), static_cast<float>(posY), static_cast<float>(posZ)};
}

#include <windows.h>

// ASCII字符总共只有0到127，一共128种字符
#define MAX_CHAR       128

void drawString(const char *str) {
    static int isFirstCall = 1;
    static GLuint lists;

    if (isFirstCall) { // 如果是第一次调用，执行初始化
        // 为每一个ASCII字符产生一个显示列表
        isFirstCall = 0;
        // 申请MAX_CHAR个连续的显示列表编号
        lists = glGenLists(MAX_CHAR);
        // 把每个字符的绘制命令都装到对应的显示列表中
        wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
    }
    // 调用每个字符对应的显示列表，绘制每个字符
    for (; *str != '\0'; ++str)
        glCallList(lists + *str);
}

#endif //OPENGLGAME_GL2DUTIL_H
