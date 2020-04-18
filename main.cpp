#include <GL/glut.h>
#include <thread>
#include <vector>
#include "mul.h"

static float myratio;  // angle绕y轴的旋转角，ratio窗口高宽比
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
std::vector<std::thread *> vt;
std::vector<float> vSpeed{500, 1210, 2500, 2120, 2920};

// 为了显示好看，尽量奇数
ItemRepository gItemRepository1(1, 9), gItemRepository2(2, 5), gItemRepository3(3, 5);

/**
 * 定义观察方式
 */
void changeSize(int w, int h) {
    //除以0的情况
    if (h == 0) h = 1;
    myratio = 1.0f * w / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //设置视口为整个窗口大小
    glViewport(0, 0, w, h);
    //设置可视空间
    gluPerspective(45, myratio, 1, 1000);

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
        case 'q':
            vSpeed[0] *= 0.9;
            break;
        case 'w':
            vSpeed[1] *= 0.9;
            break;
        case 'e':
            vSpeed[2] *= 0.9;
            break;
        case 'r':
            vSpeed[3] *= 0.9;
            break;
        case 't':
            vSpeed[4] *= 0.9;
            break;

        case 'a':
            vSpeed[0] *= 1.1;
            break;
        case 's':
            vSpeed[1] *= 1.1;
            break;
        case 'd':
            vSpeed[2] *= 1.1;
            break;
        case 'f':
            vSpeed[3] *= 1.1;
            break;
        case 'g':
            vSpeed[4] *= 1.1;
            break;
        default:
            break;
    }

    char str[80];
    sprintf(str, "Now Speed:\n\tPut:%.2f\n\tMove1:%.2f\n\tMove2:%.2f\n\tGet1:%.2f\n\tGet2:%.2f\n\n",
            1000.0f / vSpeed[0], 1000.0f / vSpeed[1], 1000.0f / vSpeed[2], 1000.0f / vSpeed[3], 1000.0f / vSpeed[4]);
//    TextOut(10, 10, str);
    system("cls");
    std::cout << str;
}

void drawSphere(ItemRepository *ir, object *ob, int i) {
    glColor3f(ob->r, ob->g, ob->b);
    glPushMatrix();
    glTranslatef(0, (i - int(ir->BUFFER_SIZE) / 2) * zoom * 2, zoom);
    glutSolidSphere(zoom, 100, 100);
    glPopMatrix();
}

void drawArrow() {
    glTranslatef(0, 0, zoom);
    glRotatef(90, 0.0f, 1.0f, 0.0f);
    glutSolidCone(zoom, zoom * 2, 100, 100);
}

//void display(std::shared_ptr<node> n) {
//    glTranslatef(0, (n->ob->rei - int(n->ir->BUFFER_SIZE) / 2) * zoom * 2, 0);
//    if (n->action == 1) { // p
//        glTranslatef(-zoom * 15, 0, 0);
//        glColor3f(0, 1, 1);
//    } else if (n->action == 2) { // m
//        if (n->ir2->index == 2) glTranslatef(zoom * 5, 6 * zoom, 0);
//        else glTranslatef(zoom * 5, -6 * zoom, 0);
//        glColor3f(0, 0, 1);
//    } else { // c
//        if (n->ir->index == 2) glTranslatef(zoom * 15, 6 * zoom, 0);
//        else glTranslatef(zoom * 15, -6 * zoom, 0);
//        glColor3f(1, 0, 0);
//    }
//    drawArrow();
//}

std::shared_ptr<node> n;
int coun;

void myDisplay() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(x, y, z, x + lx, y + ly, z + lz, 0.0f, 1.0f, 0.0f);

    // 实现鼠标旋转的核心
    glRotatef(xrot, 1.0f, 0.0f, 0.0f);
    glRotatef(yrot, 0.0f, 1.0f, 0.0f);

//    if (!mesQ.empty()) {
//        n = mesQ.wait_and_pop();
//        coun = 10;
//    }
//
//    // 消息队列处理
//    glPushMatrix();
//    if (coun) {
//        display(n);
//        coun--;
//    }
//    glPopMatrix();

    // 绘制球
    glPushMatrix();
    glTranslatef(-10 * zoom, 0, 0);
    for (int i = 0; i < gItemRepository1.BUFFER_SIZE; i++) {
        object *ob = gItemRepository1.buffer[i];
        if (ob) drawSphere(&gItemRepository1, gItemRepository1.buffer[i], ob->rei);
    }

    glColor3f(0, 0, 1);
    glPushMatrix();
    glTranslatef(-5 * zoom,
                 (int(gItemRepository1.in) - int(gItemRepository1.BUFFER_SIZE) / 2) * zoom * 2, 0);
    drawArrow();
    glColor3f(0, 0, 0.5);
    glPopMatrix();
    glTranslatef(5 * zoom,
                 (int(gItemRepository1.out) - int(gItemRepository1.BUFFER_SIZE) / 2) * zoom * 2, 0);
    drawArrow();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10 * zoom, 6 * zoom, 0);
    for (int i = 0; i < gItemRepository2.BUFFER_SIZE; i++) {
        object *ob = gItemRepository2.buffer[i];
        if (ob) drawSphere(&gItemRepository2, gItemRepository2.buffer[i], ob->rei);
    }

    glColor3f(0, 1, 1);
    glPushMatrix();
    glTranslatef(-5 * zoom,
                 (int(gItemRepository2.in) - int(gItemRepository2.BUFFER_SIZE) / 2) * zoom * 2, 0);
    drawArrow();
    glPopMatrix();
    glColor3f(0, 0.5, 0.5);
    glTranslatef(5 * zoom,
                 (int(gItemRepository2.out) - int(gItemRepository2.BUFFER_SIZE) / 2) * zoom * 2, 0);
    drawArrow();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(10 * zoom, -6 * zoom, 0);
    for (int i = 0; i < gItemRepository3.BUFFER_SIZE; i++) {
        object *ob = gItemRepository3.buffer[i];
        if (ob) drawSphere(&gItemRepository3, gItemRepository3.buffer[i], ob->rei);
    }

    glColor3f(1, 0, 1);
    glPushMatrix();
    glTranslatef(-5 * zoom,
                 (int(gItemRepository3.in) - int(gItemRepository3.BUFFER_SIZE) / 2) * zoom * 2, 0);
    drawArrow();
    glPopMatrix();
    glColor3f(0.5, 0, 0.5);
    glTranslatef(5 * zoom,
                 (int(gItemRepository3.out) - int(gItemRepository3.BUFFER_SIZE) / 2) * zoom * 2, 0);
    drawArrow();
    glPopMatrix();

    // 最先画坐标和框
    glPushMatrix();
    glCallList(index);
    glPopMatrix();
    glFlush();
    glutSwapBuffers();
}

