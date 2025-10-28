#include "PGRenderCoreGL/shaderGL.h"
#include <GL/glew.h> // Solo aquí
#include <vector>
#include <stdexcept>

namespace pgrender {

    ShaderGL::ShaderGL(const Program::Desc& desc)
        : m_desc(desc), m_programId(0), m_lastError()
    {}

    ShaderGL::~ShaderGL() {
        release();
    }

    bool ShaderGL::compile() {
        m_lastError.clear();

        if (m_programId != 0) {
            release();
        }

        m_programId = glCreateProgram();
        if (m_programId == 0) {
            m_lastError = "Failed to create GL program";
            return false;
        }

        // Establecer label para debugging si se proporciona
        if (m_desc.debugName && glObjectLabel) {
            glObjectLabel(GL_PROGRAM, m_programId, -1, m_desc.debugName);
        }

        for (const auto& stageSource : m_desc.stages) {
            unsigned int glType = shaderTypeToGL(stageSource.stage);
            unsigned int shader = 0;
            if (!compileShaderStage(glType, stageSource.source, shader)) {
                // Error ya almacenado en m_lastError
                release();
                return false;
            }
            glAttachShader(m_programId, shader);
            m_shaderObjects[glType] = shader;
        }

        glLinkProgram(m_programId);

        GLint linkStatus = GL_FALSE;
        glGetProgramiv(m_programId, GL_LINK_STATUS, &linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint logLen = 0;
            glGetProgramiv(m_programId, GL_INFO_LOG_LENGTH, &logLen);
            if (logLen > 1) {
                std::vector<char> infoLog(logLen);
                glGetProgramInfoLog(m_programId, logLen, nullptr, infoLog.data());
                m_lastError = std::string("Program link error: ") + infoLog.data();
            }
            else {
                m_lastError = "Unknown program link error";
            }
            release();
            return false;
        }

        // Detach shaders (ya no son necesarios después del link)
        detachAndDeleteShaders();
        return true;
    }

    void ShaderGL::release() {
        if (m_programId != 0) {
            glDeleteProgram(m_programId);
            m_programId = 0;
        }
        for (auto& shaderPair : m_shaderObjects) {
            glDeleteShader(shaderPair.second);
        }
        m_shaderObjects.clear();
        m_lastError.clear();
    }

    bool ShaderGL::compileShaderStage(unsigned int shaderType, const std::string& source, unsigned int& outShader) {
        GLuint shader = glCreateShader(shaderType);
        if (shader == 0) {
            m_lastError = "Failed to create shader object";
            return false;
        }
        const char* src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint compileStatus = GL_FALSE;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
        if (compileStatus != GL_TRUE) {
            GLint logLen = 0;
            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
            if (logLen > 1) {
                std::vector<char> infoLog(logLen);
                glGetShaderInfoLog(shader, logLen, nullptr, infoLog.data());
                m_lastError = std::string("Shader compile error: ") + infoLog.data();
            }
            else {
                m_lastError = "Unknown shader compile error";
            }
            glDeleteShader(shader);
            return false;
        }

        outShader = shader;
        return true;
    }

    void ShaderGL::detachAndDeleteShaders() {
        for (const auto& shaderPair : m_shaderObjects) {
            glDetachShader(m_programId, shaderPair.second);
            glDeleteShader(shaderPair.second);
        }
        m_shaderObjects.clear();
    }

    std::string ShaderGL::getLastError() const {
        return m_lastError;
    }

    unsigned long ShaderGL::nativeHandle() const {
        return static_cast<unsigned long>(m_programId);
    }

    const Program::Desc& ShaderGL::getDesc() const {
        return m_desc;
    }

    unsigned int ShaderGL::shaderTypeToGL(ShaderStage stage) const {
        switch (stage) {
        case ShaderStage::Vertex: return GL_VERTEX_SHADER;
        case ShaderStage::Fragment: return GL_FRAGMENT_SHADER;
        case ShaderStage::Geometry: return GL_GEOMETRY_SHADER;
        case ShaderStage::TessControl: return GL_TESS_CONTROL_SHADER;
        case ShaderStage::TessEvaluation: return GL_TESS_EVALUATION_SHADER;
        case ShaderStage::Compute: return GL_COMPUTE_SHADER;
            // Para trazado de rayos, en OpenGL 4.6 no hay soporte nativo,
            // estas etapas pueden ser ignoradas o gestionadas externamente.
        case ShaderStage::RayGeneration:
        case ShaderStage::ClosestHit:
        case ShaderStage::AnyHit:
        case ShaderStage::Miss:
        case ShaderStage::Intersection:
        case ShaderStage::Callable:
            throw std::runtime_error("Ray tracing shader stages are not supported in OpenGL 4.x");
        default:
            throw std::runtime_error("Unknown shader stage");
        }
    }

} // namespace pgrender
