#include "PGRenderCoreGL/deviceGL.h"
#include <stdexcept>
#include <GL/glew.h>
#include "PGRenderCoreGL/contextGL.h"
#include "PGRenderCoreGL/debugManagerGL.h"

namespace pgrender {

	DeviceGL::DeviceGL() {
	}

	DeviceGL::~DeviceGL() = default;

	std::unique_ptr<Context> DeviceGL::createContext(const Context::Desc& desc) {
		if (desc.nativeWindowHandle == nullptr) {
			throw std::invalid_argument("nativeWindowHandle is null");
		}

		auto ctx = std::make_unique<ContextGL>(desc);

		// Comprobaciones adicionales de versión/ extensiones aquí
		if (desc.enableDebug) {
			m_debugManager = std::make_unique<DebugManagerGL>();
			m_debugManager->initialize(true);
		}

		return ctx;
	}

	DebugManager* DeviceGL::getDebugManager() {
		return m_debugManager.get();
	}

} // namespace pgrender
