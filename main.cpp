#include <GL/glut.h>
#include <iostream>
#include <thread>
#include <vector>
#include "mul.h"

static float angle = 0.0, ratio;  // angle绕y轴的旋转角，ratio窗口高宽比
static float x = 0.0f, y = 0.0f, z = 5.0f;  //相机位置
static float lx = 0.0f, ly = 0.0f, lz = -1.0f;  //视线方向，初始设为沿着Z轴负方向

const int WIDTH = 1000;
const int HEIGHT = 1000;

bool mouseDown = false;
float xrot = 0.0f, yrot = 0.0f;
float xdiff = 0.0f, ydiff = 0.0f;

const float zoom = 0.1f;
const int width = 200;
const int height = 200;

int index; // 显示列表

// 为了显示好看，尽量奇数
ItemRepository gItemRepository1(9), gItemRepository2(5), gItemRepository3(5);

/**
 * 定义观察方式
 */
void changeSize(int w, int h) {
    //除以0的情况
    if (h == 0) h = 1;
    ratio = 1.0f * w / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //设置视口为整个窗口大小
    glViewport(0, 0, w, h);
    //设置可视空间
    gluPerspective(45, ratio, 1, 1000);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(x, y, z, x + lx, y + ly, z + lz, 0.0f, 1.0f, 0.0f);
}

/**
 * 视野漫游函数
 */
void orientMe(float directionx, float directiony) {
    x += directionx * 0.1;
    y += directiony * 0.1;
    glLoadIdentity();
    gluLookAt(x, y, z, x + lx, y + ly, z + lz, 0.0f, 1.0f, 0.0f);
}

/**
 * 视野漫游函数
 */
void moveMeFlat(int direction) {
    z += direction * (lz) * 0.1;
    glLoadIdentity();
    gluLookAt(x, y, z, x + lx, y + ly, z + lz, 0.0f, 1.0f, 0.0f);
}

/**
 * 鼠标事件
*/
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        mouseDown = true;
        xdiff = x - yrot;
        ydiff = -y + xrot;
    } else
        mouseDown = false;
}

/**
 * 鼠标移动事件
 */
void mouseMotion(int x, int y) {
    if (mouseDown) {
        yrot = x - xdiff;
        xrot = y + ydiff;
        glutPostRedisplay();
    }
}

/**
 * 加入按键控制
 */
void processSpecialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP:
            orientMe(0, 1);
            break;
        case GLUT_KEY_DOWN:
            orientMe(0, -1);
            break;
        case GLUT_KEY_LEFT:
            orientMe(-1, 0);
            break;
        case GLUT_KEY_RIGHT:
            orientMe(1, 0);
            break;
        case GLUT_KEY_PAGE_DOWN:
            moveMeFlat(-1);
            break;
        case GLUT_KEY_PAGE_UP:
            moveMeFlat(1);
            break;
        default:
            break;
    }
}

void processNormalKeys(unsigned char key, int x, int y) {
    switch (key) {
        // TODO add
        default:
            break;
    }
}

struct col {
    float r, g, b;

    col() {
        r = rand() % 10 / 10.0;
        g = rand() % 10 / 10.0;
        b = rand() % 10 / 10.0;
    }
};

std::vector<col> vec(100); // 100 个随机颜色

void draw(ItemRepository *ir, int colIndex) {
    // 然后画球
    for (int i = 0; i < ir->counter; i++) {
        glColor3f(vec[i + colIndex].r, vec[i + colIndex].g, vec[i + colIndex].b);
        glPushMatrix();
        glTranslatef(0, (i - (int(ir->counter) / 2)) * zoom * 2, zoom);
        glutSolidSphere(zoom, 100, 100);
        glPopMatrix();
    }
    // 画框
    glPushMatrix();
    glColor3f(1, 0, 0);
    glTranslatef(0, 0, zoom);
    glScalef(1, ir->BUFFER_SIZE, 1);
    glutWireCube(2 * zoom);
    glPopMatrix();
}

