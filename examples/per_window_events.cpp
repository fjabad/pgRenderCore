#include <pgrender/renderCore.h>
#include <pgrender/renderCoreFactory.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
	try {
		// Crear contexto
		auto context = pgrender::RenderCoreFactory::createContext();
		auto& windowMgr = context->getWindowManager();

		std::cout << "========================================\n";
		std::cout << "Colas de Eventos por Ventana\n";
		std::cout << "========================================\n\n";

		// Ventana 1: Solo eventos de teclado
		pgrender::WindowConfig config1;
		config1.title = "Ventana 1 - Solo Teclado";
		config1.width = 800;
		config1.height = 600;
		config1.renderBackend = pgrender::RenderBackend::OpenGL4;

		auto window1Id = windowMgr.createWindow(config1);
		std::cout << "✓ Ventana 1 creada (ID: " << window1Id << ") - Solo teclado\n";

		// Ventana 2: Solo eventos de ratón
		pgrender::WindowConfig config2;
		config2.title = "Ventana 2 - Solo Ratón";
		config2.width = 800;
		config2.height = 600;
		config2.renderBackend = pgrender::RenderBackend::OpenGL4;

		auto window2Id = windowMgr.createWindow(config2);
		std::cout << "✓ Ventana 2 creada (ID: " << window2Id << ") - Solo ratón\n";

		// Ventana 3: Todos los eventos
		pgrender::WindowConfig config3;
		config3.title = "Ventana 3 - Todos los Eventos";
		config3.width = 800;
		config3.height = 600;
		config3.renderBackend = pgrender::RenderBackend::OpenGL4;

		auto window3Id = windowMgr.createWindow(config3);
		std::cout << "✓ Ventana 3 creada (ID: " << window3Id << ") - Todos los eventos\n";

		// Posicionar ventanas en cascada
		windowMgr.setWindowPosition(window1Id, 100, 100);
		windowMgr.setWindowPosition(window2Id, 150, 150);
		windowMgr.setWindowPosition(window3Id, 200, 200);

		// Crear contextos para todas las ventanas
		for (auto wid : { window1Id, window2Id, window3Id }) {
			pgrender::ContextConfig ctxConfig;
			ctxConfig.backend = pgrender::RenderBackend::OpenGL4;

			auto* window = windowMgr.getWindow(wid);
			auto context = window->createContext(ctxConfig);
			windowMgr.setWindowContext(wid, std::move(context));
		}

		std::cout << "\n========================================\n";
		std::cout << "Configurando filtros de eventos...\n";
		std::cout << "========================================\n\n";

		// Configurar filtro para ventana 1: Solo teclado
		windowMgr.setWindowEventFilter(window1Id, [](const pgrender::Event& event) {
			return event.type == pgrender::EventType::KeyPress ||
				event.type == pgrender::EventType::KeyRelease;
			});
		std::cout << "✓ Filtro configurado para Ventana 1: Solo teclado\n";

		// Configurar filtro para ventana 2: Solo ratón
		windowMgr.setWindowEventFilter(window2Id, [](const pgrender::Event& event) {
			return event.type == pgrender::EventType::MouseMove ||
				event.type == pgrender::EventType::MouseButtonPress ||
				event.type == pgrender::EventType::MouseButtonRelease ||
				event.type == pgrender::EventType::MouseScroll;
			});
		std::cout << "✓ Filtro configurado para Ventana 2: Solo ratón\n";

		// Ventana 3: Sin filtro (recibe todos los eventos)
		std::cout << "✓ Ventana 3: Sin filtro (todos los eventos)\n";

		// Opcional: Configurar watchers (observadores) para logging
		windowMgr.setWindowEventWatcher(window1Id, [](const pgrender::Event& event) {
			// Este watcher ve todos los eventos antes del filtro
			return true; // No afecta el filtrado
			});

		std::cout << "\n========================================\n";
		std::cout << "=== Instrucciones ===\n";
		std::cout << "========================================\n";
		std::cout << "Interactúa con cada ventana para ver los filtros en acción:\n\n";
		std::cout << "Ventana 1:\n";
		std::cout << "  - Presiona teclas → Se registran\n";
		std::cout << "  - Mueve el ratón → Se ignoran\n";
		std::cout << "  - Haz click → Se ignoran\n\n";
		std::cout << "Ventana 2:\n";
		std::cout << "  - Presiona teclas → Se ignoran\n";
		std::cout << "  - Mueve el ratón → Se registran\n";
		std::cout << "  - Haz click → Se registran\n\n";
		std::cout << "Ventana 3:\n";
		std::cout << "  - Todo se registra\n\n";
		std::cout << "ESC (en cualquier ventana) → Cerrar esa ventana\n";
		std::cout << "Q (en cualquier ventana) → Cerrar todas\n";
		std::cout << "========================================\n\n";

		// Contadores de eventos por ventana
		int keyEvents1 = 0;
		int mouseEvents2 = 0;
		int totalEvents3 = 0;
		int frameCount = 0;

		// Loop principal
		while (windowMgr.hasOpenWindows()) {
			// Procesar eventos del sistema
			windowMgr.pollEvents();

			// Procesar eventos de ventana 1 (solo teclado)
			pgrender::Event event1;
			while (windowMgr.getEventForWindow(window1Id, event1)) {
				keyEvents1++;

				if (event1.type == pgrender::EventType::KeyPress) {
					std::cout << "[W1 Teclado] Tecla presionada (código: "
						<< static_cast<int>(event1.key.key) << ")\n";

					// ESC - Cerrar ventana 1
					if (event1.key.key == pgrender::KeyCode::Escape) {
						std::cout << "[W1] ESC presionado, cerrando ventana 1...\n";
						windowMgr.destroyWindow(window1Id);
						break;
					}
					// Q - Cerrar todas
					else if (event1.key.key == pgrender::KeyCode::Q) {
						std::cout << "[W1] Q presionado, cerrando todas las ventanas...\n";
						windowMgr.closeAllWindows();
						break;
					}
				}
				else if (event1.type == pgrender::EventType::KeyRelease) {
					std::cout << "[W1 Teclado] Tecla liberada (código: "
						<< static_cast<int>(event1.key.key) << ")\n";
				}
			}

			// Procesar eventos de ventana 2 (solo ratón)
			pgrender::Event event2;
			while (windowMgr.getEventForWindow(window2Id, event2)) {
				mouseEvents2++;

				if (event2.type == pgrender::EventType::MouseButtonPress) {
					std::cout << "[W2 Ratón] Click en ("
						<< event2.mouseButton.x << ", "
						<< event2.mouseButton.y << ") - Botón: "
						<< static_cast<int>(event2.mouseButton.button) << "\n";
				}
				else if (event2.type == pgrender::EventType::MouseMove) {
					// Solo mostrar cada 30 eventos para no saturar
					if (mouseEvents2 % 30 == 0) {
						std::cout << "[W2 Ratón] Posición: ("
							<< event2.mouseMove.x << ", "
							<< event2.mouseMove.y << ") Delta: ("
							<< event2.mouseMove.deltaX << ", "
							<< event2.mouseMove.deltaY << ")\n";
					}
				}
				else if (event2.type == pgrender::EventType::MouseScroll) {
					std::cout << "[W2 Ratón] Scroll: deltaY="
						<< event2.mouseScroll.deltaY << "\n";
				}
			}

			// Procesar eventos de ventana 3 (todos)
			pgrender::Event event3;
			while (windowMgr.getEventForWindow(window3Id, event3)) {
				totalEvents3++;

				// Mostrar tipo de evento
				std::string eventName;
				switch (event3.type) {
				case pgrender::EventType::KeyPress:
					eventName = "KeyPress";

					// ESC - Cerrar ventana 3
					if (event3.key.key == pgrender::KeyCode::Escape) {
						std::cout << "[W3] ESC presionado, cerrando ventana 3...\n";
						windowMgr.destroyWindow(window3Id);
						break;
					}
					// Q - Cerrar todas
					else if (event3.key.key == pgrender::KeyCode::Q) {
						std::cout << "[W3] Q presionado, cerrando todas las ventanas...\n";
						windowMgr.closeAllWindows();
						break;
					}
					break;
				case pgrender::EventType::KeyRelease:
					eventName = "KeyRelease";
					break;
				case pgrender::EventType::MouseMove:
					eventName = "MouseMove";
					// Solo mostrar cada 60 para no saturar
					if (totalEvents3 % 60 != 0) continue;
					break;
				case pgrender::EventType::MouseButtonPress:
					eventName = "MouseButtonPress";
					break;
				case pgrender::EventType::MouseButtonRelease:
					eventName = "MouseButtonRelease";
					break;
				case pgrender::EventType::MouseScroll:
					eventName = "MouseScroll";
					break;
				case pgrender::EventType::WindowResize:
					eventName = "WindowResize";
					break;
				case pgrender::EventType::WindowFocus:
					eventName = "WindowFocus";
					break;
				case pgrender::EventType::WindowLostFocus:
					eventName = "WindowLostFocus";
					break;
				default:
					eventName = "Otro";
					break;
				}

				std::cout << "[W3 Todos] Evento: " << eventName
					<< " (timestamp: " << event3.timestamp << ")\n";
			}

			// Cerrar ventanas marcadas
			windowMgr.processWindowClosures();

			// Renderizar ventanas activas
			for (auto wid : windowMgr.getActiveWindows()) {
				auto* ctx = windowMgr.getWindowContext(wid);
				if (ctx) {
					ctx->makeCurrent();
					// Aquí iría el renderizado
					ctx->swapBuffers();
				}
			}

			// Mostrar estadísticas cada 120 frames (~2 segundos)
			if (frameCount % 120 == 0 && frameCount > 0) {
				std::cout << "\n--- Estadísticas (Frame " << frameCount << ") ---\n";
				std::cout << "Ventanas activas: " << windowMgr.getWindowCount() << "\n";
				std::cout << "Eventos W1 (teclado): " << keyEvents1 << "\n";
				std::cout << "Eventos W2 (ratón): " << mouseEvents2 << "\n";
				std::cout << "Eventos W3 (todos): " << totalEvents3 << "\n";
				std::cout << "Eventos en cola total: " << windowMgr.getTotalQueuedEvents() << "\n";

				// Detalles por ventana
				for (auto wid : windowMgr.getActiveWindows()) {
					std::cout << "  Cola ventana " << wid << ": "
						<< windowMgr.getWindowQueueSize(wid) << " eventos\n";
				}
				std::cout << "-----------------------------------\n\n";
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(16));
			frameCount++;
		}

		std::cout << "\n========================================\n";
		std::cout << "Resumen Final\n";
		std::cout << "========================================\n";
		std::cout << "Frames totales: " << frameCount << "\n";
		std::cout << "Eventos de teclado (W1): " << keyEvents1 << "\n";
		std::cout << "Eventos de ratón (W2): " << mouseEvents2 << "\n";
		std::cout << "Eventos totales (W3): " << totalEvents3 << "\n";
		std::cout << "========================================\n";

	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << "\n";
		return 1;
	}

	return 0;
}
