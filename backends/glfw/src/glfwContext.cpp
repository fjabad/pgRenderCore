#include "pgrender/backends/glfw/GLFWContext.hpp"
#include <stdexcept>

namespace pgrender::backends::glfw {

GLFWGraphicsContext::GLFWGraphicsContext(GLFWwindow* window, const ContextConfig& config)
    : m_window(window) {
    if (!window) {
        throw std::runtime_error("Invalid window handle for GLFW context creation");
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(config.vsync ? 1 : 0);
}

GLFWGraphicsContext::GLFWGraphicsContext(const ContextConfig& config) {
    if (!glfwInit()) throw std::runtime_error("GLFW Init failed");

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, config.majorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, config.minorVersion);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, config.debugContext ? GLFW_TRUE : GLFW_FALSE);

    m_window = glfwCreateWindow(1, 1, "", nullptr, nullptr);
    if (!m_window) throw std::runtime_error("Failed to create headless GLFW context");

    m_createdInternally = true;
    glfwMakeContextCurrent(m_window);
}

GLFWGraphicsContext::~GLFWGraphicsContext() {
    if (m_createdInternally && m_window) {
        glfwDestroyWindow(m_window);
        glfwTerminate();
    }
}

void GLFWGraphicsContext::makeCurrent() {
    glfwMakeContextCurrent(m_window);
}

void GLFWGraphicsContext::swapBuffers() {
    glfwSwapBuffers(m_window);
}

} // namespace pgrender::backends::glfw
