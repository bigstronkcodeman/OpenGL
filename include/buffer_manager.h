//
// Created by prbou on 1/10/2025.
//

#ifndef OPENGL_FUN_BUFFER_MANAGER_H
#define OPENGL_FUN_BUFFER_MANAGER_H

#include <glad/glad.h>
#include <vector>
#include <memory>


struct VertexAttribute {
    GLuint index;
    GLint size;
    GLenum type;
    GLboolean normalized;
    GLsizei stride;
    const void* offset;
};

class Buffer {
protected:
    GLuint handle{};
    GLenum bufferType;

public:
    explicit Buffer(GLenum bufferType) : bufferType(bufferType) {
        glGenBuffers(1, &handle);
    }

    virtual ~Buffer() {
        glDeleteBuffers(1, &handle);
    }

    void allocate(GLsizeiptr size, const void* data, GLenum usage) const {
        bind();
        glBufferData(bufferType, size, data, usage);
    }

    void update(GLintptr offset, GLsizeiptr size, const void* data) const {
        bind();
        glBufferSubData(bufferType, offset, size, data);
    }

    void bind() const {
        glBindBuffer(bufferType, handle);
    }

    [[nodiscard]] GLuint getHandle() const {
        return handle;
    }
};

class ShaderStorageBuffer : public Buffer {
private:
    GLuint bindingPoint;

public:
    ShaderStorageBuffer() : Buffer(GL_SHADER_STORAGE_BUFFER)
                          , bindingPoint() { }

    void bindToPoint(GLuint point) {
        bindingPoint = point;
        glBindBufferBase(bufferType, bindingPoint, handle);
    }
};

class VertexArrayObject {
private:
    GLuint handle;
    std::unique_ptr<Buffer> vertexBuffer;
    std::unique_ptr<Buffer> indexBuffer;

public:
    VertexArrayObject() {
        glGenVertexArrays(1, &handle);
        vertexBuffer = std::make_unique<Buffer>(GL_ARRAY_BUFFER);
        indexBuffer = std::make_unique<Buffer>(GL_ELEMENT_ARRAY_BUFFER);
    }

    ~VertexArrayObject() {
        glDeleteVertexArrays(1, &handle);
    }

    void bind() {
        glBindVertexArray(handle);
    }

    void setVertexAttributePointers(const std::vector<VertexAttribute>& attributes) {
        bind();
        vertexBuffer->bind();

        for (const VertexAttribute& attribute : attributes) {
            glEnableVertexAttribArray(attribute.index);
            glVertexAttribPointer(attribute.index, attribute.size, attribute.type,
                                  attribute.normalized, attribute.stride, attribute.offset);
        }
    }

    void allocate(GLsizeiptr size, const void* data, GLenum usage) const {
        vertexBuffer->allocate(size, data, usage);
    }
};

#endif //OPENGL_FUN_BUFFER_MANAGER_H
