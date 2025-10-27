#include "PGRenderCoreGL/textureGL.h"
#include <GL/glew.h>  // Solo aquí
#include <stdexcept>
#include <cstring>

namespace PGRenderCore {

    TextureGL::TextureGL(const Desc& desc)
        : m_desc(desc), m_textureId(0)
    {
        glGenTextures(1, reinterpret_cast<GLuint*>(&m_textureId));
        if (m_textureId == 0) {
            throw std::runtime_error("Failed to generate OpenGL texture");
        }

        GLenum target = static_cast<GLenum>(toGLTarget());
        glBindTexture(target, m_textureId);

        switch (m_desc.type) {
        case Type::Texture1D:
            glTexStorage1D(target, m_desc.mipLevels, toGLInternalFormat(), m_desc.width);
            break;
        case Type::Texture2D:
        case Type::TextureCube:
            glTexStorage2D(target, m_desc.mipLevels, toGLInternalFormat(), m_desc.width, m_desc.height);
            break;
        case Type::Texture3D:
            glTexStorage3D(target, m_desc.mipLevels, toGLInternalFormat(), m_desc.width, m_desc.height, m_desc.depth);
            break;
        case Type::TextureBuffer:
            // No se maneja en esta función, requiere buffer específico
            break;
        default:
            throw std::runtime_error("Unsupported texture type");
        }

        if (m_desc.mipmapped && m_desc.mipLevels == 1) {
            glGenerateMipmap(target);
        }

        glTexParameteri(target, GL_TEXTURE_MIN_FILTER, m_desc.mipmapped ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_REPEAT);
        if (m_desc.type == Type::Texture3D) {
            glTexParameteri(target, GL_TEXTURE_WRAP_R, GL_REPEAT);
        }

        glBindTexture(target, 0);
    }

    TextureGL::~TextureGL() {
        if (m_textureId != 0) {
            GLuint id = static_cast<GLuint>(m_textureId);
            glDeleteTextures(1, &id);
            m_textureId = 0;
        }
    }

    void TextureGL::update(const void* pixelData, size_t, uint32_t mipLevel, uint32_t arrayLayer) {
        GLenum target = static_cast<GLenum>(toGLTarget());
        glBindTexture(target, m_textureId);

        GLenum format = static_cast<GLenum>(toGLFormat());
        GLenum type = static_cast<GLenum>(toGLType());

        switch (m_desc.type) {
        case Type::Texture1D:
            glTexSubImage1D(target, mipLevel, 0, m_desc.width, format, type, pixelData);
            break;
        case Type::Texture2D:
            glTexSubImage2D(target, mipLevel, 0, 0, m_desc.width, m_desc.height, format, type, pixelData);
            break;
        case Type::TextureCube:
            if (arrayLayer > 5) throw std::out_of_range("Invalid cubemap layer");
            glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + arrayLayer, mipLevel, 0, 0, m_desc.width, m_desc.height, format, type, pixelData);
            break;
        case Type::Texture3D:
            glTexSubImage3D(target, mipLevel, 0, 0, 0, m_desc.width, m_desc.height, m_desc.depth, format, type, pixelData);
            break;
        case Type::TextureBuffer:
            // La actualización de texturas buffer no se maneja aquí
            break;
        default:
            throw std::runtime_error("Unsupported texture type for update");
        }

        glBindTexture(target, 0);
    }

    unsigned int TextureGL::toGLTarget() const {
        switch (m_desc.type) {
        case Type::Texture1D: return GL_TEXTURE_1D;
        case Type::Texture2D: return GL_TEXTURE_2D;
        case Type::Texture3D: return GL_TEXTURE_3D;
        case Type::TextureCube: return GL_TEXTURE_CUBE_MAP;
        case Type::TextureBuffer: return GL_TEXTURE_BUFFER;
        default: throw std::runtime_error("Unknown texture type");
        }
    }

    unsigned int TextureGL::toGLInternalFormat() const {
        switch (m_desc.format) {
        case Format::R8: return GL_R8;
        case Format::RG8: return GL_RG8;
        case Format::RGB8: return GL_RGB8;
        case Format::RGBA8: return GL_RGBA8;
        case Format::R16F: return GL_R16F;
        case Format::RG16F: return GL_RG16F;
        case Format::RGB16F: return GL_RGB16F;
        case Format::RGBA16F: return GL_RGBA16F;
        case Format::R32F: return GL_R32F;
        case Format::RG32F: return GL_RG32F;
        case Format::RGB32F: return GL_RGB32F;
        case Format::RGBA32F: return GL_RGBA32F;
        case Format::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
        case Format::Depth32F: return GL_DEPTH_COMPONENT32F;
        default: throw std::runtime_error("Unsupported texture format");
        }
    }

    unsigned int TextureGL::toGLFormat() const {
        switch (m_desc.format) {
        case Format::R8: case Format::R16F: case Format::R32F: return GL_RED;
        case Format::RG8: case Format::RG16F: case Format::RG32F: return GL_RG;
        case Format::RGB8: case Format::RGB16F: case Format::RGB32F: return GL_RGB;
        case Format::RGBA8: case Format::RGBA16F: case Format::RGBA32F: return GL_RGBA;
        case Format::Depth24Stencil8: return GL_DEPTH_STENCIL;
        case Format::Depth32F: return GL_DEPTH_COMPONENT;
        default: throw std::runtime_error("Unsupported format for GL format");
        }
    }

    unsigned int TextureGL::toGLType() const {
        switch (m_desc.format) {
        case Format::R8: case Format::RG8: case Format::RGB8: case Format::RGBA8:
            return GL_UNSIGNED_BYTE;
        case Format::R16F: case Format::RG16F: case Format::RGB16F: case Format::RGBA16F:
            return GL_HALF_FLOAT;
        case Format::R32F: case Format::RG32F: case Format::RGB32F: case Format::RGBA32F: case Format::Depth32F:
            return GL_FLOAT;
        case Format::Depth24Stencil8:
            return GL_UNSIGNED_INT_24_8;
        default:
            throw std::runtime_error("Unsupported format for GL type");
        }
    }

} // namespace PGRenderCore
