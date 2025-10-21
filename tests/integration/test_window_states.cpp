#include <gtest/gtest.h>
#include <pgrender/renderCore.h>
#include <renderCoreFactory.h>
#include <thread>
#include <chrono>

class WindowStatesTest : public ::testing::Test {
protected:
	void SetUp() override {
		context = pgrender::RenderCoreFactory::createContext();
		ASSERT_NE(context, nullptr);
	}

	void TearDown() override {
		context.reset();
	}

	std::unique_ptr<pgrender::ILibraryContext> context;

	void processSomeEvents() {
		auto& windowMgr = context->getWindowManager();
		for (int i = 0; i < 3; ++i) {
			windowMgr.pollEvents();
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
};

TEST_F(WindowStatesTest, MaximizeWindow) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	EXPECT_NO_THROW(windowMgr.maximizeWindow(windowId));
	processSomeEvents();

	windowMgr.destroyWindow(windowId);
}

TEST_F(WindowStatesTest, MinimizeWindow) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	EXPECT_NO_THROW(windowMgr.minimizeWindow(windowId));
	processSomeEvents();

	windowMgr.destroyWindow(windowId);
}

TEST_F(WindowStatesTest, RestoreWindow) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	windowMgr.maximizeWindow(windowId);
	processSomeEvents();

	EXPECT_NO_THROW(windowMgr.restoreWindow(windowId));
	processSomeEvents();

	windowMgr.destroyWindow(windowId);
}

TEST_F(WindowStatesTest, RaiseWindow) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto win1 = windowMgr.createWindow(config);
	auto win2 = windowMgr.createWindow(config);

	EXPECT_NO_THROW(windowMgr.raiseWindow(win1));
	processSomeEvents();

	EXPECT_NO_THROW(windowMgr.raiseWindow(win2));
	processSomeEvents();

	windowMgr.closeAllWindows();
}

TEST_F(WindowStatesTest, SetWindowOpacity) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	EXPECT_NO_THROW(windowMgr.setWindowOpacity(windowId, 0.5f));

	float opacity = windowMgr.getWindowOpacity(windowId);
	EXPECT_GE(opacity, 0.0f);
	EXPECT_LE(opacity, 1.0f);

	windowMgr.destroyWindow(windowId);
}

TEST_F(WindowStatesTest, SetBordered) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	EXPECT_NO_THROW(windowMgr.setBordered(windowId, false));
	processSomeEvents();

	EXPECT_NO_THROW(windowMgr.setBordered(windowId, true));
	processSomeEvents();

	windowMgr.destroyWindow(windowId);
}

TEST_F(WindowStatesTest, SetWindowPosition) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	windowMgr.setWindowPosition(windowId, 100, 200);
	processSomeEvents();

	int x, y;
	windowMgr.getWindowPosition(windowId, x, y);

	// Nota: Puede haber pequeñas diferencias debido a decoraciones de ventana
	EXPECT_NEAR(x, 100, 50);
	EXPECT_NEAR(y, 200, 50);

	windowMgr.destroyWindow(windowId);
}

TEST_F(WindowStatesTest, SetFullscreen) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	EXPECT_NO_THROW(windowMgr.setFullscreen(windowId, true));
	processSomeEvents();

	EXPECT_NO_THROW(windowMgr.setFullscreen(windowId, false));
	processSomeEvents();

	windowMgr.destroyWindow(windowId);
}

TEST_F(WindowStatesTest, SetFullscreenDesktop) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	EXPECT_NO_THROW(windowMgr.setFullscreenDesktop(windowId, true));
	processSomeEvents();

	EXPECT_NO_THROW(windowMgr.setFullscreenDesktop(windowId, false));
	processSomeEvents();

	windowMgr.destroyWindow(windowId);
}
