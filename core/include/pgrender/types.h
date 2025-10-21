
#pragma once
#include <cstdint>
#include <string>

namespace pgrender {

// Backend de renderizado
enum class RenderBackend {
    OpenGL4,      ///< OpenGL 4.x
    Vulkan,       ///< Vulkan API
    Metal,        ///< Metal (macOS/iOS)
    DirectX12,    ///< DirectX 12
    WebGPU,       ///< WebGPU
    Auto          ///< Selección automática según plataforma
};

// Backend de ventanas
enum class WindowBackend {
    SDL3,
    GLFW,
    Auto
};

// ID único para ventanas
using WindowID = std::uintptr_t;

// Teclas
enum class KeyCode {
    Unknown = 0,
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    Escape, Space, Enter, Tab, Backspace,
    Left, Right, Up, Down,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
};

// Botones del ratón
enum class MouseButton {
    Left,
    Right,
    Middle,
    X1,
    X2
};

// Tipos de eventos
enum class EventType {
	Unknown,
    WindowClose,
    WindowResize,
    WindowFocus,
    WindowLostFocus,
    WindowMoved,
    WindowMinimized,
    WindowMaximized,
    WindowRestored,
    KeyPress,
    KeyRelease,
    MouseMove,
    MouseButtonPress,
    MouseButtonRelease,
    MouseScroll,
    GamepadConnected,
    GamepadDisconnected
};

// Configuración de ventana
struct WindowConfig {
    std::string title = "PGRenderCore Window";
    uint32_t width = 800;
    uint32_t height = 600;
    bool resizable = true;
    bool fullscreen = false;
    bool vsync = true;
    bool highDPI = false;
    bool transparent = false;
    bool alwaysOnTop = false;
    bool borderless = false;
    RenderBackend renderBackend = RenderBackend::Auto;
};

// Configuración de contexto gráfico
struct ContextConfig {
    RenderBackend backend = RenderBackend::Auto;
    void* shareContext = nullptr;  // IGraphicsContext* para compartir recursos
    bool debugContext = false;
    int majorVersion = 4;
    int minorVersion = 6;
};

// Evento genérico
struct Event {
    EventType type = EventType::Unknown;
    uint64_t timestamp = 0;
    
    struct KeyEvent {
        KeyCode key;
        bool repeat;
        uint16_t mod;
    };
    
    struct MouseMoveEvent {
        float x, y;
        float deltaX, deltaY;
    };
    
    struct MouseButtonEvent {
        MouseButton button;
        float x, y;
        uint8_t clicks;
    };
    
    struct MouseScrollEvent {
        float deltaX, deltaY;
        bool flipped;
    };
    
    struct WindowResizeEvent {
        uint32_t width, height;
    };
    
    struct WindowMovedEvent {
        int32_t x, y;
    };
    
    union {
        KeyEvent key;
        MouseMoveEvent mouseMove;
        MouseButtonEvent mouseButton;
        MouseScrollEvent mouseScroll;
        WindowResizeEvent windowResize;
        WindowMovedEvent windowMoved;
    };
};

// Información de display/monitor
struct DisplayInfo {
	struct Bounds {
		int x, y;
		int width, height;
	};
	
	int index;
    std::string name;
    Bounds bounds;
    Bounds usableBounds;
    float ddpi, hdpi, vdpi;
    int refreshRate;
};

/// Tipo de gamepad (útil para adaptar UI, iconos, etc.)
enum class GamepadType {
	Unknown,            ///< Gamepad desconocido o genérico
	Xbox360,            ///< Xbox 360 Controller
	XboxOne,            ///< Xbox One Controller / Xbox Series X|S
	PS3,                ///< PlayStation 3 DualShock 3
	PS4,                ///< PlayStation 4 DualShock 4
	PS5,                ///< PlayStation 5 DualSense
	NintendoSwitch     ///< Nintendo Switch Pro Controller / Joy-Con
};

/// Información completa sobre un gamepad conectado
struct GamepadInfo {
	/// Índice del dispositivo (ID interno del sistema)
	/// Valor: >= 0 si está disponible, -1 si no se pudo obtener
	int deviceIndex = -1;

	/// Nombre del gamepad proporcionado por el driver
	/// Ejemplo: "Xbox Wireless Controller", "DualSense Wireless Controller"
	std::string name;

	/// ID del fabricante (Vendor ID USB)
	/// Ejemplos: 0x045E (Microsoft), 0x054C (Sony), 0x057E (Nintendo)
	uint16_t vendor = 0;

	/// ID del producto (Product ID USB)
	/// Identifica el modelo específico del gamepad
	uint16_t product = 0;

	/// Número de serie del dispositivo (si está disponible)
	/// Útil para distinguir entre múltiples gamepads del mismo modelo
	std::string serial;

	/// Índice del jugador asignado por el sistema
	/// Rango: 0-3 típicamente, -1 si no está asignado
	/// Nota: Algunos gamepads (PS4, PS5, Switch) muestran este índice con LEDs
	int playerIndex = -1;

	// ========================================================================
	// Capacidades del gamepad
	// ========================================================================

	/// Soporta vibración/rumble estándar
	/// Presente en la mayoría de gamepads modernos (motores izquierdo/derecho)
	bool hasRumble = false;

	/// Soporta vibración en los gatillos (trigger rumble)
	/// Disponible en: Xbox One Elite, Xbox Series X|S, DualSense (gatillos adaptativos)
	bool hasRumbleTriggers = false;

