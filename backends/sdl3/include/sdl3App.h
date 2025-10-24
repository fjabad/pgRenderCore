#pragma once
#include "pgrender/windowManager.h"
#include "pgrender/inputSystem.h"
#include "pgrender/app.h"
#include <memory>

namespace pgrender::backends::sdl3 {

	// ============================================================================
	// Library Context
	// ============================================================================

	class SDL3App : public App {
	public:
		SDL3App();
		~SDL3App() override;

		SDL3App(const SDL3App&) = delete;
		SDL3App& operator=(const SDL3App&) = delete;
		SDL3App(SDL3App&&) = delete;
		SDL3App& operator=(SDL3App&&) = delete;

		std::unique_ptr<IGraphicsContext> createHeadlessContext(const ContextConfig& config) override;

		IWindowManager& getWindowManager() override;
		IInputSystem& getInputSystem() override;

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};


} // namespace pgrender::backends::sdl3
