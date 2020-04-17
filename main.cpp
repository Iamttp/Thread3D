#include <GL/glut.h>
#include <cmath>
#include <iostream>
#include <ctime>
#include <list>
#include <fstream>
#include <thread>
#include "myDrawUtil.h"

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

int useUtil = 0; // 草图1，拉升2，颜色6
MyPos color;
std::list<Per3dObject> glp; // 全局已完成的图形
Per3dObject now;    // 正在绘制的图形
int index;

const float pi = 3.1415926;
bool beginPlay = false;
MyPos carPos;
float carSpeed = 0.0f;
float carAngle = pi / 2;
const float spi = pi / 30;
bool carView = false;

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
        if (useUtil == 1) {
            MyPos world = screen2world(x, y);
//            std::cout << func(world[1]) << " " << func(world[2]) << " " << myRound(world[3]) << std::endl;
            now.sketch.emplace_back(myRound(world.x), myRound(world.y), world.z);
        } else if (useUtil == 6) {
            MyPos world = screen2world(x, y);
            world.x = myRound(world.x);
            world.y = myRound(world.y);
            for (auto &item:glp)
                if (item.collisionDetection(world))
                    item.setColor(color);
        } else {
            mouseDown = true;
            xdiff = x - yrot;
            ydiff = -y + xrot;
        }
    } else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
        if (useUtil == 2) {
            useUtil = 0;
            glp.push_back(now);
            now.sketch.clear();
            now.h = 0;
        }
    } else
        mouseDown = false;
}

/**
 * 鼠标移动事件
 */
void mouseMotion(int x, int y) {
    if (useUtil == 2) {
        MyPos world = screen2world(x, y);
        now.h = myRound(world.x) - myRound(now.sketch[0].x);
        glutPostRedisplay();
    }
    if (mouseDown) {
        if (useUtil == 2) return;
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
            if (beginPlay) carSpeed++;
            else orientMe(0, 1);
            break;
        case GLUT_KEY_DOWN:
            if (beginPlay) carSpeed--;
            else orientMe(0, -1);
            break;
        case GLUT_KEY_LEFT:
            if (beginPlay) carAngle += spi;
            else orientMe(-1, 0);
            break;
        case GLUT_KEY_RIGHT:
            if (beginPlay) carAngle -= spi;
            else orientMe(1, 0);
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
    int mod = glutGetModifiers();
    switch (key) {
        case 'z':
            if (mod == GLUT_ACTIVE_ALT) {
                // Alt + z
                if (!now.sketch.empty())
                    now.sketch.pop_back();
                else {
                    now = glp.back();
                    glp.pop_back();
                }
                break;
            }
            // 画笔
            useUtil = 1;
            xrot = 0.0f, yrot = 0.0f;
            break;
        case 'q':
            // 拉升
            if (useUtil != 1) break;
            useUtil = 2;
            xrot = -45.0f, yrot = 45.0f;
            break;
        case 'a':
            // 切除
            break;
        case 's':
            if (mod == GLUT_ACTIVE_ALT) {
                // Alt + s
                std::ofstream out("out.txt");
                if (out.is_open()) {
                    out << glp.size() << std::endl;
                    for (auto &item:glp)
                        out << item;
                    out.close();
                }
            }
            break;
        case 'l':
            if (mod == GLUT_ACTIVE_ALT) {
                // Alt + l
                std::ifstream in("out.txt");
                if (in.is_open()) {
                    int n;
                    in >> n;
                    for (int i = 0; i < n; i++) {
                        Per3dObject item;
                        in >> item;
                        glp.push_back(item);
                    }
                    in.close();
                }
            }
            break;
        case 'p':
            beginPlay = !beginPlay;
            break;
        case 'o':
            carView = !carView;
        default:
            useUtil = 0;
            break;
    }
}

void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(x, y, z, x + lx, y + ly, z + lz, 0.0f, 1.0f, 0.0f);

    // 实现鼠标旋转的核心
    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);

    if (carView) {
        glRotatef(-90 * (carAngle * 2 / pi - 1), 0, 0, 1);
        glTranslatef(-carPos.x, -carPos.y, -carPos.z - zoom);
    }

    if (beginPlay) {
        glPushMatrix();
        glTranslatef(carPos.x, carPos.y, carPos.z + zoom);
        glRotatef(90 * (carAngle * 2 / pi - 1), 0, 0, 1);
        glColor4f(1, 1, 0, 0);
        glScaled(1, 1, 0.5);
        glutSolidCube(zoom * 4);
        glPopMatrix();
    }

    // 最后画草图
    glPushMatrix();
    glColor4f(0, 0, 0, 0);
    drawSketch(now.sketch);
    glPopMatrix();

    if (useUtil == 2) {
        glPushMatrix();
        glColor4f(0.9, 0.9, 0.9, 0);
        drawPull(now.sketch, now.h);
        glPopMatrix();
        glPushMatrix();
        glColor4f(0, 0, 0, 0);
        drawPull2(now.sketch, now.h);
        glPopMatrix();
    }

    for (auto &item:glp) {
        glPushMatrix();
        glColor4f(item.color.x, item.color.y, item.color.z, 0);
        drawPull(item.sketch, item.h);
        glPopMatrix();
        glPushMatrix();
        glColor4f(0, 0, 0, 0);
        drawPull2(item.sketch, item.h);
        glPopMatrix();
    }

    glCallList(index);
    glFlush();
    glutSwapBuffers();
}

