#include <gtest/gtest.h>
#include <pgrender/app.h>
#include <pgrender/windowManager.h>
#include <appFactory.h>
#include <thread>
#include <chrono>

class MultiWindowTest : public ::testing::Test {
protected:
	void SetUp() override {
		app = pgrender::AppFactory::createApp();
		ASSERT_NE(app, nullptr);
	}

	void TearDown() override {
		app.reset();
	}

	std::unique_ptr<pgrender::App> app;
};

TEST_F(MultiWindowTest, CreateAndDestroyMultipleWindows) {
	auto& windowMgr = app->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	std::vector<pgrender::WindowID> windowIds;

	// Crear 5 ventanas
	for (int i = 0; i < 5; ++i) {
		config.title = "Window " + std::to_string(i);
		auto id = windowMgr.createWindow(config);
		windowIds.push_back(id);
	}

	EXPECT_EQ(windowMgr.getWindowCount(), 5u);

	// Destruir ventanas pares
	windowMgr.destroyWindow(windowIds[0]);
	windowMgr.destroyWindow(windowIds[2]);
	windowMgr.destroyWindow(windowIds[4]);

	EXPECT_EQ(windowMgr.getWindowCount(), 2u);

	// Destruir todas las restantes
	windowMgr.closeAllWindows();

	EXPECT_FALSE(windowMgr.hasOpenWindows());
	EXPECT_EQ(windowMgr.getWindowCount(), 0u);
}

TEST_F(MultiWindowTest, GetActiveWindows) {
	auto& windowMgr = app->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto win1 = windowMgr.createWindow(config);
	auto win2 = windowMgr.createWindow(config);
	auto win3 = windowMgr.createWindow(config);

	auto activeWindows = windowMgr.getActiveWindows();

	EXPECT_EQ(activeWindows.size(), 3u);
	EXPECT_NE(std::find(activeWindows.begin(), activeWindows.end(), win1), activeWindows.end());
	EXPECT_NE(std::find(activeWindows.begin(), activeWindows.end(), win2), activeWindows.end());
	EXPECT_NE(std::find(activeWindows.begin(), activeWindows.end(), win3), activeWindows.end());

	windowMgr.destroyWindow(win2);

	activeWindows = windowMgr.getActiveWindows();
	EXPECT_EQ(activeWindows.size(), 2u);
	EXPECT_EQ(std::find(activeWindows.begin(), activeWindows.end(), win2), activeWindows.end());

	windowMgr.closeAllWindows();
}

TEST_F(MultiWindowTest, IndependentContextsPerWindow) {
	auto& windowMgr = app->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;
	config.renderBackend = pgrender::RenderBackend::OpenGL4;

	auto win1 = windowMgr.createWindow(config);
	auto win2 = windowMgr.createWindow(config);

	// Crear contextos independientes
	auto* window1 = windowMgr.getWindow(win1);
	auto* window2 = windowMgr.getWindow(win2);

	auto ctx1 = window1->createContext(pgrender::RenderBackend::OpenGL4);
	auto ctx2 = window2->createContext(pgrender::RenderBackend::OpenGL4);

	ASSERT_NE(ctx1, nullptr);
	ASSERT_NE(ctx2, nullptr);
	EXPECT_NE(ctx1.get(), ctx2.get());

	windowMgr.setWindowContext(win1, std::move(ctx1));
	windowMgr.setWindowContext(win2, std::move(ctx2));

	auto* retrievedCtx1 = windowMgr.getWindowContext(win1);
	auto* retrievedCtx2 = windowMgr.getWindowContext(win2);

	EXPECT_NE(retrievedCtx1, nullptr);
	EXPECT_NE(retrievedCtx2, nullptr);
	EXPECT_NE(retrievedCtx1, retrievedCtx2);

	windowMgr.closeAllWindows();
}

TEST_F(MultiWindowTest, WindowClosureMarking) {
	auto& windowMgr = app->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);
	auto* window = windowMgr.getWindow(windowId);

	EXPECT_FALSE(window->shouldClose());

	// Simular marcado para cierre (normalmente hecho por el sistema de eventos)
	// Nota: Esta es una función de prueba, en producción esto lo hace el event system

	windowMgr.processWindowClosures();

	// Si la ventana estuviera marcada, se habría cerrado
	EXPECT_TRUE(windowMgr.hasOpenWindows());

	windowMgr.destroyWindow(windowId);
}
