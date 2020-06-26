#ifndef OPENGLGAME_GL2DBASE_H
#define OPENGLGAME_GL2DBASE_H

class Base {
protected:

    float r{}, g{}, b{};
    float x{}, y{}, z{};

public:
    virtual void draw() = 0;

    virtual bool listen(int xx, int yy) = 0;

    virtual void setColor(float rr, float gg, float bb) {
        r = rr;
        g = gg;
        b = bb;
    }
};

#endif //OPENGLGAME_GL2DBASE_H
