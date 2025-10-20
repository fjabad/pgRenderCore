#pragma once
#include "pgrender/graphicsContext.h"
#include <GLFW/glfw3.h>
#include <memory>

namespace pgrender::backends::glfw {

    class GLFWGraphicsContext : public IGraphicsContext {
    public:
        explicit GLFWGraphicsContext(GLFWwindow* window, const ContextConfig& config);
        explicit GLFWGraphicsContext(const ContextConfig& config);
        ~GLFWGraphicsContext() override;

        void makeCurrent() override;
        void swapBuffers() override;
        void* getNativeHandle() const override { return m_window; }
        RenderBackend getBackend() const override { return RenderBackend::OpenGL4; }

        bool isShared() const override { return m_sharedContext != nullptr; }
        IGraphicsContext* getSharedContext() const override { return m_sharedContext; }

    private:
        GLFWwindow* m_window = nullptr;
        IGraphicsContext* m_sharedContext = nullptr;
        bool m_createdInternally = false;
    };

} // namespace pgrender::backends::glfw
