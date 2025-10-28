#pragma once
#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <PGRenderCore/debugManager.h>

namespace pgrender {

    /**
     * @brief Gestor de debug para OpenGL.
     * Gestiona mensajes de debug, validaci�n y grupos de debug.
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
         * Requiere que el contexto OpenGL est� activo.
         * @param enableSynchronous true para debug s�ncrono (m�s lento pero preciso).
         * @return true si se inicializ� correctamente.
         */
        bool initialize(bool enableSynchronous = true);

        /**
         * @brief Finaliza el sistema de debug.
         */
        void shutdown();

        /**
         * @brief Verifica si el debug est� habilitado.
         */
        bool isEnabled() const { return m_enabled; }

        /**
         * @brief Establece el callback para mensajes de debug.
         * @param callback Funci�n a llamar cuando se recibe un mensaje.
         */
        void setCallback(const DebugCallback& callback) override;

        /**
         * @brief Establece el callback a nullptr (usa el por defecto).
         */
        void clearCallback() override;

        /**
         * @brief Habilita/deshabilita mensajes de un tipo espec�fico.
         * @param type Tipo de mensaje.
         * @param enabled true para habilitar, false para deshabilitar.
         */
        void setTypeEnabled(DebugType type, bool enabled) override;

        /**
         * @brief Habilita/deshabilita mensajes de una severidad espec�fica.
         * @param severity Severidad.
         * @param enabled true para habilitar, false para deshabilitar.
         */
        void setSeverityEnabled(DebugSeverity severity, bool enabled) override;

        /**
         * @brief Habilita/deshabilita mensajes de una fuente espec�fica.
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
         * @return true si se encontr� alg�n error.
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

        // Callback est�tico para OpenGL (llama al callback del usuario)
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
