#include <PGRenderCoreGL/renderPassGL.h>
#include <GL/glew.h>
#include <stdexcept>
#include <PGRenderCoreGL/renderTargetGL.h>

namespace pgrender {

    RenderPassGL::RenderPassGL(const RenderPass::Desc& desc)
        : m_desc(desc), m_isActive(false)
    {
        // Aquí puedes validar el descriptor, verificar compatibilidad, etc.
    }

    RenderPassGL::~RenderPassGL()
    {
        if (m_isActive) {
            end();
        }
    }

    const RenderPass::Desc& RenderPassGL::getDesc() const {
        return m_desc;
    }

    uint64_t RenderPassGL::nativeHandle() const {
        // OpenGL no tiene objeto explícito de render pass, retorna 0 o algún identificador interno si se usa
        return 0;
    }

    void RenderPassGL::begin() {
        if (m_isActive) {
            throw std::runtime_error("RenderPass already active");
        }
        m_isActive = true;

        // Bind framebuffer si existe
        if (m_desc.renderTarget) {
            // Cast seguro a RenderTargetGL para obtener handle y bindear FBO
            // Asume dynamic_pointer_cast, pero puedes añadir método virtual en RenderTarget para bind/unbind
            auto rtGL = std::dynamic_pointer_cast<RenderTargetGL>(m_desc.renderTarget);
            if (!rtGL) {
                throw std::runtime_error("Invalid RenderTarget for RenderPass");
            }
            glBindFramebuffer(GL_FRAMEBUFFER, static_cast<GLuint>(rtGL->nativeHandle()));
        }
        else {
            // Bind framebuffer por defecto
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        // Clear si está definido en descriptor
        GLuint clearMask = 0;
        if (m_desc.clearColor) {
            auto & c = m_desc.clearColorValue;
            glClearColor(c[0], c[1], c[2], c[3]);
            clearMask |= GL_COLOR_BUFFER_BIT;
        }
        if (m_desc.clearDepth) {
            glClearDepth(m_desc.clearDepthValue);
            clearMask |= GL_DEPTH_BUFFER_BIT;
        }
        if (m_desc.clearStencil) {
            glClearStencil(m_desc.clearStencilValue);
            clearMask |= GL_STENCIL_BUFFER_BIT;
        }
        if (clearMask != 0) {
            glClear(clearMask);
        }
    }

    void RenderPassGL::end() {
        if (!m_isActive) {
            throw std::runtime_error("RenderPass not active");
        }
        m_isActive = false;

        // Unbind framebuffer (volver a default)
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

} // namespace pgrender
