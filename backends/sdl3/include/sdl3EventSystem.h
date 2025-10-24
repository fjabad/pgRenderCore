#pragma once
#include "pgrender/eventSystem.h"
#include <memory>

namespace pgrender::backends::sdl3 {

	class SDL3EventSystem : public IEventSystem {
	public:
		SDL3EventSystem() = default;
		~SDL3EventSystem();

		// No copiable ni movible
		SDL3EventSystem(const SDL3EventSystem&) = delete;
		SDL3EventSystem& operator=(const SDL3EventSystem&) = delete;
		SDL3EventSystem(SDL3EventSystem&&) = delete;
		SDL3EventSystem& operator=(SDL3EventSystem&&) = delete;

		void pollEvents() override;
	};

} // namespace pgrender::backends::sdl3
