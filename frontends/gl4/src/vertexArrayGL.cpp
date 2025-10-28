#include "PGRenderCoreGL/vertexArrayGL.h"
#include "PGRenderCoreGL/BufferObjectGL.h"
#include <GL/glew.h>
#include <stdexcept>

namespace pgrender {

    VertexArrayGL::VertexArrayGL(const VertexArray::Desc& desc)
        : m_vao(0), m_layout(desc.layout),
        m_vertexBuffers(desc.vertexBuffers),
        m_indexBuffer(desc.indexBuffer)
    {
        // Crear VAO
        glGenVertexArrays(1, &m_vao);
        if (m_vao == 0) {
            throw std::runtime_error("Failed to create Vertex Array Object");
        }

        glBindVertexArray(m_vao);

        // Configurar atributos de vértice
        setupVertexAttributes();

        // Vincular buffer de índices si existe
        if (m_indexBuffer) {
            if (m_indexBuffer->getBackendType() != BackendType::OpenGL) {
                throw std::runtime_error("Index buffer is not an OpenGL buffer");
            }
            auto* indexBufferGL = m_indexBuffer->as<BufferObjectGL>();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferGL->nativeBufferId());
        }

        // Desvincular VAO
        glBindVertexArray(0);
    }

    VertexArrayGL::~VertexArrayGL() {
        if (m_vao != 0) {
            glDeleteVertexArrays(1, &m_vao);
            m_vao = 0;
        }
    }

    void VertexArrayGL::setupVertexAttributes() {
        // Vincular buffers de vértices y configurar bindings
        for (const auto& bufferBinding : m_layout.getBufferBindings()) {
            if (bufferBinding.binding >= m_vertexBuffers.size()) {
                throw std::runtime_error("Buffer binding index out of range");
            }

            auto& buffer = m_vertexBuffers[bufferBinding.binding];
            if (!buffer) continue;

            if (buffer->getBackendType() != BackendType::OpenGL) {
                throw std::runtime_error("Vertex buffer is not an OpenGL buffer");
            }

            auto* bufferGL = buffer->as<BufferObjectGL>();

            // Vincular buffer al binding point
            glBindVertexBuffer(bufferBinding.binding,
                bufferGL->nativeBufferId(),
                0,
                bufferBinding.stride);

            // Configurar divisor para instancing
            if (bufferBinding.instanceRate) {
                glVertexBindingDivisor(bufferBinding.binding, bufferBinding.divisor);
            }
            else {
                glVertexBindingDivisor(bufferBinding.binding, 0);
            }
        }

        // Configurar atributos
        for (const auto& attr : m_layout.getAttributes()) {
            glEnableVertexAttribArray(attr.location);

            GLint componentCount = VertexLayout::getComponentCount(attr.type);
            GLenum glType = toGLAttributeType(attr.type);
            GLboolean normalized = attr.normalized ? GL_TRUE : GL_FALSE;

            // Especificar formato del atributo
            switch (attr.type) {
                // Tipos float
            case VertexAttributeType::Float:
            case VertexAttributeType::Float2:
            case VertexAttributeType::Float3:
            case VertexAttributeType::Float4:
            case VertexAttributeType::Half2:
            case VertexAttributeType::Half4:
                glVertexAttribFormat(attr.location, componentCount, glType,
                    normalized, attr.offset);
                break;

                // Tipos integer
            case VertexAttributeType::Int:
            case VertexAttributeType::Int2:
            case VertexAttributeType::Int3:
            case VertexAttributeType::Int4:
            case VertexAttributeType::UInt:
            case VertexAttributeType::UInt2:
            case VertexAttributeType::UInt3:
            case VertexAttributeType::UInt4:
            case VertexAttributeType::Byte4:
            case VertexAttributeType::UByte4:
            case VertexAttributeType::Short2:
            case VertexAttributeType::Short4:
            case VertexAttributeType::UShort2:
            case VertexAttributeType::UShort4:
                glVertexAttribIFormat(attr.location, componentCount, glType, attr.offset);
                break;

            default:
                throw std::runtime_error("Unsupported vertex attribute type");
            }

            // Asociar atributo con binding del buffer
            glVertexAttribBinding(attr.location, attr.binding);
        }
    }

    void VertexArrayGL::setVertexBuffer(uint32_t binding, std::shared_ptr<BufferObject> buffer) {
        if (binding >= m_vertexBuffers.size()) {
            m_vertexBuffers.resize(binding + 1);
        }

        m_vertexBuffers[binding] = buffer;

        // Actualizar VAO
        glBindVertexArray(m_vao);

        if (buffer) {
            if (buffer->getBackendType() != BackendType::OpenGL) {
                throw std::runtime_error("Buffer is not an OpenGL buffer");
            }

            auto* bufferGL = buffer->as<BufferObjectGL>();

            // Encontrar el stride del binding
            uint32_t stride = 0;
            for (const auto& bufferBinding : m_layout.getBufferBindings()) {
                if (bufferBinding.binding == binding) {
                    stride = bufferBinding.stride;
                    break;
                }
            }

            glBindVertexBuffer(binding, bufferGL->nativeBufferId(), 0, stride);
        }
        else {
            glBindVertexBuffer(binding, 0, 0, 0);
        }

        glBindVertexArray(0);
    }

    void VertexArrayGL::setIndexBuffer(std::shared_ptr<BufferObject> buffer) {
        m_indexBuffer = buffer;

        glBindVertexArray(m_vao);

        if (buffer) {
            if (buffer->getBackendType() != BackendType::OpenGL) {
                throw std::runtime_error("Buffer is not an OpenGL buffer");
            }
            auto* bufferGL = buffer->as<BufferObjectGL>();
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferGL->nativeBufferId());
        }
        else {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        }

        glBindVertexArray(0);
    }

    unsigned int VertexArrayGL::toGLAttributeType(VertexAttributeType type) const {
        switch (type) {
        case VertexAttributeType::Float:
        case VertexAttributeType::Float2:
        case VertexAttributeType::Float3:
        case VertexAttributeType::Float4:
            return GL_FLOAT;
        case VertexAttributeType::Int:
        case VertexAttributeType::Int2:
        case VertexAttributeType::Int3:
        case VertexAttributeType::Int4:
            return GL_INT;
        case VertexAttributeType::UInt:
        case VertexAttributeType::UInt2:
        case VertexAttributeType::UInt3:
        case VertexAttributeType::UInt4:
            return GL_UNSIGNED_INT;
        case VertexAttributeType::Byte4:
            return GL_BYTE;
        case VertexAttributeType::UByte4:
            return GL_UNSIGNED_BYTE;
        case VertexAttributeType::Short2:
        case VertexAttributeType::Short4:
            return GL_SHORT;
        case VertexAttributeType::UShort2:
        case VertexAttributeType::UShort4:
            return GL_UNSIGNED_SHORT;
        case VertexAttributeType::Half2:
        case VertexAttributeType::Half4:
            return GL_HALF_FLOAT;
        default:
            throw std::runtime_error("Unknown vertex attribute type");
        }
    }

} // namespace pgrender
