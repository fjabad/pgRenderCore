#include <gtest/gtest.h>
#include <pgrender/types.h>

TEST(WindowConfigTest, BasicConfiguration) {
	pgrender::WindowConfig config;
	config.title = "Test Window";
	config.width = 1920;
	config.height = 1080;
	config.resizable = true;
	config.fullscreen = false;
	config.vsync = true;

	EXPECT_EQ(config.title, "Test Window");
	EXPECT_EQ(config.width, 1920u);
	EXPECT_EQ(config.height, 1080u);
	EXPECT_TRUE(config.resizable);
	EXPECT_FALSE(config.fullscreen);
	EXPECT_TRUE(config.vsync);
}

TEST(WindowConfigTest, RenderBackendSelection) {
	pgrender::WindowConfig config;

	config.renderBackend = pgrender::RenderBackend::OpenGL4;
	EXPECT_EQ(config.renderBackend, pgrender::RenderBackend::OpenGL4);

	config.renderBackend = pgrender::RenderBackend::Vulkan;
	EXPECT_EQ(config.renderBackend, pgrender::RenderBackend::Vulkan);

	config.renderBackend = pgrender::RenderBackend::Metal;
	EXPECT_EQ(config.renderBackend, pgrender::RenderBackend::Metal);
}

TEST(ContextConfigTest, OpenGLConfiguration) {
	pgrender::ContextConfig config;
	config.backend = pgrender::RenderBackend::OpenGL4;
	config.majorVersion = 4;
	config.minorVersion = 6;
	config.debugContext = true;

	EXPECT_EQ(config.backend, pgrender::RenderBackend::OpenGL4);
	EXPECT_EQ(config.majorVersion, 4);
	EXPECT_EQ(config.minorVersion, 6);
	EXPECT_TRUE(config.debugContext);
}
