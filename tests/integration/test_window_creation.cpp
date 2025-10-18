#include <gtest/gtest.h>
#include <pgrender/renderCore.h>
#include <pgrender/renderCoreFactory.h>
#include <thread>

class WindowCreationTest : public ::testing::Test {
protected:
	void SetUp() override {
		context = pgrender::RenderCoreFactory::createContext();
		ASSERT_NE(context, nullptr);
	}

	void TearDown() override {
		context.reset();
	}

	std::unique_ptr<pgrender::ILibraryContext> context;
};

TEST_F(WindowCreationTest, CreateSingleWindow) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.title = "Test Window";
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	EXPECT_NE(windowId, 0u);
	EXPECT_EQ(windowMgr.getWindowCount(), 1u);
	EXPECT_TRUE(windowMgr.hasOpenWindows());

	auto* window = windowMgr.getWindow(windowId);
	ASSERT_NE(window, nullptr);

	uint32_t width, height;
	window->getSize(width, height);
	EXPECT_EQ(width, 800u);
	EXPECT_EQ(height, 600u);

	windowMgr.destroyWindow(windowId);
	EXPECT_FALSE(windowMgr.hasOpenWindows());
}

TEST_F(WindowCreationTest, CreateMultipleWindows) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto win1 = windowMgr.createWindow(config);
	auto win2 = windowMgr.createWindow(config);
	auto win3 = windowMgr.createWindow(config);

	(void)win1; // Evitar advertencia de variable no usada
	(void)win2;	
	(void)win3;	

	EXPECT_EQ(windowMgr.getWindowCount(), 3u);

	auto activeWindows = windowMgr.getActiveWindows();
	EXPECT_EQ(activeWindows.size(), 3u);

	windowMgr.destroyWindow(win2);
	EXPECT_EQ(windowMgr.getWindowCount(), 2u);

	windowMgr.closeAllWindows();
	EXPECT_FALSE(windowMgr.hasOpenWindows());
}

TEST_F(WindowCreationTest, WindowVisibility) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);
	auto* window = windowMgr.getWindow(windowId);
	ASSERT_NE(window, nullptr);

	window->hide();
	// Nota: No hay forma portable de verificar si está oculta sin API específica

	window->show();
	// Verificar que no crash

	windowMgr.destroyWindow(windowId);
}

TEST_F(WindowCreationTest, WindowTitleChange) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.title = "Original Title";
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);
	auto* window = windowMgr.getWindow(windowId);
	ASSERT_NE(window, nullptr);

	window->setTitle("New Title");
	// Verificar que no crash

	windowMgr.destroyWindow(windowId);
}

TEST_F(WindowCreationTest, WindowResize) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);
	auto* window = windowMgr.getWindow(windowId);
	ASSERT_NE(window, nullptr);

	window->setSize(1024, 768);

#ifdef _WIN32
	uint32_t width, height;
	window->getSize(width, height);
	EXPECT_EQ(width, 1024u);
	EXPECT_EQ(height, 768u);
#else
    // IMPORTANTE: Procesar eventos para que el cambio tome efecto
    for (int i = 0; i < 10; ++i) {
        windowMgr.pollEvents();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    
    uint32_t width, height;
    window->getSize(width, height);
    
    // En Linux, el tamaño puede no ser exacto debido al window manager
    // Usar tolerancia en lugar de igualdad exacta
    EXPECT_NEAR(width, 1024u, 20u);  // Tolerancia de ±20 píxeles
    EXPECT_NEAR(height, 768u, 50u);  // Tolerancia mayor para altura (barra de título)
#endif


	windowMgr.destroyWindow(windowId);
}
