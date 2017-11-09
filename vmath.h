#ifndef VMATH_H
#define VMATH_H

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

template<int D>
struct vec {
    GLfloat data[D];
};

template<>
struct vec<1> {
    GLfloat x;
};

template<>
struct vec<2> {
    GLfloat x, y;
};

template<>
struct vec<3> {
    GLfloat x, y, z;
};

template<>
struct vec<4> {
    GLfloat x, y, z, w;
};

#endif /* VMATH_H */
