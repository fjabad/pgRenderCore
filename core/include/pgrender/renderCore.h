#pragma once
#include "graphicsContext.h"
#include "window.h"
#include "eventSystem.h"
#include "windowManager.h"
#include "inputSystem.h"
#include "types.h"
#include <memory>

namespace pgrender {

	/// Contexto de librería base
	class ILibraryContext {
	public:
		virtual ~ILibraryContext() = default;

		/// Crea una nueva ventana
		/// @param config Configuración de la ventana
		/// @return Ventana creada
		virtual std::unique_ptr<IWindow> createWindow(const WindowConfig& config) = 0;

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

		/// Obtiene el sistema de eventos global
		/// @return Referencia al sistema de eventos
		virtual IEventSystem& getEventSystem() = 0;

		/// Obtiene el gestor de ventanas
		/// @return Referencia al gestor de ventanas
		virtual IWindowManager& getWindowManager() = 0;

		/// Obtiene el sistema de entrada avanzado
		/// @return Referencia al sistema de entrada
		virtual IAdvancedInputSystem& getInputSystem() = 0;

		ILibraryContext(const ILibraryContext&) = delete;
		ILibraryContext& operator=(const ILibraryContext&) = delete;
		ILibraryContext(ILibraryContext&&) = delete;
		ILibraryContext& operator=(ILibraryContext&&) = delete;

	protected:
		ILibraryContext() = default;
	};

	/// Factory principal para crear contextos de renderizado
	class RenderCoreFactory {
	public:
		/// Crea un contexto de librería con gestor de ventanas integrado
		/// @param backend Backend de ventanas a usar (Auto selecciona automáticamente)
		/// @return Contexto de librería creado
		static std::unique_ptr<ILibraryContext> createContext(WindowBackend backend = WindowBackend::Auto);

	private:
		RenderCoreFactory() = delete;
	};

} // namespace pgrender
