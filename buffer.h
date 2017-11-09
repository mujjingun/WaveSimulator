#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <utility>

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "vmath.h"

template <int D>
class VertexArrayObject {
    GLuint id_ = 0, buf_ = 0;
    size_t size = 0;
public:
    VertexArrayObject() noexcept = default;
    VertexArrayObject(std::vector<vec<D>> verts) noexcept : size(verts.size()) {
        glGenBuffers(1, &buf_);
        glBindBuffer(GL_ARRAY_BUFFER, buf_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec<D>) * verts.size(), verts.data(), GL_STATIC_DRAW);

        glGenVertexArrays(1, &id_);
        glBindVertexArray(id_);
        glVertexAttribPointer(0, D, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
    }
    ~VertexArrayObject() noexcept {
    }
    VertexArrayObject(const VertexArrayObject&) = delete;
    VertexArrayObject &operator= (const VertexArrayObject&) = delete;
    VertexArrayObject(VertexArrayObject &&v) :
        id_(std::exchange(v.id_, 0)), buf_(std::exchange(v.buf_, 0)), size(v.size) {}
    VertexArrayObject &operator= (VertexArrayObject &&v) {
        this->~VertexArrayObject();
        return *::new (static_cast<void*>(this)) VertexArrayObject(std::move(v));
    }
    GLuint id() const noexcept {
        return id_;
    }
    void draw(GLenum type) const noexcept {
        glBindVertexArray(id_);
        glDrawArrays(type, 0, size);
    }
    void add_to_buf(vec<D> p) noexcept {
        glBindBuffer(GL_ARRAY_BUFFER, buf_);
    }
};

#endif /* BUFFER_H */
