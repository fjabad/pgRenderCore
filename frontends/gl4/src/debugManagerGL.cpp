#include "PGRenderCoreGL/debugManagerGL.h"
#include <GL/glew.h>
#include <iostream>
#include <sstream>
#include <iomanip>

namespace pgrender {

	DebugManagerGL::DebugManagerGL()
		: m_callback(defaultCallback)
	{
	}

	DebugManagerGL::~DebugManagerGL() {
		shutdown();
	}

	void DebugManagerGL::shutdown() {
		if (!m_enabled) return;

		glDisable(GL_DEBUG_OUTPUT);
		glDisable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		if (GLEW_KHR_debug) {
			glDebugMessageCallback(nullptr, nullptr);
		}
		else if (GLEW_ARB_debug_output) {
			glDebugMessageCallbackARB(nullptr, nullptr);
		}

		m_enabled = false;

		std::cout << "OpenGL Debug Manager shutdown" << std::endl;
		std::cout << "Debug Statistics:" << std::endl;
		std::cout << "  Total messages: " << m_statistics.totalMessages << std::endl;
		std::cout << "  Errors: " << m_statistics.errors << std::endl;
		std::cout << "  Warnings: " << m_statistics.warnings << std::endl;
		std::cout << "  Performance: " << m_statistics.performance << std::endl;
		std::cout << "  Other: " << m_statistics.other << std::endl;
	}

	void DebugManagerGL::setCallback(const DebugCallback& callback) {
		m_callback = callback;
	}

	void DebugManagerGL::clearCallback() {
		m_callback = defaultCallback;
	}

