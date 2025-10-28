#include <pgrender/app.h>
#include <pgrender/windowManager.h>
#include <appFactory.h>
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
        auto app = pgrender::AppFactory::createApp();
        
        // Ventana principal
        pgrender::IWindow::Desc mainConfig;
        mainConfig.title = "PGRenderCore - Contextos Compartidos";
        mainConfig.width = 1280;
        mainConfig.height = 720;
        mainConfig.renderBackend = pgrender::RenderBackend::OpenGL4;
        
        auto& windowManager = app->getWindowManager();
        auto mainWindowID = windowManager.createWindow(mainConfig);

		auto* mainWindow = windowManager.getWindow(mainWindowID);
        
        // Contexto principal
		pgrender::GLContextDescriptor mainCtxConfig;
        mainCtxConfig.debugContext = true;
        
        auto mainContext = mainWindow->createContext(mainCtxConfig);
        
        
        std::cout << "Contexto principal creado\n";
        
        // Contexto compartido para carga en segundo plano
		pgrender::GLContextDescriptor  sharedConfig;
        sharedConfig.shareContext = mainContext.get();
        
        auto uploadContext = app->createHeadlessContext(sharedConfig);
        
        std::cout << "Contexto de carga creado (compartido: " 
                  << (uploadContext->isShared() ? "sí" : "no") << ")\n";
        
		mainContext->makeCurrent();

        // Iniciar thread de carga
        std::atomic<bool> running{true};
        std::thread worker(uploadThread, uploadContext.get(), std::ref(running));
        
        // Loop principal
        int frameCount = 0;
        
        while (!mainWindow->shouldClose() && frameCount < 300) {
            windowManager.pollEvents();
            
            pgrender::Event event;
            while (windowManager.getEventForWindow(mainWindowID, event)) {
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
			windowManager.processWindowClosures();
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
