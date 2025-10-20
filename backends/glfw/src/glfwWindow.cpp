#include "pgrender/backends/glfw/GLFWWindow.hpp"
#include <stdexcept>

namespace pgrender::backends::glfw {

static void window_close_callback(GLFWwindow* window) {
    auto* instance = static_cast<GLFWWindow*>(glfwGetWindowUserPointer(window));
    if (instance) instance->markForClose();
}

GLFWWindow::GLFWWindow(const WindowConfig& config) : m_config(config) {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, config.resizable ? GLFW_TRUE : GLFW_FALSE);

    m_window = glfwCreateWindow(config.width, config.height, config.title.c_str(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwSetWindowUserPointer(m_window, this);
    glfwSetWindowCloseCallback(m_window, window_close_callback);
}

GLFWWindow::~GLFWWindow() {
    if (m_window) {
        glfwDestroyWindow(m_window);
    }
}

void GLFWWindow::show() { glfwShowWindow(m_window); }
void GLFWWindow::hide() { glfwHideWindow(m_window); }

void GLFWWindow::setTitle(const std::string& title) { glfwSetWindowTitle(m_window, title.c_str()); }

void GLFWWindow::setSize(uint32_t width, uint32_t height) { glfwSetWindowSize(m_window, width, height); }

void GLFWWindow::getSize(uint32_t& width, uint32_t& height) const {
    int w, h;
    glfwGetWindowSize(m_window, &w, &h);
    width = w;
    height = h;
}

bool GLFWWindow::shouldClose() const { return m_shouldClose || glfwWindowShouldClose(m_window); }

std::unique_ptr<IGraphicsContext> GLFWWindow::createContext(const ContextConfig& config) {
    auto ctx = std::make_unique<GLFWGraphicsContext>(m_window, config);
    return ctx;
}

void GLFWWindow::markForClose() { m_shouldClose = true; }

void GLFWWindow::setRelativeMouseMode(bool enabled) {
    glfwSetInputMode(m_window, GLFW_CURSOR, enabled ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

bool GLFWWindow::getRelativeMouseMode() const {
    return glfwGetInputMode(m_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

void GLFWWindow::setMouseGrab(bool grabbed) {
    glfwSetInputMode(m_window, GLFW_CURSOR, grabbed ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

bool GLFWWindow::getMouseGrab() const {
    return glfwGetInputMode(m_window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
}

} // namespace pgrender::backends::glfw
