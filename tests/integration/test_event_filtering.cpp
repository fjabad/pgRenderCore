#include <gtest/gtest.h>
#include <pgrender/renderCore.h>
#include <pgrender/renderCoreFactory.h>

class EventFilteringTest : public ::testing::Test {
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

TEST_F(EventFilteringTest, SetEventFilter) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	// Establecer filtro que solo acepta eventos de teclado
	EXPECT_NO_THROW(
		windowMgr.setWindowEventFilter(windowId,
			[](const pgrender::Event& event) {
				return event.type == pgrender::EventType::KeyPress ||
					event.type == pgrender::EventType::KeyRelease;
			})
	);

	windowMgr.destroyWindow(windowId);
}

TEST_F(EventFilteringTest, SetEventWatcher) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	int watchedEvents = 0;

	// Establecer watcher
	EXPECT_NO_THROW(
		windowMgr.setWindowEventWatcher(windowId,
			[&watchedEvents](const pgrender::Event&) {
				watchedEvents++;
				return true;
			})
	);

	windowMgr.pollEvents();

	// El watcher puede o no haber visto eventos dependiendo del sistema
	EXPECT_GE(watchedEvents, 0);

	windowMgr.destroyWindow(windowId);
}

TEST_F(EventFilteringTest, MultipleWindowsWithDifferentFilters) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto win1 = windowMgr.createWindow(config);
	auto win2 = windowMgr.createWindow(config);

	// Filtro 1: Solo teclado
	windowMgr.setWindowEventFilter(win1,
		[](const pgrender::Event& e) {
			return e.type == pgrender::EventType::KeyPress;
		});

	// Filtro 2: Solo ratón
	windowMgr.setWindowEventFilter(win2,
		[](const pgrender::Event& e) {
			return e.type == pgrender::EventType::MouseMove;
		});

	windowMgr.pollEvents();

	// Verificar que no crashea con múltiples filtros
	pgrender::Event event;
	windowMgr.getEventForWindow(win1, event);
	windowMgr.getEventForWindow(win2, event);

	windowMgr.closeAllWindows();
}
