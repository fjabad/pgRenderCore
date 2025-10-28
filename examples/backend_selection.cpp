#include <pgrender/app.h>
#include <pgrender/windowManager.h>
#include <appFactory.h>
#include <iostream>
#include <thread>
int main(int argc, char** argv) {
    try {
        // Seleccionar backend desde línea de comandos
        pgrender::WindowBackend backend = pgrender::WindowBackend::Auto;
        
        if (argc > 1) {
            std::string arg = argv[1];
            if (arg == "sdl3") {
                backend = pgrender::WindowBackend::SDL3;
                std::cout << "Usando backend: SDL3\n";
            } else if (arg == "glfw") {
                backend = pgrender::WindowBackend::GLFW;
                std::cout << "Usando backend: GLFW\n";
            } else {
                std::cout << "Uso: " << argv[0] << " [sdl3|glfw]\n";
                std::cout << "Usando backend automático\n";
            }
        } else {
            std::cout << "Usando backend automático\n";
        }
        
        // Crear contexto con backend seleccionado
        auto app = pgrender::AppFactory::createApp(backend);
        
        // Crear ventana
        pgrender::IWindow::Desc config;
        config.title = "PGRenderCore - Selección de Backend";
        config.width = 1280;
        config.height = 720;
        config.renderBackend = pgrender::RenderBackend::OpenGL4;
        
		
		auto& wm = app->getWindowManager();
        
        auto windowID = wm.createWindow(config);
		auto window = wm.getWindow(windowID);
		auto graphicsContext = window->createContext(pgrender::GLContextDescriptor{});
        graphicsContext->makeCurrent();
        
        std::cout << "Ventana creada exitosamente\n";
        std::cout << "Presiona ESC para salir\n";
        
        // Loop principal
        
        while (!window->shouldClose()) {
            wm.pollEvents();
            
            pgrender::Event event;
            while (wm.getEventForWindow(windowID, event)) {
                if (event.type == pgrender::EventType::KeyPress) {
                    if (event.key.key == pgrender::KeyCode::Escape) {
                        std::cout << "Cerrando aplicación...\n";
                        return 0;
                    }
                }
            }
            
            graphicsContext->swapBuffers();
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
