#include <pgrender/app.h>
#include <pgrender/windowManager.h>
#include <appFactory.h>
#include <iostream>

int main() {
	try {
		// Crear contexto (incluye window manager automáticamente)
		auto app = pgrender::AppFactory::createApp();
		auto& windowMgr = app->getWindowManager();

		// Crear ventanas
		pgrender::IWindow::Desc config;
		config.title = "Mi Aplicación";
		config.width = 1280;
		config.height = 720;

		auto window1 = windowMgr.createWindow(config);
		//auto window2 = windowMgr.createWindow(config);

		pgrender::GLContextDescriptor glCtx;
		
		windowMgr.setWindowContext(window1, glCtx);

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
