#include <pgrender/app.h>
#include <pgrender/windowManager.h>
#include <appFactory.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        // Crear contexto
        auto app = pgrender::AppFactory::createApp(pgrender::WindowBackend::Auto);
        
        // Configurar ventana
        pgrender::IWindow::Desc config;
        config.title = "PGRenderCore - Ventana Básica";
        config.width = 1280;
        config.height = 720;
        config.renderBackend = pgrender::RenderBackend::OpenGL4;
        

        auto &windowManager = app->getWindowManager();

        // Crear ventana
        auto windowID = windowManager.createWindow(config);
		auto* window = windowManager.getWindow(windowID);

        // Crear contexto gráfico
		auto graphicsContext = window->createContext(pgrender::GLContextDescriptor{});
        graphicsContext->makeCurrent();
        
        std::cout << "Ventana creada exitosamente\n";
        std::cout << "Backend: " << (graphicsContext->isShared() ? "Compartido" : "Normal") << "\n";
        
        // Loop principal
        int frameCount = 0;
		constexpr  int maxFrames = 3000;

        while (!window->shouldClose() && frameCount < maxFrames) {
            windowManager.pollEvents();
            
            pgrender::Event event;
            while (windowManager.getEventForWindow(windowID, event)) {
                if (event.type == pgrender::EventType::KeyPress) {
                    std::cout << "Tecla presionada (timestamp: " << event.timestamp << ")\n";
                    
                    if (event.key.key == pgrender::KeyCode::Escape) {
                        std::cout << "Escape presionado, cerrando...\n";
                        frameCount = maxFrames; // Forzar salida
                    }
                } else if (event.type == pgrender::EventType::WindowResize) {
                    std::cout << "Ventana redimensionada: " 
                              << event.windowResize.width << "x" 
                              << event.windowResize.height << "\n";
                }
            }
            
            // Aquí iría tu código de renderizado
            // glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            // glClear(GL_COLOR_BUFFER_BIT);
            
            graphicsContext->swapBuffers();
            
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            frameCount++;
            windowManager.processWindowClosures();
        }
        
        std::cout << "Aplicación finalizada correctamente\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
