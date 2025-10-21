#include <gtest/gtest.h>
#include <pgrender/renderCore.h>
#include <renderCoreFactory.h>
#include <thread>
#if PGRENDER_USE_GLFW
#include <glfwWindow.h>
#endif


class InputSystemTest : public ::testing::Test {
protected:
	void SetUp() override {
		context = pgrender::RenderCoreFactory::createContext();
		ASSERT_NE(context, nullptr);
#if PGRENDER_USE_GLFW
		// el sistema de entrada de GLFW necesita una ventana para ciertas operaciones
		auto& windowManager = context->getWindowManager();
		pgrender::WindowConfig config;
		config.width = config.height = 100;
		config.title = "InputSystemTest";
		windowID = windowManager.createWindow(config);
		window = static_cast<pgrender::backends::glfw::GLFWWindow *>(windowManager.getWindow(windowID));
		auto ctx = window->createContext(pgrender::ContextConfig{});
		ctx->makeCurrent();
#endif
	}

	void TearDown() override {
		#if PGRENDER_USE_GLFW
		auto& windowManager = context->getWindowManager();
		windowManager.destroyWindow(windowID);
		#endif
		context.reset();
	}

	std::unique_ptr<pgrender::ILibraryContext> context;
#if PGRENDER_USE_GLFW
	pgrender::WindowID windowID;
	pgrender::backends::glfw::GLFWWindow *window;
#endif
};

TEST_F(InputSystemTest, GetGamepadCount) {
	auto& inputSystem = context->getInputSystem();

	int count = inputSystem.getGamepadCount();

	// Puede ser 0 si no hay gamepads conectados
	EXPECT_GE(count, 0);
}

TEST_F(InputSystemTest, GetAvailableGamepads) {
	auto& inputSystem = context->getInputSystem();

	auto gamepads = inputSystem.getAvailableGamepads();

	EXPECT_EQ(gamepads.size(), static_cast<size_t>(inputSystem.getGamepadCount()));
}

TEST_F(InputSystemTest, ClipboardOperations) {
	auto& inputSystem = context->getInputSystem();

	std::string testText = "Hello, PGRenderCore!";

	EXPECT_NO_THROW(inputSystem.setClipboardText(testText));

	std::string retrieved = inputSystem.getClipboardText();
	EXPECT_EQ(retrieved, testText);

	EXPECT_TRUE(inputSystem.hasClipboardText());
}

TEST_F(InputSystemTest, CursorVisibility) {
	auto& inputSystem = context->getInputSystem();

	// Nota: El estado inicial puede variar
	bool initialState = inputSystem.isCursorVisible();

	(void)initialState; // Evitar advertencia de variable no usada

	inputSystem.showCursor(false);
	// Dar tiempo al sistema
	std::this_thread::sleep_for(std::chrono::milliseconds(50));

	inputSystem.showCursor(true);
	// Verificar que no crashea

	SUCCEED();
}

TEST_F(InputSystemTest, CaptureMouse) {
	auto& inputSystem = context->getInputSystem();

	EXPECT_NO_THROW(inputSystem.captureMouse(true));
	EXPECT_NO_THROW(inputSystem.captureMouse(false));
}
