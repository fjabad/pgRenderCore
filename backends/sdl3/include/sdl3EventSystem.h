#pragma once
#include "pgrender/eventSystem.h"
#include <memory>

namespace pgrender::backends::sdl3 {

	class SDL3PerWindowEventSystem : public IEventSystem {
	public:
		SDL3PerWindowEventSystem() = default;
		~SDL3PerWindowEventSystem();

		// No copiable ni movible
		SDL3PerWindowEventSystem(const SDL3PerWindowEventSystem&) = delete;
		SDL3PerWindowEventSystem& operator=(const SDL3PerWindowEventSystem&) = delete;
		SDL3PerWindowEventSystem(SDL3PerWindowEventSystem&&) = delete;
		SDL3PerWindowEventSystem& operator=(SDL3PerWindowEventSystem&&) = delete;

		void pollEvents() override;
	};

} // namespace pgrender::backends::sdl3
