#include <pgrender/renderCore.h>
#include <pgrender/renderCoreFactory.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
	try {
		// Crear contexto
		auto context = pgrender::RenderCoreFactory::createContext(pgrender::WindowBackend::Auto);
		auto& windowMgr = context->getWindowManager();

		// Mostrar información de displays
		int displayCount = windowMgr.getDisplayCount();
		std::cout << "========================================\n";
		std::cout << "Displays disponibles: " << displayCount << "\n";
		std::cout << "========================================\n";

		for (int i = 0; i < displayCount; ++i) {
			auto info = windowMgr.getDisplayInfo(i);
			std::cout << "Display " << i << ": " << info.name << "\n";
			std::cout << "  Resolución: " << info.bounds.width << "x" << info.bounds.height << "\n";
			std::cout << "  Posición: (" << info.bounds.x << ", " << info.bounds.y << ")\n";
			std::cout << "  Área usable: " << info.usableBounds.width << "x" << info.usableBounds.height << "\n";
			std::cout << "  Refresh rate: " << info.refreshRate << " Hz\n";
			std::cout << "  DPI: " << info.ddpi << "\n";
		}

		std::cout << "\n========================================\n";
		std::cout << "Creando ventanas...\n";
		std::cout << "========================================\n\n";

		// Crear ventana principal
		pgrender::WindowConfig config1;
		config1.title = "Ventana Principal";
		config1.width = 1280;
		config1.height = 720;
		config1.renderBackend = pgrender::RenderBackend::OpenGL4;

		auto window1Id = windowMgr.createWindow(config1);
		std::cout << "✓ Ventana 1 creada (ID: " << window1Id << ")\n";

		// Crear contexto para ventana principal
		pgrender::ContextConfig ctxConfig1;
		ctxConfig1.backend = pgrender::RenderBackend::OpenGL4;
		ctxConfig1.majorVersion = 4;
		ctxConfig1.minorVersion = 6;
		ctxConfig1.debugContext = false;

		auto* window1 = windowMgr.getWindow(window1Id);
		auto context1 = window1->createContext(ctxConfig1);
		windowMgr.setWindowContext(window1Id, std::move(context1));

		// Centrar ventana 1 en el display principal
		if (displayCount > 0) {
			windowMgr.centerWindowOnDisplay(window1Id, 0);
		}

		// Crear ventana secundaria con contexto compartido
		pgrender::WindowConfig config2;
		config2.title = "Ventana Secundaria - Contexto Compartido";
		config2.width = 800;
		config2.height = 600;
		config2.renderBackend = pgrender::RenderBackend::OpenGL4;

		auto window2Id = windowMgr.createWindow(config2);
		std::cout << "✓ Ventana 2 creada (ID: " << window2Id << ") [Contexto compartido]\n";

		// Crear contexto compartido
		pgrender::ContextConfig ctxConfig2;
		ctxConfig2.backend = pgrender::RenderBackend::OpenGL4;
		ctxConfig2.shareContext = windowMgr.getWindowContext(window1Id);

		auto* window2 = windowMgr.getWindow(window2Id);
		auto context2 = window2->createContext(ctxConfig2);
		windowMgr.setWindowContext(window2Id, std::move(context2));

		// Posicionar ventana 2 al lado de la ventana 1
		int x1, y1;
		windowMgr.getWindowPosition(window1Id, x1, y1);
		windowMgr.setWindowPosition(window2Id, x1 + 1300, y1);

		// Crear ventana terciaria en otro display (si hay más de uno)
		pgrender::WindowID window3Id = 0;
		if (displayCount > 1) {
			pgrender::WindowConfig config3;
			config3.title = "Ventana Terciaria - Display Secundario";
			config3.width = 640;
			config3.height = 480;
			config3.renderBackend = pgrender::RenderBackend::OpenGL4;

			window3Id = windowMgr.createWindow(config3);
			std::cout << "✓ Ventana 3 creada (ID: " << window3Id << ") [Display secundario]\n";

			// Crear contexto independiente
			auto* window3 = windowMgr.getWindow(window3Id);
			auto context3 = window3->createContext(pgrender::RenderBackend::OpenGL4);
			windowMgr.setWindowContext(window3Id, std::move(context3));

			// Centrar en el segundo display
			windowMgr.centerWindowOnDisplay(window3Id, 1);
		}
		else {
			std::cout << "  (Solo 1 display, ventana 3 omitida)\n";
		}

		std::cout << "\n========================================\n";
		std::cout << "Ventanas activas: " << windowMgr.getWindowCount() << "\n";
		std::cout << "========================================\n\n";

		// Configurar callbacks por ventana
		windowMgr.setWindowEventCallback(window1Id,
			[](pgrender::WindowID id, const pgrender::Event& event) {
				if (event.type == pgrender::EventType::KeyPress) {
					std::cout << "[Ventana " << id << "] Tecla presionada\n";
				}
				if (event.type == pgrender::EventType::WindowFocus) {
					std::cout << "[Ventana " << id << "] Obtuvo el foco\n";
				}
			});

		windowMgr.setWindowEventCallback(window2Id,
			[](pgrender::WindowID id, const pgrender::Event& event) {
				if (event.type == pgrender::EventType::MouseButtonPress) {
					std::cout << "[Ventana " << id << "] Click en ("
						<< event.mouseButton.x << ", "
						<< event.mouseButton.y << ")\n";
				}
				if (event.type == pgrender::EventType::WindowResize) {
					std::cout << "[Ventana " << id << "] Redimensionada: "
						<< event.windowResize.width << "x"
						<< event.windowResize.height << "\n";
				}
			});

		if (window3Id != 0) {
			windowMgr.setWindowEventCallback(window3Id,
				[](pgrender::WindowID id, const pgrender::Event& event) {
					if (event.type == pgrender::EventType::WindowMoved) {
						std::cout << "[Ventana " << id << "] Movida a ("
							<< event.windowMoved.x << ", "
							<< event.windowMoved.y << ")\n";
					}
				});
		}

		std::cout << "=== Controles ===\n";
		std::cout << "ESC (cualquier ventana) - Cerrar esa ventana\n";
		std::cout << "Q (cualquier ventana) - Cerrar todas las ventanas\n";
		std::cout << "1 - Enfocar ventana principal\n";
		std::cout << "2 - Enfocar ventana secundaria\n";
		if (window3Id != 0) {
			std::cout << "3 - Enfocar ventana terciaria\n";
		}
		std::cout << "M - Maximizar ventana con foco\n";
		std::cout << "R - Restaurar ventana con foco\n";
		std::cout << "X (cerrar ventana) - Cerrar ventana individual\n";
		std::cout << "\nLa aplicación se cerrará cuando todas las ventanas estén cerradas.\n\n";

		// Variables de estado
		int frameCount = 0;
		pgrender::WindowID focusedWindow = window1Id;

		// Loop principal
		while (windowMgr.hasOpenWindows()) {
			// Procesar eventos
			windowMgr.pollEvents();

			// Procesar eventos de cada ventana activa
			for (auto wid : windowMgr.getActiveWindows()) {
				pgrender::Event event;
				while (windowMgr.getEventForWindow(wid, event)) {
					// Eventos comunes a todas las ventanas
					if (event.type == pgrender::EventType::KeyPress) {
						// ESC - Cerrar ventana actual
						if (event.key.key == pgrender::KeyCode::Escape) {
							std::cout << "ESC en ventana " << wid << ", cerrando...\n";
							windowMgr.destroyWindow(wid);
							break;
						}
						// Q - Cerrar todas las ventanas
						else if (event.key.key == pgrender::KeyCode::Q) {
							std::cout << "Q presionado, cerrando todas las ventanas...\n";
							windowMgr.closeAllWindows();
							break;
						}
						// 1 - Enfocar ventana 1
						else if (event.key.key == pgrender::KeyCode::Num1) {
							windowMgr.raiseWindow(window1Id);
							focusedWindow = window1Id;
							std::cout << "Enfocando ventana 1\n";
						}
						// 2 - Enfocar ventana 2
						else if (event.key.key == pgrender::KeyCode::Num2) {
							windowMgr.raiseWindow(window2Id);
							focusedWindow = window2Id;
							std::cout << "Enfocando ventana 2\n";
						}
						// 3 - Enfocar ventana 3 (si existe)
						else if (event.key.key == pgrender::KeyCode::Num3 && window3Id != 0) {
							windowMgr.raiseWindow(window3Id);
							focusedWindow = window3Id;
							std::cout << "Enfocando ventana 3\n";
						}
						// M - Maximizar ventana actual
						else if (event.key.key == pgrender::KeyCode::M) {
							windowMgr.maximizeWindow(wid);
							std::cout << "Maximizando ventana " << wid << "\n";
						}
						// R - Restaurar ventana actual
						else if (event.key.key == pgrender::KeyCode::R) {
							windowMgr.restoreWindow(wid);
							std::cout << "Restaurando ventana " << wid << "\n";
						}
					}

					// Actualizar ventana enfocada
					if (event.type == pgrender::EventType::WindowFocus) {
						focusedWindow = wid;
					}
				}
			}

			// Cerrar ventanas marcadas (cuando se presiona X)
			windowMgr.processWindowClosures();

			// Renderizar cada ventana activa
			for (auto wid : windowMgr.getActiveWindows()) {
				auto* ctx = windowMgr.getWindowContext(wid);
				if (ctx) {
					ctx->makeCurrent();

					// Renderizado específico por ventana
					// Aquí podrías diferenciar el color de fondo o contenido por ventana
					// Ejemplo:
					// if (wid == window1Id) {
					//     glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
					// } else if (wid == window2Id) {
					//     glClearColor(0.4f, 0.2f, 0.3f, 1.0f);
					// } else {
					//     glClearColor(0.3f, 0.4f, 0.2f, 1.0f);
					// }
					// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

					ctx->swapBuffers();
				}
			}

			// Estadísticas cada 60 frames
			if (frameCount % 60 == 0) {
				std::cout << "[Frame " << frameCount << "] "
					<< "Ventanas: " << windowMgr.getWindowCount()
					<< " | Eventos en cola: " << windowMgr.getTotalQueuedEvents() << "\n";
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(16));
			frameCount++;
		}

		std::cout << "\n========================================\n";
		std::cout << "Todas las ventanas cerradas\n";
		std::cout << "Frames totales: " << frameCount << "\n";
		std::cout << "Tiempo aprox: " << (frameCount * 16 / 1000.0f) << " segundos\n";
		std::cout << "========================================\n";

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}

	return 0;
}
