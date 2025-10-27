#pragma once

#include <string>
#include <functional>
namespace PGRenderCore {

    /**
     * @brief Severidad de mensaje de debug.
     */
    enum class DebugSeverity {
        High,           // Errores cr�ticos
        Medium,         // Warnings importantes
        Low,            // Informaci�n general
        Notification    // Notificaciones de bajo nivel
    };


    /**
     * @brief Fuente del mensaje de debug.
     */
    enum class DebugSource {
        API,                // Llamadas a la API de OpenGL
        WindowSystem,       // Sistema de ventanas (WGL, GLX, etc.)
        ShaderCompiler,     // Compilador de shaders
        ThirdParty,         // Librer�as de terceros
        Application,        // Aplicaci�n del usuario
		ValidationLayer,    // Capas de validaci�n
		Driver,			    // Controlador de GPU
        Other               // Otras fuentes
    };

    /**
     * @brief Tipo de mensaje de debug.
     */
    enum class DebugType {
        Error,              // Error de API
        DeprecatedBehavior, // Uso de funcionalidad deprecated
        UndefinedBehavior,  // Comportamiento indefinido
        Portability,        // Problemas de portabilidad
        Performance,        // Problemas de performance
        Marker,             // Marker/label de grupo
        PushGroup,          // Inicio de grupo de debug
        PopGroup,           // Fin de grupo de debug
        Other               // Otros mensajes
    };

    /**
     * @brief Informaci�n de un mensaje de debug.
     */
    struct DebugMessage {
        DebugSeverity severity;
        DebugSource source;
        DebugType type;
        uint32_t id;
        std::string message;
    };

    using DebugCallback = std::function<void(const DebugMessage&)>;

    class DebugManager {
    public:
        /**
         * @brief Inicializa el sistema de debug.
         * Requiere que el contexto OpenGL est� activo.
         * @param enableSynchronous true para debug s�ncrono (m�s lento pero preciso).
         * @return true si se inicializ� correctamente.
         */
        virtual bool initialize(bool enableSynchronous = true) = 0;

        /**
         * @brief Finaliza el sistema de debug.
         */
        virtual void shutdown() = 0;

        /**
         * @brief Verifica si el debug est� habilitado.
         */
        virtual bool isEnabled() const = 0;

        /**
         * @brief Establece el callback para mensajes de debug.
         * @param callback Funci�n a llamar cuando se recibe un mensaje.
         */
        virtual void setCallback(const DebugCallback& callback) = 0;

        /**
         * @brief Establece el callback a nullptr (usa el por defecto).
         */
        virtual void clearCallback() = 0;

        /**
         * @brief Habilita/deshabilita mensajes de un tipo espec�fico.
         * @param type Tipo de mensaje.
         * @param enabled true para habilitar, false para deshabilitar.
         */
        virtual void setTypeEnabled(DebugType type, bool enabled) = 0;

        /**
         * @brief Habilita/deshabilita mensajes de una severidad espec�fica.
         * @param severity Severidad.
         * @param enabled true para habilitar, false para deshabilitar.
         */
        virtual void setSeverityEnabled(DebugSeverity severity, bool enabled) = 0;

        /**
         * @brief Habilita/deshabilita mensajes de una fuente espec�fica.
         * @param source Fuente.
         * @param enabled true para habilitar, false para deshabilitar.
         */
        virtual void setSourceEnabled(DebugSource source, bool enabled) = 0;

        /**
         * @brief Inserta un mensaje de debug personalizado.
         * @param type Tipo de mensaje.
         * @param severity Severidad.
         * @param message Texto del mensaje.
         */
        virtual void insertMessage(DebugType type, DebugSeverity severity, const std::string& message) = 0;

        /**
         * @brief Inicia un grupo de debug (para organizar mensajes).
         * @param name Nombre del grupo.
         */
        virtual void pushDebugGroup(const std::string& name) = 0;

        /**
         * @brief Finaliza el grupo de debug actual.
         */
        virtual void popDebugGroup() = 0;

        /**
         * @brief Obtiene estad�sticas de mensajes de debug.
         */
        struct Statistics {
            uint32_t totalMessages = 0;
            uint32_t errors = 0;
            uint32_t warnings = 0;
            uint32_t performance = 0;
            uint32_t other = 0;
        };

        /**
         * @brief Obtiene las estad�sticas de mensajes.
         */
        const Statistics& getStatistics() const { return m_statistics; }

        /**
         * @brief Reinicia las estad�sticas.
         */
		void resetStatistics() { m_statistics = Statistics(); }


        virtual ~DebugManager() = default;
    protected:
		Statistics m_statistics;
    };

}