// 有动作再调用
void myIdle(int i) {
    std::unique_lock<std::mutex> lock1(gItemRepository1.mtx);
    std::unique_lock<std::mutex> lock2(gItemRepository2.mtx);
    std::unique_lock<std::mutex> lock3(gItemRepository3.mtx);
    myDisplay();
    lock1.unlock(); // 解锁.
    lock2.unlock(); // 解锁.
    lock3.unlock(); // 解锁.
    glutTimerFunc(20, myIdle, 1);
}

void drawFrame(ItemRepository *ir) {
    // 画框
    glPushMatrix();
    glTranslatef(0, 0, zoom);
    glScalef(1, ir->BUFFER_SIZE, 1);
    glLineWidth(2);
    glutWireCube(2 * zoom);
    glPopMatrix();
}

void init() {
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.93f, 0.93f, 0.93f, 0.0f);

    // 显示列表
    index = glGenLists(1);//glGenLists()唯一的标识一个显示列表
    glNewList(index, GL_COMPILE);//用于对显示列表进行定界。第一个参数是一个整形索引值，由glGenLists()指定

    glColor3f(0, 1, 0);
    // Buff1
    glPushMatrix();
    glTranslatef(-10 * zoom, 0, 0);
    drawFrame(&gItemRepository1);
    glPopMatrix();

    // Buff2
    glPushMatrix();
    glTranslatef(10 * zoom, 6 * zoom, 0);
    drawFrame(&gItemRepository2);
    glPopMatrix();

    // Buff3
    glPushMatrix();
    glTranslatef(10 * zoom, -6 * zoom, 0);
    drawFrame(&gItemRepository3);
    glPopMatrix();

    // 再画线
    glPushMatrix();
    glTranslatef(-width * zoom, 0, 0);
    glColor4f(1, 0, 0, 0);
    for (int i = 0; i < 2 * width; i += 4) {
        glBegin(GL_LINES);
        glVertex3f(0, -height * zoom, 0);
        glVertex3f(0, height * zoom, 0);
        glEnd();
        glTranslatef(zoom * 4, 0, 0);
    }
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, -height * zoom, 0);
    glColor4f(0, 0, 1, 0);
    for (int i = 0; i < 2 * height; i += 4) {
        glBegin(GL_LINES);
        glVertex3f(-width * zoom, 0, 0);
        glVertex3f(width * zoom, 0, 0);
        glEnd();
        glTranslatef(0, zoom * 4, 0);
    }
    glPopMatrix();

    // 先画平面
    glPushMatrix();
    glTranslatef(0, 0, -0.2 * zoom);
    glColor4f(1, 1, 1, 1);
    glRectf(-width * zoom, -height * zoom, width * zoom, height * zoom);
    glPopMatrix();
    glEndList();

    // 任务
    vt.push_back(new std::thread(putTask, &gItemRepository1, &vSpeed[0]));
    vt.push_back(new std::thread(moveTask, &gItemRepository1, &gItemRepository2, &vSpeed[1]));
    vt.push_back(new std::thread(moveTask, &gItemRepository1, &gItemRepository3, &vSpeed[2]));
    vt.push_back(new std::thread(getTask, &gItemRepository2, &vSpeed[3]));
    vt.push_back(new std::thread(getTask, &gItemRepository3, &vSpeed[4]));
    for (auto &item:vt) item->detach();
}

int main(int argc, char *argv[]) {
    system("cls");
    std::cout << "Welcone\n\tq,w,e,r,t ---- Accelerated \n\ta,s,d,f,g ---- decelerated\n";
    std::cout << "\tyou can also use mouse to control field";

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowPosition(100, 100);
    glutInitWindowSize(WIDTH, HEIGHT);
    glutCreateWindow("Demo");  // 改了窗口标题

    glutDisplayFunc(myDisplay);
//    glutIdleFunc(myIdle);  // 表示在CPU空闲的时间调用某一函数
    glutTimerFunc(20, myIdle, 1);
    glutSpecialFunc(processSpecialKeys);  // 按键
    glutKeyboardFunc(processNormalKeys);
    glutReshapeFunc(changeSize);
    glutMouseFunc(mouse);
    glutMotionFunc(mouseMotion);

    init();
    glutMainLoop();
    return 0;
}
