#pragma once
#include "window.h"
#include "eventSystem.h"
#include "types.h"
#include <vector>
#include <functional>
#include <memory>

namespace pgrender {
	/// Gestor unificado de ventanas
	/// Gestiona múltiples ventanas, displays, eventos y contextos gráficos
	class IWindowManager {
	public:
		virtual ~IWindowManager();

		// ========================================================================
		// Gestión de ventanas
		// ========================================================================

		/// Crea una nueva ventana
		/// @param config Configuración de la ventana
		/// @return ID único de la ventana creada
		virtual WindowID createWindow(const WindowConfig& config) = 0;

		/// Destruye una ventana existente
		/// @param id ID de la ventana a destruir
		void destroyWindow(WindowID id);

		/// Cierra todas las ventanas abiertas
		void closeAllWindows();

		/// Obtiene un puntero a una ventana
		/// @param id ID de la ventana
		/// @return Puntero a la ventana o nullptr si no existe
		IWindow* getWindow(WindowID id);
		const IWindow* getWindow(WindowID id) const;

		/// Obtiene la lista de IDs de ventanas activas
		/// @return Vector con los IDs de todas las ventanas abiertas
		std::vector<WindowID> getActiveWindows() const;

		/// Obtiene el número de ventanas abiertas
		/// @return Cantidad de ventanas activas
		size_t getWindowCount() const;

		/// Verifica si hay ventanas abiertas
		/// @return true si hay al menos una ventana abierta
		bool hasOpenWindows() const;

		// ========================================================================
		// Gestión de contextos gráficos
		// ========================================================================

		/// Obtiene el contexto gráfico asociado a una ventana
		/// @param id ID de la ventana
		/// @return Puntero al contexto o nullptr si no tiene
		IGraphicsContext* getWindowContext(WindowID id);

		/// Establece el contexto gráfico de una ventana
		/// @param id ID de la ventana
		/// @param context Contexto gráfico a asociar (toma ownership)
		void setWindowContext(WindowID id, std::unique_ptr<IGraphicsContext> context);

		// ========================================================================
		// Gestión de displays/monitores
		// ========================================================================

		/// Obtiene el número de displays conectados
		/// @return Cantidad de displays/monitores
		virtual int getDisplayCount() const = 0;

		/// Obtiene información sobre un display
		/// @param index Índice del display (0 a getDisplayCount()-1)
		/// @return Información del display
		virtual DisplayInfo getDisplayInfo(int index) const = 0;

		/// Obtiene el índice del display donde está una ventana
		/// @param windowId ID de la ventana
		/// @return Índice del display o -1 si no se puede determinar
		virtual int getWindowDisplayIndex(WindowID windowId) const = 0;

		// ========================================================================
		// Posicionamiento y geometría de ventanas
		// ========================================================================

		/// Centra una ventana en un display específico
		/// @param windowId ID de la ventana
		/// @param displayIndex Índice del display
		virtual void centerWindowOnDisplay(WindowID windowId, int displayIndex) = 0;

		/// Establece la posición de una ventana
		/// @param windowId ID de la ventana
		/// @param x Posición X en píxeles
		/// @param y Posición Y en píxeles
		virtual void setWindowPosition(WindowID windowId, int x, int y) = 0;

		/// Obtiene la posición de una ventana
		/// @param windowId ID de la ventana
		/// @param x Referencia donde se almacenará la posición X
		/// @param y Referencia donde se almacenará la posición Y
		virtual void getWindowPosition(WindowID windowId, int& x, int& y) const = 0;

		// ========================================================================
		// Estados de ventana
		// ========================================================================

		/// Maximiza una ventana
		/// @param windowId ID de la ventana
		virtual void maximizeWindow(WindowID windowId) = 0;

		/// Minimiza una ventana
		/// @param windowId ID de la ventana
		virtual void minimizeWindow(WindowID windowId) = 0;

		/// Restaura una ventana minimizada o maximizada
		/// @param windowId ID de la ventana
		virtual void restoreWindow(WindowID windowId) = 0;

