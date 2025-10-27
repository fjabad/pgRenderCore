#include <gmock/gmock.h>
#include <pgrender/types.h>
#include <pgrender/window.h>

using namespace ::testing;
// ============================================================================
// GamepadInfo Tests
// ============================================================================

TEST(GamepadInfoTest, TypeNames) {
	pgrender::GamepadInfo info;

	info.type = pgrender::GamepadType::Xbox360;
	EXPECT_EQ(info.getTypeName(), "Xbox 360 Controller");

	info.type = pgrender::GamepadType::XboxOne;
	EXPECT_EQ(info.getTypeName(), "Xbox One/Series Controller");

	info.type = pgrender::GamepadType::PS4;
	EXPECT_EQ(info.getTypeName(), "PlayStation 4 Controller");

	info.type = pgrender::GamepadType::PS5;
	EXPECT_EQ(info.getTypeName(), "PlayStation 5 Controller");

	info.type = pgrender::GamepadType::NintendoSwitch;
	EXPECT_EQ(info.getTypeName(), "Nintendo Switch Controller");

	info.type = pgrender::GamepadType::Unknown;
	EXPECT_EQ(info.getTypeName(), "Unknown Controller");
}

TEST(GamepadInfoTest, XboxControllerDetection) {
	pgrender::GamepadInfo info;

	info.type = pgrender::GamepadType::Xbox360;
	EXPECT_TRUE(info.isXboxController());
	EXPECT_FALSE(info.isPlayStationController());
	EXPECT_FALSE(info.isNintendoController());

	info.type = pgrender::GamepadType::XboxOne;
	EXPECT_TRUE(info.isXboxController());
}

TEST(GamepadInfoTest, PlayStationControllerDetection) {
	pgrender::GamepadInfo info;

	info.type = pgrender::GamepadType::PS3;
	EXPECT_TRUE(info.isPlayStationController());
	EXPECT_FALSE(info.isXboxController());
	EXPECT_FALSE(info.isNintendoController());

	info.type = pgrender::GamepadType::PS4;
	EXPECT_TRUE(info.isPlayStationController());

	info.type = pgrender::GamepadType::PS5;
	EXPECT_TRUE(info.isPlayStationController());
}

TEST(GamepadInfoTest, NintendoControllerDetection) {
	pgrender::GamepadInfo info;

	info.type = pgrender::GamepadType::NintendoSwitch;
	EXPECT_TRUE(info.isNintendoController());
	EXPECT_FALSE(info.isXboxController());
	EXPECT_FALSE(info.isPlayStationController());
}

TEST(GamepadInfoTest, AdvancedFeatures) {
	pgrender::GamepadInfo info;

	// Sin características avanzadas
	info.hasSensors = false;
	info.hasRumbleTriggers = false;
	info.hasLED = false;
	info.hasTouchpad = false;
	EXPECT_FALSE(info.hasAdvancedFeatures());

	// Con sensores
	info.hasSensors = true;
	EXPECT_TRUE(info.hasAdvancedFeatures());

	// Con rumble en gatillos
	info.hasSensors = false;
	info.hasRumbleTriggers = true;
	EXPECT_TRUE(info.hasAdvancedFeatures());

	// Con LED y touchpad
	info.hasRumbleTriggers = false;
	info.hasLED = true;
	info.hasTouchpad = true;
	EXPECT_TRUE(info.hasAdvancedFeatures());
}

TEST(GamepadInfoTest, UniqueId) {
	pgrender::GamepadInfo info1;
	info1.vendor = 0x045E;
	info1.product = 0x02EA;
	info1.deviceIndex = 0;
	info1.serial = "ABC123";

	std::string id1 = info1.getUniqueId();
	EXPECT_EQ(id1, "045E:02EA:ABC123");

	pgrender::GamepadInfo info2;
	info2.vendor = 0x054C;
	info2.product = 0x0CE6;
	info2.deviceIndex = 1;
	info2.serial = "";

	std::string id2 = info2.getUniqueId();
	EXPECT_EQ(id2, "054C:0CE6:1");
}

TEST(GamepadInfoTest, IsValid) {
	pgrender::GamepadInfo info;

	info.deviceIndex = -1;
	info.name = "";
	EXPECT_FALSE(info.isValid());

	info.deviceIndex = 0;
	info.name = "";
	EXPECT_FALSE(info.isValid());

	info.deviceIndex = 0;
	info.name = "Xbox Controller";
	EXPECT_TRUE(info.isValid());
}

// ============================================================================
// DisplayInfo Tests
// ============================================================================

TEST(DisplayInfoTest, DefaultValues) {
	pgrender::DisplayInfo info{};

	EXPECT_EQ(info.index, 0);
	EXPECT_TRUE(info.name.empty());
	EXPECT_EQ(info.refreshRate, 0);
}

// ============================================================================
// IWindow::Desc Tests
// ============================================================================

TEST(WindowConfigTest, DefaultValues) {
	pgrender::IWindow::Desc config;

	EXPECT_THAT(config.title, Eq("PGRenderCore Window"));
	EXPECT_THAT(config.width, Eq(800u));
	EXPECT_THAT(config.height, Eq(600u));
	EXPECT_TRUE(config.resizable);
	EXPECT_FALSE(config.fullscreen);
	EXPECT_TRUE(config.vsync);
	EXPECT_FALSE(config.highDPI);
	EXPECT_FALSE(config.transparent);
	EXPECT_FALSE(config.alwaysOnTop);
	EXPECT_FALSE(config.borderless);

	EXPECT_THAT(config.renderBackend, Eq(pgrender::RenderBackend::Auto));
}

// ============================================================================
// Event Tests
// ============================================================================

TEST(EventTest, DefaultConstruction) {
	pgrender::Event event{};

	EXPECT_EQ(event.type, pgrender::EventType::Unknown);
	EXPECT_EQ(event.timestamp, 0u);
}
