#include <gtest/gtest.h>
#include <pgrender/app.h>
#include <pgrender/windowManager.h>
#include <appFactory.h>

class SharedContextsTest : public ::testing::Test {
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

TEST_F(SharedContextsTest, CreateSharedContext) {
	auto& windowMgr = app->getWindowManager();

	// Crear ventana principal
	pgrender::IWindow::Desc config;
	config.width = 800;
	config.height = 600;
	config.renderBackend = pgrender::RenderBackend::OpenGL4;

	auto window1Id = windowMgr.createWindow(config);
	auto* window1 = windowMgr.getWindow(window1Id);

	// Crear contexto principal
	auto context1 = window1->createContext(pgrender::GLContextDescriptor{});
	ASSERT_NE(context1, nullptr);

	windowMgr.setWindowContext(window1Id, std::move(context1));

	// Crear ventana secundaria
	auto window2Id = windowMgr.createWindow(config);
	auto* window2 = windowMgr.getWindow(window2Id);

	// Crear contexto compartido
	pgrender::GLContextDescriptor sharedConfig;
	sharedConfig.shareContext = windowMgr.getWindowContext(window1Id);

	auto context2 = window2->createContext(sharedConfig);
	ASSERT_NE(context2, nullptr);

	EXPECT_TRUE(context2->isShared());
	EXPECT_EQ(context2->getSharedContext(), windowMgr.getWindowContext(window1Id));

	windowMgr.setWindowContext(window2Id, std::move(context2));

	windowMgr.destroyWindow(window1Id);
	windowMgr.destroyWindow(window2Id);
}

TEST_F(SharedContextsTest, HeadlessSharedContext) {
	auto& windowMgr = app->getWindowManager();

	// Crear ventana con contexto
	pgrender::IWindow::Desc config;
	config.width = 800;
	config.height = 600;
	config.renderBackend = pgrender::RenderBackend::OpenGL4;

	auto windowId = windowMgr.createWindow(config);
	auto* window = windowMgr.getWindow(windowId);

	auto mainContext = window->createContext(pgrender::GLContextDescriptor{});
	windowMgr.setWindowContext(windowId, std::move(mainContext));

	// Crear contexto headless compartido
	pgrender::GLContextDescriptor headlessConfig;
	headlessConfig.shareContext = windowMgr.getWindowContext(windowId);

	auto headlessContext = app->createHeadlessContext(headlessConfig);

	ASSERT_NE(headlessContext, nullptr);
	EXPECT_TRUE(headlessContext->isShared());

	windowMgr.destroyWindow(windowId);
}
