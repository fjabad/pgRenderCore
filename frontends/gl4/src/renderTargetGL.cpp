#include "PGRenderCoreGL/renderTargetGL.h"
#include "PGRenderCoreGL/textureGL.h"  // Para dinámica_pointer_cast
#include <GL/glew.h>
#include <stdexcept>
#include <iostream>

namespace PGRenderCore {

    RenderTargetGL::RenderTargetGL(const Desc& desc)
        : m_desc(desc), m_width(desc.width), m_height(desc.height),
        m_colorAttachments(desc.colorAttachments),
        m_depthStencilAttachment(desc.depthStencilAttachment)
    {
        glGenFramebuffers(1, &m_fboId);
        if (m_fboId == 0) {
            throw std::runtime_error("Failed to generate framebuffer");
        }
        glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);

        // Attach color attachments
        std::vector<GLenum> drawBuffers;
        for (size_t i = 0; i < m_colorAttachments.size(); ++i) {
            auto texGL = std::dynamic_pointer_cast<TextureGL>(m_colorAttachments[i]);
            if (!texGL) {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                throw std::invalid_argument("Color attachment is not a valid TextureGL");
            }
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i),
                texGL->toGLTarget(), texGL->nativeTextureId(), 0);
            drawBuffers.push_back(GL_COLOR_ATTACHMENT0 + static_cast<GLenum>(i));
        }

        if (drawBuffers.empty()) {
            // Deshabilitar render target si no hay color
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }
        else {
            glDrawBuffers(static_cast<GLsizei>(drawBuffers.size()), drawBuffers.data());
        }

        // Attach depth/stencil if present
        if (m_depthStencilAttachment) {
            auto depthTexGL = std::dynamic_pointer_cast<TextureGL>(m_depthStencilAttachment);
            if (!depthTexGL) {
                glBindFramebuffer(GL_FRAMEBUFFER, 0);
                throw std::invalid_argument("DepthStencil attachment is not a valid TextureGL");
            }
            GLenum attachPoint = GL_DEPTH_STENCIL_ATTACHMENT;
            // Alternativamente se podrían comprobar formatos para separar DEPTH_ATTACHMENT o STENCIL_ATTACHMENT
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachPoint, depthTexGL->toGLTarget(), depthTexGL->nativeTextureId(), 0);
        }

        checkFramebufferStatus();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    RenderTargetGL::~RenderTargetGL()
    {
        if (m_fboId != 0) {
            glDeleteFramebuffers(1, &m_fboId);
            m_fboId = 0;
        }
    }

    std::shared_ptr<Texture> RenderTargetGL::getColorAttachment(uint32_t index) const
    {
        if (index >= m_colorAttachments.size()) return nullptr;
        return m_colorAttachments[index];
    }

    std::shared_ptr<Texture> RenderTargetGL::getDepthStencilAttachment() const
    {
        return m_depthStencilAttachment;
    }

    void RenderTargetGL::checkFramebufferStatus() const
    {
        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (status != GL_FRAMEBUFFER_COMPLETE) {
            std::string errorStr = "Framebuffer incomplete: ";
            switch (status) {
            case GL_FRAMEBUFFER_UNDEFINED: errorStr += "Undefined framebuffer"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT: errorStr += "Incomplete attachment"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT: errorStr += "Missing attachment"; break;
            case GL_FRAMEBUFFER_UNSUPPORTED: errorStr += "Unsupported framebuffer configuration"; break;
            case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE: errorStr += "Incomplete multisample"; break;
            default: errorStr += "Unknown error"; break;
            }
            throw std::runtime_error(errorStr);
        }
    }

} // namespace PGRenderCore
