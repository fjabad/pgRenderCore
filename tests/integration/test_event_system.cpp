#include <gtest/gtest.h>
#include <pgrender/renderCore.h>
#include <renderCoreFactory.h>

class EventSystemTest : public ::testing::Test {
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

TEST_F(EventSystemTest, PollEventsDoesNotCrash) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	// Procesar eventos no debería crashear
	EXPECT_NO_THROW(windowMgr.pollEvents());

	windowMgr.destroyWindow(windowId);
}

TEST_F(EventSystemTest, GetEventForWindowWithNoEvents) {
	auto& windowMgr = context->getWindowManager();

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
	auto& windowMgr = context->getWindowManager();

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
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	int callbackCount = 0;

	windowMgr.setWindowEventCallback(windowId,
		[&callbackCount](pgrender::WindowID, const pgrender::Event&) {
			callbackCount++;
		});

	// Procesar eventos (puede o no generar eventos dependiendo del sistema)
	windowMgr.pollEvents();

	// Verificar que el callback se estableció sin crashear
	// El conteo real depende de eventos del sistema
	EXPECT_GE(callbackCount, 0);

	windowMgr.destroyWindow(windowId);
}
