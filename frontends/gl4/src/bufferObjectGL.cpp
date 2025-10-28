#include "PGRenderCoreGL/bufferObjectGL.h"
#include <GL/glew.h>
#include <stdexcept>
#include <cstring>

namespace pgrender {

    BufferObjectGL::BufferObjectGL(const BufferObject::Desc& desc)
        : m_desc(desc), m_bufferId(0), m_size(desc.size), m_isMapped(false)
    {
        if (m_size == 0) {
            throw std::invalid_argument("Buffer size cannot be zero");
        }

        glGenBuffers(1, &m_bufferId);
        if (m_bufferId == 0) {
            throw std::runtime_error("Failed to generate OpenGL buffer");
        }

        GLenum target = toGLTarget();
        GLenum usage = toGLUsage();

        glBindBuffer(target, m_bufferId);

        // Crear buffer con almacenamiento inmutable (más eficiente en OpenGL moderno)
        GLbitfield flags = 0;
        if (desc.usage == BufferUsage::Dynamic || desc.usage == BufferUsage::Stream) {
            flags = GL_DYNAMIC_STORAGE_BIT | GL_MAP_WRITE_BIT | GL_MAP_READ_BIT;
        }

        glBufferStorage(target, m_size, desc.data, flags);

        // Si no soporta glBufferStorage, usar el método legacy
        GLenum error = glGetError();
        if (error == GL_INVALID_OPERATION || error == GL_INVALID_ENUM) {
            glBufferData(target, m_size, desc.data, usage);
        }

        // Establecer label para debugging si se proporciona
        if (desc.debugName && glObjectLabel) {
            glObjectLabel(GL_BUFFER, m_bufferId, -1, desc.debugName);
        }

        glBindBuffer(target, 0);
    }

    BufferObjectGL::~BufferObjectGL() {
        if (m_isMapped) {
            unmap();
        }

        if (m_bufferId != 0) {
            glDeleteBuffers(1, &m_bufferId);
            m_bufferId = 0;
        }
    }

    void BufferObjectGL::update(const void* data, size_t size, size_t offset) {
        if (!data) {
            throw std::invalid_argument("Data pointer is null");
        }

        if (offset + size > m_size) {
            throw std::out_of_range("Update exceeds buffer size");
        }

        GLenum target = toGLTarget();
        glBindBuffer(target, m_bufferId);
        glBufferSubData(target, offset, size, data);
        glBindBuffer(target, 0);
    }

    void* BufferObjectGL::map(BufferAccessFlags access, size_t offset, size_t size) {
        if (m_isMapped) {
            throw std::runtime_error("Buffer is already mapped");
        }

        if (size == 0) {
            size = m_size - offset;
        }

        if (offset + size > m_size) {
            throw std::out_of_range("Map range exceeds buffer size");
        }

        GLenum target = toGLTarget();
        GLbitfield accessFlags = toGLAccessFlags(access);

        glBindBuffer(target, m_bufferId);
        void* ptr = glMapBufferRange(target, offset, size, accessFlags);
        glBindBuffer(target, 0);

        if (!ptr) {
            throw std::runtime_error("Failed to map buffer");
        }

        m_isMapped = true;
        return ptr;
    }

    void BufferObjectGL::unmap() {
        if (!m_isMapped) {
            throw std::runtime_error("Buffer is not mapped");
        }

        GLenum target = toGLTarget();
        glBindBuffer(target, m_bufferId);
        GLboolean success = glUnmapBuffer(target);
        glBindBuffer(target, 0);

        m_isMapped = false;

        if (!success) {
            throw std::runtime_error("Failed to unmap buffer (data may be corrupted)");
        }
    }

    void BufferObjectGL::copyFrom(const std::shared_ptr<BufferObject>& src,
        size_t srcOffset,
        size_t dstOffset,
        size_t size) {
        if (!src) {
            throw std::invalid_argument("Source buffer is null");
        }

        if (src->getBackendType() != BackendType::OpenGL) {
            throw std::runtime_error("Cannot copy from non-OpenGL buffer");
        }

        auto* srcGL = src->as<BufferObjectGL>();

        if (srcOffset + size > src->getSize()) {
            throw std::out_of_range("Source copy range exceeds buffer size");
        }

        if (dstOffset + size > m_size) {
            throw std::out_of_range("Destination copy range exceeds buffer size");
        }

        glCopyNamedBufferSubData(srcGL->nativeBufferId(), m_bufferId,
            srcOffset, dstOffset, size);
    }

    void BufferObjectGL::resize(size_t newSize, const void* data) {
        if (newSize == 0) {
            throw std::invalid_argument("New buffer size cannot be zero");
        }

        if (m_isMapped) {
            unmap();
        }

        m_size = newSize;

        GLenum target = toGLTarget();
        GLenum usage = toGLUsage();

        glBindBuffer(target, m_bufferId);

        // Realocar buffer (destruye contenido anterior)
        glBufferData(target, m_size, data, usage);

        glBindBuffer(target, 0);
    }

    unsigned int BufferObjectGL::toGLTarget() const {
        switch (m_desc.type) {
        case BufferType::Vertex: return GL_ARRAY_BUFFER;
        case BufferType::Index: return GL_ELEMENT_ARRAY_BUFFER;
        case BufferType::Uniform: return GL_UNIFORM_BUFFER;
        case BufferType::ShaderStorage: return GL_SHADER_STORAGE_BUFFER;
        case BufferType::TransferSrc:
        case BufferType::TransferDst:
            return GL_COPY_READ_BUFFER;
        default:
            throw std::runtime_error("Unknown buffer type");
        }
    }

    unsigned int BufferObjectGL::toGLUsage() const {
        switch (m_desc.usage) {
        case BufferUsage::Static: return GL_STATIC_DRAW;
        case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
        case BufferUsage::Stream: return GL_STREAM_DRAW;
        default:
            throw std::runtime_error("Unknown buffer usage");
        }
    }

    unsigned int BufferObjectGL::toGLAccessFlags(BufferAccessFlags flags) const {
        GLbitfield glFlags = 0;

        if (static_cast<uint32_t>(flags & BufferAccessFlags::Read)) {
            glFlags |= GL_MAP_READ_BIT;
        }

        if (static_cast<uint32_t>(flags & BufferAccessFlags::Write)) {
            glFlags |= GL_MAP_WRITE_BIT;
        }

        // Añadir flags adicionales para mejor performance
        if (m_desc.usage == BufferUsage::Stream) {
            glFlags |= GL_MAP_INVALIDATE_BUFFER_BIT | GL_MAP_UNSYNCHRONIZED_BIT;
        }

        return glFlags;
    }

} // namespace pgrender
