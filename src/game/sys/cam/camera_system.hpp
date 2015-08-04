/**************************************************************************\
 * manages cameras                                                        *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2014 Florian Oetke                                       *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include "../../../core/ecs/ecs.hpp"
#include "../../../core/units.hpp"
#include "../../../core/utils/template_utils.hpp"
#include "../../../core/renderer/camera.hpp"
#include <core/renderer/texture.hpp>
#include <core/renderer/vertex_object.hpp>
#include <core/renderer/graphics_ctx.hpp>

#include "camera_target_comp.hpp"

namespace mo {
	class Game_engine;

namespace sys {
namespace cam {

	struct VScreen {
		renderer::Camera camera;
		std::vector<ecs::Entity_ptr> targets;

		renderer::Framebuffer vscreen;

		VScreen(glm::vec2 size, float world_scale);
		VScreen(VScreen&&) = default;
	};

	class Camera_system {
		public:
			Camera_system(ecs::Entity_manager& entity_manager, Game_engine& engine);

			void update(Time dt);

			// Func = void(const Camera&, const std::vector<ecs::Entity_ptr>&)
			template<typename Func>
			void draw(Func f) {
				for(auto& c : _cameras) {
					renderer::Framebuffer_binder vsb = c.vscreen;

					c.camera.bind_viewport();

					c.vscreen.clear();

					f(c.camera, c.targets);
				}

				_gctx.reset_viewport();
			}

			auto vscreens()const noexcept {return util::range(_cameras);}

			auto main_camera()const noexcept -> const auto& {
				return _main_camera;
			}

			void reset();

		private:
			bool _uninitialized = true;
			renderer::Graphics_ctx& _gctx;
			Camera_target_comp::Pool& _targets;
			const glm::vec2 _vscreen_size;

			renderer::Camera _main_camera;
			std::vector<VScreen> _cameras;
	};

}
}
}
