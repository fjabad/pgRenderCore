#include "PGRenderCoreGL/pipelineGL.h"
#include "PGRenderCoreGL/shaderGL.h"

#include <GL/glew.h>
#include <stdexcept>
#include <iostream>

namespace pgrender {

	PipelineGL::PipelineGL(const Pipeline::Desc& desc)
		: m_desc(desc)
	{
	}

	PipelineGL::~PipelineGL() {
	}

	void PipelineGL::apply() const
	{
		glUseProgram(m_desc.program->nativeHandle());
		applyBlendMode();
		applyDepthState();
		applyCullMode();
		applyPolygonMode();
		// Aquí se pueden aplicar otros estados si es necesario
	}

	void PipelineGL::applyBlendMode() const {
		if (m_desc.customBlendState.enabled)
			glEnable(GL_BLEND);
		else {
			glDisable(GL_BLEND);
			return;
		}

		glBlendEquationSeparate(
			toGLBlendOp(m_desc.customBlendState.colorOp),
			toGLBlendOp(m_desc.customBlendState.alphaOp)
		);

		glBlendFuncSeparate(
			toGLBlendFactor(m_desc.customBlendState.srcColorFactor),
			toGLBlendFactor(m_desc.customBlendState.dstColorFactor),
			toGLBlendFactor(m_desc.customBlendState.srcAlphaFactor),
			toGLBlendFactor(m_desc.customBlendState.dstAlphaFactor)
		);

		glBlendColor(
			m_desc.customBlendState.constantColor.r,
			m_desc.customBlendState.constantColor.g,
			m_desc.customBlendState.constantColor.b,
			m_desc.customBlendState.constantColor.a);

	}

	void PipelineGL::applyDepthState() const {
		static const GLenum glFuncs[] = {
			GL_NEVER, GL_LESS, GL_LEQUAL, GL_GREATER,
			GL_GEQUAL, GL_EQUAL, GL_NOTEQUAL, GL_ALWAYS
		};

		static_assert(
			static_cast<int>(DepthFunc::Never) == 0 &&
			static_cast<int>(DepthFunc::Always) == 7, "DepthFunc enum values must match array indices");

		if (m_desc.depthState.depthTestEnabled) {
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(glFuncs[static_cast<int>(m_desc.depthState.depthFunc)]);
			glDepthMask(m_desc.depthState.depthWriteEnabled ? GL_TRUE : GL_FALSE);
		}
		else {
			glDisable(GL_DEPTH_TEST);
		}
	}

	void PipelineGL::applyCullMode() const {
		if (m_desc.cullMode == CullMode::None) {
			glDisable(GL_CULL_FACE);
		}
		else {
			glEnable(GL_CULL_FACE);
			glCullFace(m_desc.cullMode == CullMode::Back ? GL_BACK : GL_FRONT);
		}
	}

	void PipelineGL::applyPolygonMode() const {
		auto toGLMode = [](PolygonMode mode) -> GLenum {
			switch (mode) {
			case PolygonMode::Fill: return GL_FILL;
			case PolygonMode::Line: return GL_LINE;
			case PolygonMode::Point: return GL_POINT;
			default: return GL_FILL;
			}
			};
		glPolygonMode(GL_FRONT_AND_BACK, toGLMode(m_desc.polygonMode.mode));
	}

	unsigned int PipelineGL::toGLBlendFactor(BlendFactor factor) const
	{
		switch (factor) {
		case BlendFactor::Zero: return GL_ZERO;
		case BlendFactor::One: return GL_ONE;
		case BlendFactor::SrcColor: return GL_SRC_COLOR;
		case BlendFactor::OneMinusSrcColor: return GL_ONE_MINUS_SRC_COLOR;
		case BlendFactor::DstColor: return GL_DST_COLOR;
		case BlendFactor::OneMinusDstColor: return GL_ONE_MINUS_DST_COLOR;
		case BlendFactor::SrcAlpha: return GL_SRC_ALPHA;
		case BlendFactor::OneMinusSrcAlpha: return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFactor::DstAlpha: return GL_DST_ALPHA;
		case BlendFactor::OneMinusDstAlpha: return GL_ONE_MINUS_DST_ALPHA;
		case BlendFactor::ConstantColor: return GL_CONSTANT_COLOR;
		case BlendFactor::OneMinusConstantColor: return GL_ONE_MINUS_CONSTANT_COLOR;
		case BlendFactor::ConstantAlpha: return GL_CONSTANT_ALPHA;
		case BlendFactor::OneMinusConstantAlpha: return GL_ONE_MINUS_CONSTANT_ALPHA;
		default: return GL_ONE;
		}
	}

	unsigned int PipelineGL::toGLBlendOp(BlendOp op) const
	{
		switch (op) {
		case BlendOp::Add: return GL_FUNC_ADD;
		case BlendOp::Subtract: return GL_FUNC_SUBTRACT;
		case BlendOp::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
		case BlendOp::Min: return GL_MIN;
		case BlendOp::Max: return GL_MAX;
		default: return GL_FUNC_ADD;
		}
	}

} // namespace pgrender

