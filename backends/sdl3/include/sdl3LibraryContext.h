#pragma once
#include "pgrender/windowManager.h"
#include "pgrender/inputSystem.h"
#include "pgrender/renderCore.h"
#include <memory>

namespace pgrender::backends::sdl3 {

	// ============================================================================
	// Library Context
	// ============================================================================

	class SDL3LibraryContext : public ILibraryContext {
	public:
		SDL3LibraryContext();
		~SDL3LibraryContext() override;

		SDL3LibraryContext(const SDL3LibraryContext&) = delete;
		SDL3LibraryContext& operator=(const SDL3LibraryContext&) = delete;
		SDL3LibraryContext(SDL3LibraryContext&&) = delete;
		SDL3LibraryContext& operator=(SDL3LibraryContext&&) = delete;

		std::unique_ptr<IGraphicsContext> createHeadlessContext(const ContextConfig& config) override;

		IWindowManager& getWindowManager() override;
		IInputSystem& getInputSystem() override;

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};


} // namespace pgrender::backends::sdl3