	void DebugManagerGL::pushDebugGroup(const std::string& name) {
		if (!m_enabled) return;

		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, 0,
			static_cast<GLsizei>(name.length()), name.c_str());
		m_groupDepth++;
	}

	void DebugManagerGL::popDebugGroup() {
		if (!m_enabled || m_groupDepth == 0) return;

		glPopDebugGroup();
		m_groupDepth--;
	}

	bool DebugManagerGL::checkGLError(const char* context) {
		GLenum error = glGetError();
		if (error == GL_NO_ERROR) {
			return false;
		}

		bool hasError = false;
		while (error != GL_NO_ERROR) {
			hasError = true;

			std::ostringstream oss;
			if (context) {
				oss << context << ": ";
			}

			switch (error) {
			case GL_INVALID_ENUM:
				oss << "GL_INVALID_ENUM - An unacceptable value is specified for an enumerated argument";
				break;
			case GL_INVALID_VALUE:
				oss << "GL_INVALID_VALUE - A numeric argument is out of range";
				break;
			case GL_INVALID_OPERATION:
				oss << "GL_INVALID_OPERATION - The specified operation is not allowed in the current state";
				break;
			case GL_STACK_OVERFLOW:
				oss << "GL_STACK_OVERFLOW - Command would cause a stack overflow";
				break;
			case GL_STACK_UNDERFLOW:
				oss << "GL_STACK_UNDERFLOW - Command would cause a stack underflow";
				break;
			case GL_OUT_OF_MEMORY:
				oss << "GL_OUT_OF_MEMORY - Not enough memory left to execute the command";
				break;
			case GL_INVALID_FRAMEBUFFER_OPERATION:
				oss << "GL_INVALID_FRAMEBUFFER_OPERATION - Framebuffer object is not complete";
				break;
			default:
				oss << "Unknown OpenGL error: 0x" << std::hex << error;
				break;
			}

			DebugMessage msg;
			msg.source = DebugSource::API;
			msg.type = DebugType::Error;
			msg.severity = DebugSeverity::High;
			msg.id = error;
			msg.message = oss.str();

			if (m_callback) {
				m_callback(msg);
			}

			m_statistics.totalMessages++;
			m_statistics.errors++;

			error = glGetError();
		}

		return hasError;
	}


	// Conversiones de enums GL a nuestros enums
	static DebugSource convertSource(unsigned int glSource);
	static DebugType convertType(unsigned int glType);
	static DebugSeverity convertSeverity(unsigned int glSeverity);

	// Conversiones inversas
	static unsigned int toGLSource(DebugSource source);
	static unsigned int toGLType(DebugType type);
	static unsigned int toGLSeverity(DebugSeverity severity);

	// Colores ANSI para consola (opcional)
	static const char* getSeverityColor(DebugSeverity severity);
	static const char* getResetColor();

	void DebugManagerGL::setTypeEnabled(DebugType type, bool enabled) {
		if (!m_enabled) return;

		unsigned int glType = toGLType(type);

		if (enabled) {
			glDebugMessageControl(GL_DONT_CARE, glType, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
		else {
			glDebugMessageControl(GL_DONT_CARE, glType, GL_DONT_CARE, 0, nullptr, GL_FALSE);
		}
	}

	void DebugManagerGL::setSeverityEnabled(DebugSeverity severity, bool enabled) {
		if (!m_enabled) return;

		unsigned int glSeverity = toGLSeverity(severity);

		if (enabled) {
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, glSeverity, 0, nullptr, GL_TRUE);
		}
		else {
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, glSeverity, 0, nullptr, GL_FALSE);
		}
	}

	void DebugManagerGL::setSourceEnabled(DebugSource source, bool enabled) {
		if (!m_enabled) return;

		unsigned int glSource = toGLSource(source);

		if (enabled) {
			glDebugMessageControl(glSource, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}
		else {
			glDebugMessageControl(glSource, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_FALSE);
		}
	}

	void DebugManagerGL::insertMessage(DebugType type, DebugSeverity severity, const std::string& message) {
		if (!m_enabled) return;

		glDebugMessageInsert(
			toGLSource(DebugSource::Application),
			toGLType(type),
			0, // ID
			toGLSeverity(severity),
			static_cast<GLsizei>(message.length()),
			message.c_str()
		);
	}


#ifdef _WIN32
#undef GLAPIENTRY
#define GLAPIENTRY __stdcall
#endif

	void GLAPIENTRY debugCallbackStatic(
		unsigned int source,
		unsigned int type,
		unsigned int id,
		unsigned int severity,
		int length,
		const char* message,
		const void* userParam)
	{
		// Filtrar algunos mensajes muy comunes y no útiles
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204) {
			return; // Mensajes de buffer usage hint (NVIDIA)
		}

		auto* manager = static_cast<const DebugManagerGL*>(userParam);
		if (!manager || !manager->m_callback) {
			return;
		}

		DebugMessage msg;
		msg.source = convertSource(source);
		msg.type = convertType(type);
		msg.severity = convertSeverity(severity);
		msg.id = id;
		msg.message = std::string(message, length > 0 ? length : strlen(message));

		// Actualizar estadísticas (const_cast es seguro aquí)
		auto* mutableManager = const_cast<DebugManagerGL*>(manager);
		mutableManager->m_statistics.totalMessages++;

		switch (msg.type) {
		case DebugType::Error:
			mutableManager->m_statistics.errors++;
			break;
		case DebugType::DeprecatedBehavior:
		case DebugType::UndefinedBehavior:
		case DebugType::Portability:
			mutableManager->m_statistics.warnings++;
			break;
		case DebugType::Performance:
			mutableManager->m_statistics.performance++;
			break;
		default:
			mutableManager->m_statistics.other++;
			break;
		}

		manager->m_callback(msg);
	}



	bool DebugManagerGL::initialize(bool enableSynchronous) {
		m_synchronous = enableSynchronous;

		// Registrar callback
		glDebugMessageCallback(debugCallbackStatic, this);

		// Por defecto, deshabilitar notificaciones de bajo nivel (muy verbosas)
		setSeverityEnabled(DebugSeverity::Notification, false);

		// Habilitar debug output
		glEnable(GL_DEBUG_OUTPUT);

		if (m_synchronous) {
			// Debug síncrono: los mensajes se generan inmediatamente
			// Más lento pero permite breakpoints precisos
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		}

		m_enabled = true;

		std::cout << "OpenGL Debug Manager initialized (synchronous: "
			<< (m_synchronous ? "yes" : "no") << ")" << std::endl;

		// Insertar mensaje de prueba
		insertMessage(DebugType::Marker, DebugSeverity::Notification,
			"Debug Manager initialized successfully");

		return true;
	}


	// ===== CONVERSIONES =====

	static DebugSource convertSource(unsigned int glSource) {
		switch (glSource) {
		case GL_DEBUG_SOURCE_API: return DebugSource::API;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return DebugSource::WindowSystem;
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return DebugSource::ShaderCompiler;
		case GL_DEBUG_SOURCE_THIRD_PARTY: return DebugSource::ThirdParty;
		case GL_DEBUG_SOURCE_APPLICATION: return DebugSource::Application;
		default: return DebugSource::Other;
		}
	}

	static DebugType convertType(unsigned int glType) {
		switch (glType) {
		case GL_DEBUG_TYPE_ERROR: return DebugType::Error;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return DebugType::DeprecatedBehavior;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return DebugType::UndefinedBehavior;
		case GL_DEBUG_TYPE_PORTABILITY: return DebugType::Portability;
		case GL_DEBUG_TYPE_PERFORMANCE: return DebugType::Performance;
		case GL_DEBUG_TYPE_MARKER: return DebugType::Marker;
		case GL_DEBUG_TYPE_PUSH_GROUP: return DebugType::PushGroup;
		case GL_DEBUG_TYPE_POP_GROUP: return DebugType::PopGroup;
		default: return DebugType::Other;
		}
	}

	static DebugSeverity convertSeverity(unsigned int glSeverity) {
		switch (glSeverity) {
		case GL_DEBUG_SEVERITY_HIGH: return DebugSeverity::High;
		case GL_DEBUG_SEVERITY_MEDIUM: return DebugSeverity::Medium;
		case GL_DEBUG_SEVERITY_LOW: return DebugSeverity::Low;
		case GL_DEBUG_SEVERITY_NOTIFICATION: return DebugSeverity::Notification;
		default: return DebugSeverity::Notification;
		}
	}

	static unsigned int toGLSource(DebugSource source) {
		switch (source) {
		case DebugSource::API: return GL_DEBUG_SOURCE_API;
		case DebugSource::WindowSystem: return GL_DEBUG_SOURCE_WINDOW_SYSTEM;
		case DebugSource::ShaderCompiler: return GL_DEBUG_SOURCE_SHADER_COMPILER;
		case DebugSource::ThirdParty: return GL_DEBUG_SOURCE_THIRD_PARTY;
		case DebugSource::Application: return GL_DEBUG_SOURCE_APPLICATION;
		default: return GL_DEBUG_SOURCE_OTHER;
		}
	}

	static unsigned int toGLType(DebugType type) {
		switch (type) {
		case DebugType::Error: return GL_DEBUG_TYPE_ERROR;
		case DebugType::DeprecatedBehavior: return GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR;
		case DebugType::UndefinedBehavior: return GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR;
		case DebugType::Portability: return GL_DEBUG_TYPE_PORTABILITY;
		case DebugType::Performance: return GL_DEBUG_TYPE_PERFORMANCE;
		case DebugType::Marker: return GL_DEBUG_TYPE_MARKER;
		case DebugType::PushGroup: return GL_DEBUG_TYPE_PUSH_GROUP;
		case DebugType::PopGroup: return GL_DEBUG_TYPE_POP_GROUP;
		default: return GL_DEBUG_TYPE_OTHER;
		}
	}

	static unsigned int toGLSeverity(DebugSeverity severity) {
		switch (severity) {
		case DebugSeverity::High: return GL_DEBUG_SEVERITY_HIGH;
		case DebugSeverity::Medium: return GL_DEBUG_SEVERITY_MEDIUM;
		case DebugSeverity::Low: return GL_DEBUG_SEVERITY_LOW;
		case DebugSeverity::Notification: return GL_DEBUG_SEVERITY_NOTIFICATION;
		default: return GL_DEBUG_SEVERITY_NOTIFICATION;
		}
	}

	// ===== CALLBACK POR DEFECTO =====

	void DebugManagerGL::defaultCallback(const DebugMessage& msg) {
		const char* color = getSeverityColor(msg.severity);
		const char* reset = getResetColor();

		std::ostream& out = (msg.severity == DebugSeverity::High) ? std::cerr : std::cout;

		out << color << "[OpenGL Debug] ";

		// Fuente
		out << "[";
		switch (msg.source) {
		case DebugSource::API: out << "API"; break;
		case DebugSource::WindowSystem: out << "Window"; break;
		case DebugSource::ShaderCompiler: out << "Shader"; break;
		case DebugSource::ThirdParty: out << "ThirdParty"; break;
		case DebugSource::Application: out << "App"; break;
		default: out << "Other"; break;
		}
		out << "] ";

		// Tipo
		out << "[";
		switch (msg.type) {
		case DebugType::Error: out << "ERROR"; break;
		case DebugType::DeprecatedBehavior: out << "Deprecated"; break;
		case DebugType::UndefinedBehavior: out << "Undefined"; break;
		case DebugType::Portability: out << "Portability"; break;
		case DebugType::Performance: out << "Performance"; break;
		case DebugType::Marker: out << "Marker"; break;
		default: out << "Other"; break;
		}
		out << "] ";

		// Severidad
		switch (msg.severity) {
		case DebugSeverity::High: out << "[HIGH] "; break;
		case DebugSeverity::Medium: out << "[MEDIUM] "; break;
		case DebugSeverity::Low: out << "[LOW] "; break;
		case DebugSeverity::Notification: out << "[INFO] "; break;
		}

		// Mensaje
		out << msg.message << reset << std::endl;
	}

	static const char* getSeverityColor(DebugSeverity severity) {
#ifdef _WIN32
		return ""; // Windows console no soporta ANSI por defecto
#else
		switch (severity) {
		case DebugSeverity::High: return "\033[1;31m"; // Rojo bold
		case DebugSeverity::Medium: return "\033[1;33m"; // Amarillo bold
		case DebugSeverity::Low: return "\033[1;37m"; // Blanco bold
		case DebugSeverity::Notification: return "\033[0;36m"; // Cyan
		default: return "";
		}
#endif
	}

	static const char* getResetColor() {
#ifdef _WIN32
		return "";
#else
		return "\033[0m";
#endif
	}

} // namespace pgrender
