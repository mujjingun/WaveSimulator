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
    size_t capacity = 0, size_ = 0, p = 0;
public:
    VertexArrayObject() noexcept = default;
    VertexArrayObject(size_t capacity) :
        capacity(capacity) {
        glGenBuffers(1, &buf_);
        glBindBuffer(GL_ARRAY_BUFFER, buf_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec<D>) * capacity, NULL, GL_STATIC_DRAW);

        glGenVertexArrays(1, &id_);
        glBindVertexArray(id_);
        glVertexAttribPointer(0, D, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
    }
    VertexArrayObject(std::vector<vec<D>> verts) noexcept :
        capacity(verts.size()), size_(verts.size()) {
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
        id_(std::exchange(v.id_, 0)), buf_(std::exchange(v.buf_, 0)),
        capacity(v.capacity), size_(v.size_), p(v.p) {}
    VertexArrayObject &operator= (VertexArrayObject &&v) {
        this->~VertexArrayObject();
        return *::new (static_cast<void*>(this)) VertexArrayObject(std::move(v));
    }
    GLuint id() const noexcept {
        return id_;
    }
    void draw(GLenum type) const noexcept {
        glBindVertexArray(id_);
        int tailcnt = size_, headcnt = 0;
        if (p + size_ > capacity) {
            tailcnt = capacity - p;
            headcnt = size_ - tailcnt;
        }
        glDrawArrays(type, p, tailcnt);
        if (headcnt) glDrawArrays(type, 0, headcnt);
    }
    void push_back(vec<D> v) noexcept {
        glBindBuffer(GL_ARRAY_BUFFER, buf_);
        int end = (p + size_) % capacity;
        glBufferSubData(GL_ARRAY_BUFFER, sizeof(vec<D>) * end, sizeof(vec<D>), &v);
        if (size_ < capacity) ++size_;
        else p = (p + 1) % capacity;
    }
    void pop_back() noexcept {
        if (size_ == 0) error("Queue empty");
        --size_;
    }
    int size() const noexcept {
        return size_;
    }
};

#endif /* BUFFER_H */
