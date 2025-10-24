#pragma once
#include "pgrender/types.h"
#include <memory>

namespace pgrender
{
	class App;

	/// Factory principal para crear contextos de renderizado
	class AppFactory {
	public:
		/// Crea un contexto de librería con gestor de ventanas integrado
		/// @param backend Backend de ventanas a usar (Auto selecciona automáticamente)
		/// @return Contexto de aplicación creado
		static std::unique_ptr<App> createApp(WindowBackend backend = WindowBackend::Auto);

	private:
		AppFactory() = delete;
	};
} // namespace pgrender