	/// Tiene LED controlable
	/// Puede ser RGB (PS4/PS5) o LEDs de jugador (Switch Pro, PS3)
	/// Tipos de LED:
	/// - RGB: Barra de luz completa (PS4), anillo LED (PS5)
	/// - Player: LEDs numerados 1-4 (PS3, Switch Pro)
	/// - Mono: LED simple on/off
	bool hasLED = false;

	/// Tiene touchpad/trackpad
	/// Disponible en: PS4 DualShock 4, PS5 DualSense
	/// Permite entrada táctil multi-touch
	bool hasTouchpad = false;

	/// Tiene sensores de movimiento (giroscopio y/o acelerómetro)
	/// Disponible en: PS3, PS4, PS5, Switch Pro, Joy-Con
	/// Útil para controles de movimiento y aim assist
	bool hasSensors = false;

	// ========================================================================
	// Información del tipo de gamepad
	// ========================================================================

	/// Tipo de gamepad detectado
	/// Útil para mostrar iconos apropiados (botones A/B/X/Y vs Cruz/Círculo)
	GamepadType type = GamepadType::Unknown;

	// ========================================================================
	// Métodos de utilidad
	// ========================================================================

	/// Obtiene una descripción legible del tipo de gamepad
	/// @return String descriptivo del tipo (ej: "Xbox One Controller")
	std::string getTypeName() const {
		switch (type) {
		case GamepadType::Xbox360:        return "Xbox 360 Controller";
		case GamepadType::XboxOne:        return "Xbox One/Series Controller";
		case GamepadType::PS3:            return "PlayStation 3 Controller";
		case GamepadType::PS4:            return "PlayStation 4 Controller";
		case GamepadType::PS5:            return "PlayStation 5 Controller";
		case GamepadType::NintendoSwitch: return "Nintendo Switch Controller";
		default:                          return "Unknown Controller";
		}
	}

	/// Verifica si el gamepad es de la familia Xbox
	/// @return true si es Xbox 360, Xbox One o Xbox Series
	bool isXboxController() const {
		return type == GamepadType::Xbox360 || type == GamepadType::XboxOne;
	}

	/// Verifica si el gamepad es de la familia PlayStation
	/// @return true si es PS3, PS4 o PS5
	bool isPlayStationController() const {
		return type == GamepadType::PS3 ||
			type == GamepadType::PS4 ||
			type == GamepadType::PS5;
	}

	/// Verifica si el gamepad es de Nintendo
	/// @return true si es Switch Pro o Joy-Con
	bool isNintendoController() const {
		return type == GamepadType::NintendoSwitch;
	}

	/// Verifica si el gamepad tiene características avanzadas
	/// @return true si tiene sensores, LED RGB, o rumble en gatillos
	bool hasAdvancedFeatures() const {
		return hasSensors || hasRumbleTriggers || (hasLED && hasTouchpad);
	}

	/// Obtiene un identificador único del gamepad
	/// @return String único basado en vendor, product y serial
	std::string getUniqueId() const {
		char buffer[128];
		if (!serial.empty()) {
			snprintf(buffer, sizeof(buffer), "%04X:%04X:%s", vendor, product, serial.c_str());
		}
		else {
			snprintf(buffer, sizeof(buffer), "%04X:%04X:%d", vendor, product, deviceIndex);
		}
		return std::string(buffer);
	}

	/// Verifica si el gamepad tiene capacidades mínimas básicas
	/// @return true si al menos tiene botones y ejes (gamepad funcional)
	bool isValid() const {
		return deviceIndex >= 0 && !name.empty();
	}
};

// ============================================================================
// Gamepad Buttons (ya definidos previamente, aquí documentados)
// ============================================================================

/// Botones estándar de un gamepad
/// Usa nomenclatura Xbox/universal como base
enum class GamepadButton {
	A,              ///< Botón A (Xbox) / Cruz (PlayStation) / B (Nintendo)
	B,              ///< Botón B (Xbox) / Círculo (PlayStation) / A (Nintendo)
	X,              ///< Botón X (Xbox) / Cuadrado (PlayStation) / Y (Nintendo)
	Y,              ///< Botón Y (Xbox) / Triángulo (PlayStation) / X (Nintendo)
	Back,           ///< Botón Back/View/Select/Share
	Guide,          ///< Botón Guide/Home/PS/Xbox
	Start,          ///< Botón Start/Menu/Options
	LeftStick,      ///< Presión del stick analógico izquierdo (L3)
	RightStick,     ///< Presión del stick analógico derecho (R3)
	LeftShoulder,   ///< Botón de hombro izquierdo (LB/L1)
	RightShoulder,  ///< Botón de hombro derecho (RB/R1)
	DPadUp,         ///< D-Pad arriba
	DPadDown,       ///< D-Pad abajo
	DPadLeft,       ///< D-Pad izquierda
	DPadRight       ///< D-Pad derecha
};

/// Ejes analógicos de un gamepad
enum class GamepadAxis {
	LeftX,          ///< Eje X del stick izquierdo (-1.0 izq, +1.0 der)
	LeftY,          ///< Eje Y del stick izquierdo (-1.0 arriba, +1.0 abajo)
	RightX,         ///< Eje X del stick derecho (-1.0 izq, +1.0 der)
	RightY,         ///< Eje Y del stick derecho (-1.0 arriba, +1.0 abajo)
	TriggerLeft,    ///< Gatillo izquierdo (0.0 sin presionar, +1.0 totalmente presionado)
	TriggerRight    ///< Gatillo derecho (0.0 sin presionar, +1.0 totalmente presionado)
};

/// Tipos de sensores disponibles en gamepads
enum class SensorType {
	Accelerometer,  ///< Acelerómetro (detecta aceleración lineal)
	Gyroscope       ///< Giroscopio (detecta rotación angular)
};


} // namespace pgrender
