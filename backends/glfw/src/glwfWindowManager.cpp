#include "pgrender/backends/glfw/GLFWFeatures.hpp"
#include "pgrender/backends/glfw/GLFWWindow.hpp"
#include <GLFW/glfw3.h>

using namespace pgrender::backends::glfw;

GLFWWindowManager::GLFWWindowManager() { glfwInit(); }
GLFWWindowManager::~GLFWWindowManager() { closeAllWindows(); glfwTerminate(); }

WindowID GLFWWindowManager::createWindow(const WindowConfig& config) {
    auto window = std::make_unique<GLFWWindow>(config);
    WindowID id = reinterpret_cast<WindowID>(window->getGLFWHandle());
    m_windows[id].window = std::move(window);
    return id;
}