		/// Eleva una ventana sobre las demás
		/// @param windowId ID de la ventana
		virtual void raiseWindow(WindowID windowId) = 0;

		// ========================================================================
		// Fullscreen
		// ========================================================================

		/// Establece el modo fullscreen exclusivo
		/// @param windowId ID de la ventana
		/// @param fullscreen true para activar, false para desactivar
		virtual void setFullscreen(WindowID windowId, bool fullscreen) = 0;

		/// Establece el modo fullscreen desktop (borderless windowed)
		/// @param windowId ID de la ventana
		/// @param fullscreen true para activar, false para desactivar
		virtual void setFullscreenDesktop(WindowID windowId, bool fullscreen) = 0;

		// ========================================================================
		// Apariencia de ventanas
		// ========================================================================

		/// Establece la opacidad de una ventana
		/// @param windowId ID de la ventana
		/// @param opacity Opacidad (0.0 = transparente, 1.0 = opaco)
		virtual void setWindowOpacity(WindowID windowId, float opacity) = 0;

		/// Obtiene la opacidad de una ventana
		/// @param windowId ID de la ventana
		/// @return Opacidad actual (0.0 a 1.0)
		virtual float getWindowOpacity(WindowID windowId) const = 0;

		/// Establece si la ventana tiene borde/decoración
		/// @param windowId ID de la ventana
		/// @param bordered true para mostrar borde, false para ocultarlo
		virtual void setBordered(WindowID windowId, bool bordered) = 0;

		// ========================================================================
		// Sistema de eventos por ventana
		// ========================================================================

		/// Procesa todos los eventos pendientes del sistema
		/// Debe llamarse en cada frame del loop principal
		void pollEvents();

		/// Obtiene el siguiente evento de una ventana específica
		/// @param windowId ID de la ventana
		/// @param event Referencia donde se almacenará el evento
		/// @return true si había un evento, false si la cola está vacía
		bool getEventForWindow(WindowID windowId, Event& event);

		/// Procesa y cierra ventanas marcadas para cerrar
		/// Debe llamarse después de procesar eventos en cada frame
		void processWindowClosures();

		/// Establece un callback que se llama por cada evento de una ventana
		/// @param windowId ID de la ventana
		/// @param callback Función a llamar por cada evento
		void setWindowEventCallback(WindowID windowId, EventCallback callback);

		/// Establece un filtro de eventos para una ventana
		/// Solo los eventos que pasen el filtro llegarán a la cola
		/// @param windowId ID de la ventana
		/// @param filter Función que retorna true para aceptar el evento
		void setWindowEventFilter(WindowID windowId, EventFilter filter);

		/// Establece un observador de eventos para una ventana
		/// El watcher recibe copia de todos los eventos sin consumirlos
		/// @param windowId ID de la ventana
		/// @param watcher Función que recibe cada evento
		void setWindowEventWatcher(WindowID windowId, EventFilter watcher);

		// ========================================================================
		// Estadísticas y depuración
		// ========================================================================

		/// Obtiene el número de eventos pendientes en la cola de una ventana
		/// @param windowId ID de la ventana
		/// @return Cantidad de eventos en cola
		size_t getWindowQueueSize(WindowID windowId) const;

		/// Obtiene el número total de eventos pendientes en todas las ventanas
		/// @return Cantidad total de eventos en todas las colas
		size_t getTotalQueuedEvents() const;

	protected:
		IWindowManager(std::unique_ptr<IEventSystem> eventSystem);
		
		void registerWindowCreation(std::unique_ptr<IWindow> window);

		// No copiable ni movible
		IWindowManager(const IWindowManager&) = delete;
		IWindowManager& operator=(const IWindowManager&) = delete;
		IWindowManager(IWindowManager&&) = delete;
		IWindowManager& operator=(IWindowManager&&) = delete;

		class Impl;
		std::unique_ptr<Impl> m_impl;
	};

} // namespace pgrender
