#include <pgrender/renderCore.h>
#include <pgrender/renderCoreFactory.h>
#include <iostream>

int main() {
	try {
		// Crear contexto (incluye window manager automáticamente)
		auto context = pgrender::RenderCoreFactory::createContext();
		auto& windowMgr = context->getWindowManager();

		// Crear ventanas
		pgrender::WindowConfig config;
		config.title = "Mi Aplicación";
		config.width = 1280;
		config.height = 720;

		auto window1 = windowMgr.createWindow(config);
		auto window2 = windowMgr.createWindow(config);

		// Loop principal
		while (windowMgr.hasOpenWindows()) {
			windowMgr.pollEvents();

			// Procesar eventos...

			windowMgr.processWindowClosures();
		}

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}

	return 0;
}
