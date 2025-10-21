#pragma once

#include "pgrender/eventSystem.h"

#include <memory>

namespace pgrender::backends::glfw {

	class GLFWEventSystem : public IEventSystem {
	public:
		GLFWEventSystem();
		~GLFWEventSystem();

		// No copiable ni movible
		GLFWEventSystem(const GLFWEventSystem&) = delete;
		GLFWEventSystem& operator=(const GLFWEventSystem&) = delete;
		GLFWEventSystem(GLFWEventSystem&&) = delete;
		GLFWEventSystem& operator=(GLFWEventSystem&&) = delete;

		void registerWindow(WindowID windowId, IWindow* window) override;

		void pollEvents() override;

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
