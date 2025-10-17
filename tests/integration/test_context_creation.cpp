#include <gtest/gtest.h>
#include <pgrender/renderCore.h>
#include <pgrender/renderCoreFactory.h>

class ContextCreationTest : public ::testing::Test {
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

TEST_F(ContextCreationTest, CreateWindowContext) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;
	config.renderBackend = pgrender::RenderBackend::OpenGL4;

	auto windowId = windowMgr.createWindow(config);
	auto* window = windowMgr.getWindow(windowId);
	ASSERT_NE(window, nullptr);

	pgrender::ContextConfig ctxConfig;
	ctxConfig.backend = pgrender::RenderBackend::OpenGL4;

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
	pgrender::ContextConfig config;
	config.backend = pgrender::RenderBackend::OpenGL4;

	auto headlessContext = context->createHeadlessContext(config);

	ASSERT_NE(headlessContext, nullptr);
	EXPECT_EQ(headlessContext->getBackend(), pgrender::RenderBackend::OpenGL4);
	EXPECT_FALSE(headlessContext->isShared());
}

TEST_F(ContextCreationTest, MakeContextCurrent) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;
	config.renderBackend = pgrender::RenderBackend::OpenGL4;

	auto windowId = windowMgr.createWindow(config);
	auto* window = windowMgr.getWindow(windowId);

	auto gfxContext = window->createContext(pgrender::RenderBackend::OpenGL4);
	ASSERT_NE(gfxContext, nullptr);

	EXPECT_NO_THROW(gfxContext->makeCurrent());

	windowMgr.setWindowContext(windowId, std::move(gfxContext));
	windowMgr.destroyWindow(windowId);
}
