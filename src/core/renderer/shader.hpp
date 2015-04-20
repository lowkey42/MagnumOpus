/**************************************************************************\
 * simple wrapper for OpenGL shader/programs	                          *
 *                                               ___                      *
 *    /\/\   __ _  __ _ _ __  _   _ _ __ ___     /___\_ __  _   _ ___     *
 *   /    \ / _` |/ _` | '_ \| | | | '_ ` _ \   //  // '_ \| | | / __|    *
 *  / /\/\ \ (_| | (_| | | | | |_| | | | | | | / \_//| |_) | |_| \__ \    *
 *  \/    \/\__,_|\__, |_| |_|\__,_|_| |_| |_| \___/ | .__/ \__,_|___/    *
 *                |___/                              |_|                  *
 *                                                                        *
 * Copyright (c) 2014 Florian Oetke                                       *
 *  Based on code of GDW-SS2014 project by Stefan Bodenschatz             *
 *  which was distributed under the MIT license.                          *
 *                                                                        *
 *  This file is part of MagnumOpus and distributed under the MIT License *
 *  See LICENSE file for details.                                         *
\**************************************************************************/

#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include "../asset/asset_manager.hpp"

namespace core {
namespace renderer {

	class Shader_program;

	struct Shader_compiler_error : public asset::Loading_failed {
		explicit Shader_compiler_error(const std::string& msg)noexcept : asset::Loading_failed(msg){}
	};

	enum class Shader_type {
		fragment,
		vertex
	};

	class Shader {
		public:
			Shader(Shader_type type, const std::string& source, const std::string name="unnamed")throw(Shader_compiler_error);
			~Shader()noexcept;

			Shader& operator=(Shader&&);

		private:
			friend class Shader_program;
			unsigned int _handle;
			std::string _name;
			mutable std::vector<Shader_program*> _attached_to;

			void _on_attach(Shader_program* prog)const;
			void _on_detach(Shader_program* prog)const;
	};

	class Vertex_layout;

	class Shader_program {
		public:
			Shader_program();
			Shader_program(Shader_program&&) = default;
			~Shader_program()noexcept;

			Shader_program& operator=(Shader_program&&) = default;

			Shader_program& attach_shader(std::shared_ptr<const Shader> shader);
			Shader_program& bind_all_attribute_locations(const Vertex_layout&);
			Shader_program& bind_attribute_location(const std::string& name, int l);
			Shader_program& build()throw(Shader_compiler_error);
			Shader_program& detach_all();


			Shader_program& bind();
			Shader_program& unbind();

			Shader_program& set_uniform(const std::string& name, int value);
			Shader_program& set_uniform(const std::string& name,float value);
			Shader_program& set_uniform(const std::string& name,const glm::vec2& value);
			Shader_program& set_uniform(const std::string& name,const glm::vec3& value);
			Shader_program& set_uniform(const std::string& name,const glm::vec4& value);
			Shader_program& set_uniform(const std::string& name,const glm::mat2& value);
			Shader_program& set_uniform(const std::string& name,const glm::mat3& value);
			Shader_program& set_uniform(const std::string& name,const glm::mat4& value);
			Shader_program& set_uniform(const std::string& name,const std::vector<float>& value);
			Shader_program& set_uniform(const std::string& name,const std::vector<glm::vec2>& value);
			Shader_program& set_uniform(const std::string& name,const std::vector<glm::vec3>& value);
			Shader_program& set_uniform(const std::string& name,const std::vector<glm::vec4>& value);

		private:
			unsigned int _handle;
			std::vector<std::shared_ptr<const Shader>> _attached_shaders;
			std::unordered_map<std::string,int> _uniform_locations;
	};

} /* namespace renderer */

namespace asset {
	template<>
	struct Loader<renderer::Shader> {
		using RT = std::shared_ptr<renderer::Shader>;

		static RT load(istream in) throw(Loading_failed){
			switch(in.aid().type()) {
				case Asset_type::frag_shader:
					return std::make_shared<renderer::Shader>(renderer::Shader_type::fragment, in.content(), in.aid().str());

				case Asset_type::vert_shader:
					return std::make_shared<renderer::Shader>(renderer::Shader_type::vertex, in.content(), in.aid().str());

				default:
					break;
			}

			throw Loading_failed("Unsupported assetId for shader: "+in.aid().str());
		}

		static void store(istream out, renderer::Shader&) throw(Loading_failed) {
			throw Loading_failed("Saving shaders is not supported!");
		}
	};
}
}
