#include <gtest/gtest.h>
#include <pgrender/app.h>
#include <pgrender/windowManager.h>
#include <appFactory.h>

class ContextCreationTest : public ::testing::Test {
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

TEST_F(ContextCreationTest, CreateWindowContext) {
	auto& windowMgr = app->getWindowManager();

	pgrender::IWindow::Desc config;
	config.width = 800;
	config.height = 600;
	config.renderBackend = pgrender::RenderBackend::OpenGL4;

	auto windowId = windowMgr.createWindow(config);
	auto* window = windowMgr.getWindow(windowId);
	ASSERT_NE(window, nullptr);

	pgrender::GLContextDescriptor ctxConfig;

	auto gfxContext = window->createContext(ctxConfig);
	ASSERT_NE(gfxContext, nullptr);

	EXPECT_EQ(gfxContext->getBackend(), pgrender::RenderBackend::OpenGL4);
	EXPECT_FALSE(gfxContext->isShared());
	EXPECT_EQ(gfxContext->getSharedContext(), nullptr);

	windowMgr.setWindowContext(windowId, std::move(gfxContext));

	auto* ctx = windowMgr.getWindowContext(windowId);
	EXPECT_NE(ctx, nullptr);

	windowMgr.destroyWindow(windowId);
}

TEST_F(ContextCreationTest, CreateHeadlessContext) {
	pgrender::GLContextDescriptor config;

	auto headlessContext = app->createHeadlessContext(config);

	ASSERT_NE(headlessContext, nullptr);
	EXPECT_EQ(headlessContext->getBackend(), pgrender::RenderBackend::OpenGL4);
	EXPECT_FALSE(headlessContext->isShared());
}

TEST_F(ContextCreationTest, MakeContextCurrent) {
	auto& windowMgr = app->getWindowManager();

	pgrender::IWindow::Desc config;
	config.width = 800;
	config.height = 600;
	config.renderBackend = pgrender::RenderBackend::OpenGL4;

	auto windowId = windowMgr.createWindow(config);
	auto* window = windowMgr.getWindow(windowId);

	pgrender::GLContextDescriptor ctxConfig;
	auto gfxContext = window->createContext(ctxConfig);
	ASSERT_NE(gfxContext, nullptr);

	EXPECT_NO_THROW(gfxContext->makeCurrent());

	windowMgr.setWindowContext(windowId, std::move(gfxContext));
	windowMgr.destroyWindow(windowId);
}
