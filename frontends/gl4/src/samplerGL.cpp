#include "PGRenderCoreGL/samplerGL.h"
#include <GL/glew.h>  // Solo aquí
#include <stdexcept>

namespace PGRenderCore {

    SamplerGL::SamplerGL(const Desc& desc)
        : m_desc(desc), m_samplerId(0)
    {
        glGenSamplers(1, reinterpret_cast<GLuint*>(&m_samplerId));
        if (m_samplerId == 0) {
            throw std::runtime_error("Failed to generate OpenGL sampler");
        }

        glSamplerParameteri(m_samplerId, GL_TEXTURE_MIN_FILTER, toGLFilterMode(m_desc.minFilter));
        glSamplerParameteri(m_samplerId, GL_TEXTURE_MAG_FILTER, toGLFilterMode(m_desc.magFilter));
        // OpenGL no tiene mipFilter explícito separado en sampler, el minFilter puede ser trilinear

        glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_S, toGLAddressMode(m_desc.addressU));
        glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_T, toGLAddressMode(m_desc.addressV));
        glSamplerParameteri(m_samplerId, GL_TEXTURE_WRAP_R, toGLAddressMode(m_desc.addressW));

        if (m_desc.maxAnisotropy > 1.0f) {
            GLfloat maxAniso = 0.0f;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAniso);
            GLfloat value = m_desc.maxAnisotropy > maxAniso ? maxAniso : static_cast<GLfloat>(m_desc.maxAnisotropy);
            glSamplerParameterf(m_samplerId, GL_TEXTURE_MAX_ANISOTROPY_EXT, value);
        }

        if (m_desc.compareEnable) {
            glSamplerParameteri(m_samplerId, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
            glSamplerParameteri(m_samplerId, GL_TEXTURE_COMPARE_FUNC, m_desc.compareFunc);
        }
        else {
            glSamplerParameteri(m_samplerId, GL_TEXTURE_COMPARE_MODE, GL_NONE);
        }
    }

    SamplerGL::~SamplerGL()
    {
        if (m_samplerId != 0) {
            GLuint id = static_cast<GLuint>(m_samplerId);
            glDeleteSamplers(1, &id);
            m_samplerId = 0;
        }
    }

    unsigned int SamplerGL::toGLFilterMode(FilterMode mode) const {
        switch (mode) {
        case FilterMode::Nearest: return GL_NEAREST;
        case FilterMode::Linear: return GL_LINEAR;
        case FilterMode::Anisotropic:
            // Anisotrópico no es filtro directo sino mejora, usar linear
            return GL_LINEAR;
        default: return GL_LINEAR;
        }
    }

    unsigned int SamplerGL::toGLAddressMode(AddressMode mode) const {
        switch (mode) {
        case AddressMode::Repeat: return GL_REPEAT;
        case AddressMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
        case AddressMode::MirroredRepeat: return GL_MIRRORED_REPEAT;
        case AddressMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
        default: return GL_REPEAT;
        }
    }

} // namespace PGRenderCore
