#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <PGRenderCore/debugManager.h>

namespace pgrender {

    /**
     * @brief Gestor de debug para OpenGL.
     * Gestiona mensajes de debug, validación y grupos de debug.
     */
    class DebugManagerGL : public DebugManager {
    public:
        /**
         * @brief Constructor.
         */
        DebugManagerGL();

        /**
         * @brief Destructor.
         */
        ~DebugManagerGL();

        /**
         * @brief Inicializa el sistema de debug.
         * Requiere que el contexto OpenGL esté activo.
         * @param enableSynchronous true para debug síncrono (más lento pero preciso).
         * @return true si se inicializó correctamente.
         */
        bool initialize(bool enableSynchronous = true);

        /**
         * @brief Finaliza el sistema de debug.
         */
        void shutdown();

        /**
         * @brief Verifica si el debug está habilitado.
         */
        bool isEnabled() const { return m_enabled; }

        /**
         * @brief Establece el callback para mensajes de debug.
         * @param callback Función a llamar cuando se recibe un mensaje.
         */
        void setCallback(const DebugCallback& callback) override;

        /**
         * @brief Establece el callback a nullptr (usa el por defecto).
         */
        void clearCallback() override;

        /**
         * @brief Habilita/deshabilita mensajes de un tipo específico.
         * @param type Tipo de mensaje.
         * @param enabled true para habilitar, false para deshabilitar.
         */
        void setTypeEnabled(DebugType type, bool enabled) override;

        /**
         * @brief Habilita/deshabilita mensajes de una severidad específica.
         * @param severity Severidad.
         * @param enabled true para habilitar, false para deshabilitar.
         */
        void setSeverityEnabled(DebugSeverity severity, bool enabled) override;

        /**
         * @brief Habilita/deshabilita mensajes de una fuente específica.
         * @param source Fuente.
         * @param enabled true para habilitar, false para deshabilitar.
         */
        void setSourceEnabled(DebugSource source, bool enabled) override;

        /**
         * @brief Inserta un mensaje de debug personalizado.
         * @param type Tipo de mensaje.
         * @param severity Severidad.
         * @param message Texto del mensaje.
         */
        void insertMessage(DebugType type, DebugSeverity severity, const std::string& message) override;

        /**
         * @brief Inicia un grupo de debug (para organizar mensajes).
         * @param name Nombre del grupo.
         */
        void pushDebugGroup(const std::string& name) override;

        /**
         * @brief Finaliza el grupo de debug actual.
         */
        void popDebugGroup() override;

        /**
         * @brief Verifica errores de OpenGL y los reporta.
         * @param context Contexto descriptivo (ej: "After drawing triangle").
         * @return true si se encontró algún error.
         */
        bool checkGLError(const char* context = nullptr);

        /**
         * @brief Callback por defecto que imprime mensajes a consola.
         */
        static void defaultCallback(const DebugMessage& msg);

        DebugCallback m_callback;
        Statistics m_statistics;
    private:
        bool m_enabled = false;
        bool m_synchronous = false;
        uint32_t m_groupDepth = 0;

        // Callback estático para OpenGL (llama al callback del usuario)
        //static void GLAPIENTRY debugCallbackStatic(
        //    unsigned int source,
        //    unsigned int type,
        //    unsigned int id,
        //    unsigned int severity,
        //    int length,
        //    const char* message,
        //    const void* userParam
        //);

    };

} // namespace pgrender
