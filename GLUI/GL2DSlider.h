#ifndef OPENGLGAME_GL2DSLIDER_H
#define OPENGLGAME_GL2DSLIDER_H

#include <GL/glut.h>
#include "GL2DUtil.h"
#include <iostream>

/**
 * 使用规则 :
 *
 * 1. 初始化滑动条
 *      eg: Slider slider{-1, 1.6, 10, 0, 10000};
 * 2. 在渲染循环里面绘制滑动条
 *      eg: glPushMatrix();
            slider.draw();
            glPopMatrix();
 * 3. 在鼠标事件里面监听，是否触发滑动条事件
 *      eg: slider.listen(x, y);
 * 4. 在合适位置获取滑动条的值
 *      eg: slider.getVal()
 *
 */
class Slider {
    float r{}, g{}, b{};
    float x{}, y{}, z{};
    float hBar{}, wBar{};
    float hBlock{}, wBlock{};
    float xBlock{};

    int val{20};
    int min{0}, max{100};

    void init() {
        r = g = b = 0.8;
        x = y = 0 * RATE;
        z = ZGUI * RATE;
        hBar = 0.1 * RATE;
        wBar = 1 * RATE;
        hBlock = 0.15 * RATE;
        wBlock = 0.1 * RATE;
    }

    void valToX() {
        float rate = float(val - min) / (max - min);
        xBlock = wBar * rate;
    }

    void XToVal() {
        float rate = xBlock / wBar;
        val = min + (max - min) * rate;
    }

public:
    Slider() {
        init();
    }

    Slider(float x, float y) {
        init();
        this->x = x;
        this->y = y;
        valToX();
    }

    Slider(float x, float y, int val, int min, int max) {
        init();
        this->x = x;
        this->y = y;
        this->val = val;
        this->min = min;
        this->max = max;
        valToX();
    }

    void draw() {
        // 块绘制
        glPushMatrix();
        glTranslatef(-wBar / 2 + xBlock + x, y, z + 0.001);
        glColor3f(r - 0.2, g - 0.2, b - 0.2);
        glRectf(-wBlock / 2, -hBlock / 2, wBlock / 2, hBlock / 2);
        glPopMatrix();

        // 滑动条绘制
        glPushMatrix();
        glTranslatef(x, y, z);
        glColor3f(r, g, b);
        glRectf(-wBar / 2, -hBar / 2, wBar / 2, hBar / 2);
        glPopMatrix();
    }

    void listen(int xx, int yy) {
        MyPos myPos = screen2world(xx, yy);
        // 碰撞检测
        if (myPos.x >= x - wBar / 2 && myPos.x <= x + wBar / 2 &&
            myPos.y >= y - hBar / 2 && myPos.y <= y + hBar / 2) {
            xBlock = myPos.x - x + wBar / 2;
            XToVal();
        }
    }

    int getVal() const {
        return val;
    }
};

#endif //OPENGLGAME_GL2DSLIDER_H
