#include "pgrender/windowManager.h"

namespace pgrender {

	//class IWindowManager::Impl {
	//	public:
	//	Impl() = default;
	//	~Impl() = default;


	//	std::mutex mutex;


	//};

	//IWindow* GLFWWindowManager::getWindow(WindowID id) {
	//	std::lock_guard<std::mutex> lock(m_impl->mutex);
	//	auto it = m_impl->windows.find(id);
	//	return (it != m_impl->windows.end()) ? it->second.window.get() : nullptr;
	//}

	//const IWindow* GLFWWindowManager::getWindow(WindowID id) const {
	//	std::lock_guard<std::mutex> lock(m_impl->mutex);
	//	auto it = m_impl->windows.find(id);
	//	return (it != m_impl->windows.end()) ? it->second.window.get() : nullptr;
	//}
}