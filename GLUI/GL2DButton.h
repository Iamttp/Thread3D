#ifndef OPENGLGAME_GL2DBUTTON_H
#define OPENGLGAME_GL2DBUTTON_H

#include <GL/glut.h>
#include "GL2DUtil.h"
#include "GL2DBase.h"

/**
 * 使用规则 :
 *
 * 1. 初始化按钮
 *      eg: char str[] = "hello";
            Button button(0, 0, str);
 * 2. 在渲染循环里面绘制按钮
 *      eg: glPushMatrix();
            button.draw();
            glPopMatrix();
 * 3. 在鼠标事件里面监听，是否触发按钮事件
 *      eg: button.listen(x, y);
 *
 * 文字功能还有问题
 */
class Button : public Base {
    float h{}, w{};
    char *str{};

    void init() {
        r = b = 0;
        g = 1;
        x = y = 0 * RATE;
        z = ZGUI * RATE;
        h = w = 0.2 * RATE;
    }

public:
    Button() {
        init();
    }

    Button(float x, float y, char *str) {
        init();
        this->x = x;
        this->y = y;
        this->str = str;
    }

    void draw() override {
        // TODO
//        glPushMatrix();
//        glColor3f(1.0f, 0.0f, 0.0f);
//        glRasterPos2f(0.0f, 0.0f);
//        drawString(str);
//        glPopMatrix();

        // 块绘制
        glPushMatrix();
        glTranslatef(x, y, z);
        glColor3f(r, g, b);
        glRectf(-w / 2, -h / 2, w / 2, h / 2);
        glPopMatrix();
    }

    bool listen(int xx, int yy) override {
        MyPos myPos = screen2world(xx, yy);
        // 碰撞检测
        if (myPos.x >= x - w / 2 && myPos.x <= x + w / 2 &&
            myPos.y >= y - h / 2 && myPos.y <= y + h / 2) {
            if (g == 1) r = 1, g = 0;
            else if (r == 1) g = 1, r = 0;
            return true;
        }
        return false;
    }
};

#endif //OPENGLGAME_GL2DBUTTON_H
