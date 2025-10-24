#pragma once
#include <memory>
#include "types.h"

namespace pgrender {
	class IGraphicsContext;
	struct ContextConfig;
	class IWindowManager;
	class IInputSystem;
	/// Contexto de aplicación
	class App {
	public:
		virtual ~App() = default;

		/// Crea un contexto gráfico sin ventana (headless)
		/// @param config Configuración del contexto
		/// @return Contexto gráfico headless
		virtual std::unique_ptr<IGraphicsContext> createHeadlessContext(const ContextConfig& config) = 0;

		/// Sobrecarga de conveniencia para crear contexto headless
		std::unique_ptr<IGraphicsContext> createHeadlessContext(RenderBackend backend) {
			ContextConfig config;
			config.backend = backend;
			return createHeadlessContext(config);
		}

		/// Obtiene el gestor de ventanas
		/// @return Referencia al gestor de ventanas
		virtual IWindowManager& getWindowManager() = 0;

		/// Obtiene el sistema de entrada avanzado
		/// @return Referencia al sistema de entrada
		virtual IInputSystem& getInputSystem() = 0;

		App(const App&) = delete;
		App& operator=(const App&) = delete;
		App(App&&) = delete;
		App& operator=(App&&) = delete;

	protected:
		App() = default;
	};
} // namespace pgrender