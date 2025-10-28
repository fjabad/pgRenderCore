#include <pgrender/app.h>
#include <pgrender/windowManager.h>
#include <appFactory.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
	try {
		// Crear contexto (backend seleccionado automáticamente)
		auto app = pgrender::AppFactory::createApp(pgrender::WindowBackend::Auto);

		// Obtener el window manager (siempre disponible)
		auto& windowMgr = app->getWindowManager();

		// Configurar ventana
		pgrender::IWindow::Desc config;
		config.title = "PGRenderCore - Ventana Básica";
		config.width = 1280;
		config.height = 720;
		config.resizable = true;
		config.vsync = true;
		config.renderBackend = pgrender::RenderBackend::OpenGL4;

		// Crear ventana usando el window manager
		auto windowId = windowMgr.createWindow(config);

		// Obtener puntero a la ventana
		auto* window = windowMgr.getWindow(windowId);
		if (!window) {
			std::cerr << "Error: No se pudo obtener la ventana\n";
			return 1;
		}

		// Crear y asociar contexto gráfico
		pgrender::GLContextDescriptor ctxConfig;
		ctxConfig.majorVersion = 4;
		ctxConfig.minorVersion = 6;
		ctxConfig.debugContext = true;

		auto graphicsContext = window->createContext(ctxConfig);
		graphicsContext->makeCurrent();

		// Guardar el contexto en el window manager
		windowMgr.setWindowContext(windowId, std::move(graphicsContext));

		std::cout << "Ventana creada exitosamente\n";
		std::cout << "  ID: " << windowId << "\n";
		std::cout << "  Tamaño: " << config.width << "x" << config.height << "\n";

		auto* ctx = windowMgr.getWindowContext(windowId);
		if (ctx) {
			std::cout << "  Backend: "
				<< (ctx->getBackend() == pgrender::RenderBackend::OpenGL4 ? "OpenGL 4.x" : "Otro")
				<< "\n";
			std::cout << "  Compartido: " << (ctx->isShared() ? "Sí" : "No") << "\n";
		}

		// Mostrar información de displays
		std::cout << "\nDisplays disponibles: " << windowMgr.getDisplayCount() << "\n";
		for (int i = 0; i < windowMgr.getDisplayCount(); ++i) {
			auto displayInfo = windowMgr.getDisplayInfo(i);
			std::cout << "  Display " << i << ": " << displayInfo.name
				<< " (" << displayInfo.bounds.width << "x" << displayInfo.bounds.height
				<< " @ " << displayInfo.refreshRate << "Hz)\n";
		}

		// Configurar callback de eventos para esta ventana
		windowMgr.setWindowEventCallback(windowId,
			[windowId](const pgrender::Event& event) {
				if (event.type == pgrender::EventType::WindowResize) {
					std::cout << "[Ventana " << windowId << "] Redimensionada a: "
						<< event.windowResize.width << "x"
						<< event.windowResize.height << "\n";
				}
			});

		std::cout << "\n=== Controles ===\n";
		std::cout << "ESC - Cerrar aplicación\n";
		std::cout << "F - Toggle fullscreen\n";
		std::cout << "M - Maximizar ventana\n";
		std::cout << "R - Restaurar ventana\n";
		std::cout << "O - Cambiar opacidad\n";
		std::cout << "X o cerrar ventana - Salir\n\n";

		// Variables de estado
		bool isFullscreen = false;
		float opacity = 1.0f;
		int frameCount = 0;

		// Loop principal
		while (windowMgr.hasOpenWindows()) {
			// Procesar eventos del sistema
			windowMgr.pollEvents();

			// Procesar eventos de nuestra ventana
			pgrender::Event event;
			while (windowMgr.getEventForWindow(windowId, event)) {
				switch (event.type) {
				case pgrender::EventType::KeyPress:
					std::cout << "Tecla presionada (timestamp: " << event.timestamp << ") : " 
						<< static_cast<int>(event.key.key) << "\n";

					// ESC - Salir
					if (event.key.key == pgrender::KeyCode::Escape) {
						std::cout << "ESC presionado, cerrando...\n";
						windowMgr.destroyWindow(windowId);
					}
					// F - Toggle fullscreen
					else if (event.key.key == pgrender::KeyCode::F) {
						isFullscreen = !isFullscreen;
						windowMgr.setFullscreen(windowId, isFullscreen);
						std::cout << "Fullscreen: " << (isFullscreen ? "ON" : "OFF") << "\n";
					}
					// M - Maximizar
					else if (event.key.key == pgrender::KeyCode::M) {
						windowMgr.maximizeWindow(windowId);
						std::cout << "Ventana maximizada\n";
					}
					// R - Restaurar
					else if (event.key.key == pgrender::KeyCode::R) {
						windowMgr.restoreWindow(windowId);
						std::cout << "Ventana restaurada\n";
					}
					// O - Cambiar opacidad
					else if (event.key.key == pgrender::KeyCode::O) {
						opacity -= 0.2f;
						if (opacity < 0.2f) opacity = 1.0f;
						windowMgr.setWindowOpacity(windowId, opacity);
						std::cout << "Opacidad: " << (opacity * 100) << "%\n";
					}
					break;

				case pgrender::EventType::MouseButtonPress:
					std::cout << "Click en (" << event.mouseButton.x
						<< ", " << event.mouseButton.y << ")\n";
					break;

				case pgrender::EventType::MouseMove:
					// Mostrar cada 60 frames para no saturar
					if (frameCount % 60 == 0) {
						std::cout << "Mouse en (" << event.mouseMove.x
							<< ", " << event.mouseMove.y << ")\n";
					}
					break;

				case pgrender::EventType::WindowFocus:
					std::cout << "Ventana obtuvo el foco\n";
					break;

				case pgrender::EventType::WindowLostFocus:
					std::cout << "Ventana perdió el foco\n";
					break;

				case pgrender::EventType::WindowMinimized:
					std::cout << "Ventana minimizada\n";
					break;

				case pgrender::EventType::WindowMaximized:
					std::cout << "Ventana maximizada\n";
					break;

				case pgrender::EventType::WindowRestored:
					std::cout << "Ventana restaurada\n";
					break;

				default:
					break;
				}
			}

			// Procesar cierres de ventanas (cuando se presiona X)
			windowMgr.processWindowClosures();

			// Renderizado
			auto* renderCtx = windowMgr.getWindowContext(windowId);
			if (renderCtx) {
				renderCtx->makeCurrent();

				// Aquí iría tu código de renderizado OpenGL
				// Ejemplo:
				// glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
				// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				renderCtx->swapBuffers();
			}

			// Limitar a ~60 FPS
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
			frameCount++;
		}

		std::cout << "Aplicación finalizada correctamente (frames totales: "
			<< frameCount << ")\n";

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}

	return 0;
}