void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(x, y, z, x + lx, y + ly, z + lz, 0.0f, 1.0f, 0.0f);

    // 实现鼠标旋转的核心
    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);

    // Buff1
    glPushMatrix();
    glTranslatef(-10 * zoom, 0, 0);
    draw(&gItemRepository1, 50);
    glPopMatrix();

    // Buff2
    glPushMatrix();
    glTranslatef(10 * zoom, 6 * zoom, 0);
    draw(&gItemRepository2, 30);
    glPopMatrix();

    // Buff3
    glPushMatrix();
    glTranslatef(10 * zoom, -6 * zoom, 0);
    draw(&gItemRepository3, 10);
    glPopMatrix();

    // 最先画坐标
    glPushMatrix();
    glCallList(index);
    glPopMatrix();
    glFlush();
    glutSwapBuffers();
}

// !!! 不可有opengl函数
//std::string exec(const char *str, int len) {
//    if (str[0] == 'p') {
//
//    }
//    return "";
//}
//
//void myScript() {
//    while (true) {
//        std::cout << ">>> ";
//        char str[100];
//        std::cin.getline(str, 100);
//        std::cout << exec(str, strlen(str)) << std::endl;
//    }
//}

/**
 * 计时增加角度
 */
void myIdle() {
    myDisplay();
}

std::vector<std::thread *> vt;

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.93f, 0.93f, 0.93f, 0.0f);

    // 显示列表
    index = glGenLists(1);//glGenLists()唯一的标识一个显示列表
    glNewList(index, GL_COMPILE);//用于对显示列表进行定界。第一个参数是一个整形索引值，由glGenLists()指定
    // 然后坐标系
    glLineWidth(5);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    glPushMatrix();
    glColor4f(1, 0, 0, 0);
    glBegin(GL_LINES);
    glVertex3f(-width * zoom, 0, 0);
    glVertex3f(width * zoom, 0, 0);
    glEnd();

    glPushMatrix();
    glColor4f(1, 0, 0, 0);
    glBegin(GL_LINES);
    glVertex3f(0, -height * zoom, 0);
    glVertex3f(0, height * zoom, 0);
    glEnd();

    // 再画线
    glPushMatrix();
    glTranslatef(-width * zoom, 0, 0);
    glColor4f(0, 1, 0, 0);
    for (int i = 0; i < 2 * width; i++) {
        glBegin(GL_LINES);
        glVertex3f(0, -height * zoom, 0);
        glVertex3f(0, height * zoom, 0);
        glEnd();
        glTranslatef(zoom, 0, 0);
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -height * zoom, 0);
    for (int i = 0; i < 2 * height; i++) {
        glBegin(GL_LINES);
        glVertex3f(-width * zoom, 0, 0);
        glVertex3f(width * zoom, 0, 0);
        glEnd();
        glTranslatef(0, zoom, 0);
    }
    glPopMatrix();

    // 先画平面
    glColor4f(1, 1, 1, 1);
    glRectf(-width * zoom, -height * zoom, width * zoom, height * zoom);
    glEndList();

//    std::thread t(myScript);
//    t.detach();

    // 任务
    vt.push_back(new std::thread(putTask, &gItemRepository1, 490));
//    vt.push_back(new std::thread(putTask, &gItemRepository1, 1000));
//    vt.push_back(new std::thread(putTask, &gItemRepository1, 1000));

    vt.push_back(new std::thread(moveTask, &gItemRepository1, &gItemRepository2, 1000));
    vt.push_back(new std::thread(moveTask, &gItemRepository1, &gItemRepository3, 1000));

    vt.push_back(new std::thread(getTask, &gItemRepository2, 2000));
//    vt.push_back(new std::thread(getTask, &gItemRepository2, 1000));
//    vt.push_back(new std::thread(getTask, &gItemRepository2, 1000));

    vt.push_back(new std::thread(getTask, &gItemRepository3, 2000));
//    vt.push_back(new std::thread(getTask, &gItemRepository3, 1000));
//    vt.push_back(new std::thread(getTask, &gItemRepository3, 1000));
    for (auto &item:vt) item->detach();
}

int main(int argc, char *argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Demo");  // 改了窗口标题

    glutDisplayFunc(myDisplay);
    glutIdleFunc(myIdle);  // 表示在CPU空闲的时间调用某一函数
    glutSpecialFunc(processSpecialKeys);  // 按键
    glutKeyboardFunc(processNormalKeys);
    glutReshapeFunc(changeSize);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);

    init();
    glutMainLoop();
    return 0;
}