// !!! 不可有opengl函数
std::string exec(const char *str, int len) {
    if (str[0] == 'p') {
        beginPlay = !beginPlay;
        return "success play";
    } else if (str[0] == 'c') {
        // 颜色设置
        std::string temp;
        int step = 0;
        for (int i = 1; i < len; i++) {
            if (str[i] == ',') {
                if (step == 0) color.x = stof(temp);
                else if (step == 1) color.y = stof(temp);
                temp = "";
                step++;
                continue;
            }
            temp += str[i];
        }
        color.z = stof(temp);
        useUtil = 6;
        return "success set color";
    }
    return "";
}

void myScript() {
    while (true) {
        std::cout << ">>> ";
        char str[100];
        std::cin.getline(str, 100);
        std::cout << exec(str, strlen(str)) << std::endl;
    }
}

/**
 * 计时增加角度
 */
void myIdle() {
    if (beginPlay) {
        carPos.y += std::sin(carAngle) * carSpeed * 0.001;
        carPos.x += std::cos(carAngle) * carSpeed * 0.001;
    }
    myDisplay();
}

void processMenuEvents(int option) {
    useUtil = 6;
    //option ，就是传递过来的value的值。
    switch (option) {
        case 1:
            color.set(1, 0, 0);
            break;
        case 2:
            color.set(0, 0, 1);
            break;
        case 3:
            color.set(0, 1, 0);
            break;
        default:
            break;
    }
}

void init() {
    const GLubyte *name = glGetString(GL_VENDOR);
    const GLubyte *biaoshifu = glGetString(GL_RENDERER);
    const GLubyte *OpenGLVersion = glGetString(GL_VERSION);
    const GLubyte *gluVersion = gluGetString(GLU_VERSION);
    std::cout << name << std::endl;
    std::cout << biaoshifu << std::endl;
    std::cout << OpenGLVersion << std::endl;
    std::cout << gluVersion << std::endl;

    srand((unsigned) time(NULL));

    // 在OpenGL中，默认是没有开启深度检测的，后绘制的物体覆盖先绘制的物体。
    // GL_DEPTH_TEST 用来开启更新深度缓冲区的功能
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.93f, 0.93f, 0.93f, 0.0f);

    // 创建菜单 processMenuEvents处理菜单事件。
    glutCreateMenu(processMenuEvents);
    // 给菜单增加条目
    glutAddMenuEntry("Red", 1);
    glutAddMenuEntry("Blue", 2);
    glutAddMenuEntry("Green", 3);
    //把菜单和鼠标右键关联起来。
    glutAttachMenu(GLUT_MIDDLE_BUTTON);

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

    std::thread t(myScript);
    t.detach();
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
