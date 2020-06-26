#ifndef OPENGLGAME_GL2DUTIL_H
#define OPENGLGAME_GL2DUTIL_H

// GUI主控件高度
#define ZGUI 0.01
// GUI主控件比例
#define RATE 1

struct MyPos {
    float x, y, z;
};

// https://blog.csdn.net/sac761/article/details/52179585
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

#endif //OPENGLGAME_GL2DUTIL_H
