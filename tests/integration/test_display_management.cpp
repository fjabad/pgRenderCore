#include <gtest/gtest.h>
#include <pgrender/renderCore.h>
#include <renderCoreFactory.h>

class DisplayManagementTest : public ::testing::Test {
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

TEST_F(DisplayManagementTest, GetDisplayCount) {
	auto& windowMgr = context->getWindowManager();

	int displayCount = windowMgr.getDisplayCount();

	EXPECT_GT(displayCount, 0); // Debe haber al menos 1 display
}

TEST_F(DisplayManagementTest, GetDisplayInfo) {
	auto& windowMgr = context->getWindowManager();

	int displayCount = windowMgr.getDisplayCount();
	ASSERT_GT(displayCount, 0);

	for (int i = 0; i < displayCount; ++i) {
		auto info = windowMgr.getDisplayInfo(i);

		EXPECT_EQ(info.index, i);
		//EXPECT_FALSE(info.name.empty());
		EXPECT_GT(info.bounds.width, 0);
		EXPECT_GT(info.bounds.height, 0);
		EXPECT_GE(info.refreshRate, 0);
		EXPECT_GT(info.ddpi, 0.0f);
	}
}

TEST_F(DisplayManagementTest, GetPrimaryDisplayInfo) {
	auto& windowMgr = context->getWindowManager();

	auto primaryDisplay = windowMgr.getDisplayInfo(0);

	EXPECT_EQ(primaryDisplay.index, 0);
	//EXPECT_FALSE(primaryDisplay.name.empty());
	EXPECT_GT(primaryDisplay.bounds.width, 0);
	EXPECT_GT(primaryDisplay.bounds.height, 0);
}

TEST_F(DisplayManagementTest, GetWindowDisplayIndex) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	int displayIndex = windowMgr.getWindowDisplayIndex(windowId);

	EXPECT_GE(displayIndex, 0);
	EXPECT_LT(displayIndex, windowMgr.getDisplayCount());

	windowMgr.destroyWindow(windowId);
}

TEST_F(DisplayManagementTest, CenterWindowOnDisplay) {
	auto& windowMgr = context->getWindowManager();

	pgrender::WindowConfig config;
	config.width = 800;
	config.height = 600;

	auto windowId = windowMgr.createWindow(config);

	int displayCount = windowMgr.getDisplayCount();
	ASSERT_GT(displayCount, 0);

	// Centrar en el primer display
	EXPECT_NO_THROW(windowMgr.centerWindowOnDisplay(windowId, 0));

	// Verificar que la posición cambió
	int x, y;
	windowMgr.getWindowPosition(windowId, x, y);

	auto displayInfo = windowMgr.getDisplayInfo(0);

	// La ventana debería estar dentro del área del display
	EXPECT_GE(x, displayInfo.bounds.x);
	EXPECT_GE(y, displayInfo.bounds.y);

	windowMgr.destroyWindow(windowId);
}
