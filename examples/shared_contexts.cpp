#include <pgrender/RenderCore.hpp>
#include <pgrender/RenderCoreFactory.hpp>
#include <iostream>
#include <thread>
#include <atomic>

void uploadThread(pgrender::IGraphicsContext* context, std::atomic<bool>& running) {
    try {
        context->makeCurrent();
        std::cout << "[Upload Thread] Contexto activo\n";
        
        while (running) {
            // Simular carga de recursos (texturas, modelos, etc.)
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        
        std::cout << "[Upload Thread] Finalizando\n";
    } catch (const std::exception& e) {
        std::cerr << "[Upload Thread] Error: " << e.what() << "\n";
    }
}

int main() {
    try {
        auto context = pgrender::RenderCoreFactory::createContext();
        
        // Ventana principal
        pgrender::WindowConfig mainConfig;
        mainConfig.title = "PGRenderCore - Contextos Compartidos";
        mainConfig.width = 1280;
        mainConfig.height = 720;
        mainConfig.renderBackend = pgrender::RenderBackend::OpenGL4;
        
        auto mainWindow = context->createWindow(mainConfig);
        
        // Contexto principal
        pgrender::ContextConfig mainCtxConfig;
        mainCtxConfig.backend = pgrender::RenderBackend::OpenGL4;
        mainCtxConfig.debugContext = true;
        
        auto mainContext = mainWindow->createContext(mainCtxConfig);
        mainContext->makeCurrent();
        
        std::cout << "Contexto principal creado\n";
        
        // Contexto compartido para carga en segundo plano
        pgrender::ContextConfig sharedConfig;
        sharedConfig.backend = pgrender::RenderBackend::OpenGL4;
        sharedConfig.shareContext = mainContext.get();
        
        auto uploadContext = context->createHeadlessContext(sharedConfig);
        
        std::cout << "Contexto de carga creado (compartido: " 
                  << (uploadContext->isShared() ? "sí" : "no") << ")\n";
        
        // Iniciar thread de carga
        std::atomic<bool> running{true};
        std::thread worker(uploadThread, uploadContext.get(), std::ref(running));
        
        // Loop principal
        auto& events = context->getEventSystem();
        int frameCount = 0;
        
        while (!mainWindow->shouldClose() && frameCount < 300) {
            events.pollEvents();
            
            pgrender::Event event;
            while (events.getEvent(event)) {
                if (event.type == pgrender::EventType::KeyPress) {
                    if (event.key.key == pgrender::KeyCode::Escape) {
                        frameCount = 1000;
                    }
                }
            }
            
            mainContext->makeCurrent();
            // Renderizado principal aquí
            mainContext->swapBuffers();
            
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
            frameCount++;
        }
        
        // Detener thread
        running = false;
        worker.join();
        
        std::cout << "Aplicación finalizada\n";
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
    
    return 0;
}
