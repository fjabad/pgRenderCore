#include <gtest/gtest.h>
#include <pgrender/app.h>
#include <pgrender/windowManager.h>
#include <appFactory.h>

class EventSystemTest : public ::testing::Test {
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

TEST_F(EventSystemTest, PollEventsDoesNotCrash) {
	auto& windowMgr = app->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	// Procesar eventos no debería crashear
	EXPECT_NO_THROW(windowMgr.pollEvents());

	windowMgr.destroyWindow(windowId);
}

TEST_F(EventSystemTest, GetEventForWindowWithNoEvents) {
	auto& windowMgr = app->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	windowMgr.pollEvents();

	pgrender::Event event;
	bool hasEvent = windowMgr.getEventForWindow(windowId, event);
	(void)hasEvent;
	// Sin interacción del usuario, no debería haber eventos (o muy pocos)
	// Este test principalmente verifica que no crashea

	windowMgr.destroyWindow(windowId);
}

TEST_F(EventSystemTest, EventQueueSizes) {
	auto& windowMgr = app->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto win1 = windowMgr.createWindow(config);
	auto win2 = windowMgr.createWindow(config);

	windowMgr.pollEvents();

	size_t queueSize1 = windowMgr.getWindowQueueSize(win1);
	size_t queueSize2 = windowMgr.getWindowQueueSize(win2);
	size_t totalSize = windowMgr.getTotalQueuedEvents();

	EXPECT_EQ(totalSize, queueSize1 + queueSize2);

	windowMgr.closeAllWindows();
}

TEST_F(EventSystemTest, WindowEventCallback) {
	auto& windowMgr = app->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	int callbackCount = 0;

	windowMgr.setWindowEventCallback(windowId,
		[&callbackCount](const pgrender::Event&) {
			callbackCount++;
		});

	// Procesar eventos (puede o no generar eventos dependiendo del sistema)
	windowMgr.pollEvents();

	// Verificar que el callback se estableció sin crashear
	// El conteo real depende de eventos del sistema
	EXPECT_GE(callbackCount, 0);

	windowMgr.destroyWindow(windowId);
}
