#pragma once
#include "pgrender/types.h"
#include <memory>

namespace pgrender
{
	class App;

	/// Factory principal para crear contextos de renderizado
	class AppFactory {
	public:
		/// Crea un contexto de librer�a con gestor de ventanas integrado
		/// @param backend Backend de ventanas a usar (Auto selecciona autom�ticamente)
		/// @return Contexto de aplicaci�n creado
		static std::unique_ptr<App> createApp(WindowBackend backend = WindowBackend::Auto);

	private:
		AppFactory() = delete;
	};
} // namespace pgrender
