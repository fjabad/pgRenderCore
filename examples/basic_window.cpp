#include <pgrender/renderCore.h>
#include <pgrender/renderCoreFactory.h>
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    try {
        // Crear contexto
        auto context = pgrender::RenderCoreFactory::createContext(pgrender::WindowBackend::Auto);
        
        // Configurar ventana
        pgrender::WindowConfig config;
        config.title = "PGRenderCore - Ventana Básica";
        config.width = 1280;
        config.height = 720;
        config.renderBackend = pgrender::RenderBackend::OpenGL4;
        
        // Crear ventana
        auto window = context->createWindow(config);
        
        // Crear contexto gráfico
        auto graphicsContext = window->createContext(pgrender::RenderBackend::OpenGL4);
        graphicsContext->makeCurrent();
        
        std::cout << "Ventana creada exitosamente\n";
        std::cout << "Backend: " << (graphicsContext->isShared() ? "Compartido" : "Normal") << "\n";
        
        // Sistema de eventos
        auto& eventSystem = context->getEventSystem();
        
        // Loop principal
        int frameCount = 0;
		constexpr  int maxFrames = 3000;

        while (!window->shouldClose() && frameCount < maxFrames) {
            eventSystem.pollEvents();
            
            pgrender::Event event;
            while (eventSystem.getEvent(event)) {
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
        }
        
        std::cout << "Aplicación finalizada correctamente\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
