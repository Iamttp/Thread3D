#ifndef OPENGLGAME_OPENGL2DSLIDER_H
#define OPENGLGAME_OPENGL2DSLIDER_H

#include <GL/glut.h>

class Slider {
    float r, g, b;
    float x, y;
    float h, w;

public:
    Slider() {
        r = g = b = 0.8;
        x = y = 0;
        h = w = 0.5;
    }

    int draw() {
        // 显示列表
        int index = glGenLists(1);//glGenLists()唯一的标识一个显示列表
        glNewList(index, GL_COMPILE);//用于对显示列表进行定界。第一个参数是一个整形索引值，由glGenLists()指定

        // 滑动条绘制
        glPushMatrix();
        glTranslatef(x, y, 0);

        glColor3f(r, g, b);
        glRectf(-h / 2, -w / 2, h / 2, w / 2);

        glPopMatrix();

        glEndList();
        return index;
    }
};


#endif //OPENGLGAME_OPENGL2DSLIDER_H
