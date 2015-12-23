#include "shader.hpp"

#include "../utils/string_utils.hpp"
#include "../utils/log.hpp"

#include <GL/glew.h>
#include <glm/gtc/type_ptr.hpp>

#include "vertex_object.hpp"

namespace mo {
namespace renderer {

	namespace {
		GLenum shader_type_to_GLenum(Shader_type type) {
			switch(type) {
				case Shader_type::vertex:   return GL_VERTEX_SHADER;
				case Shader_type::fragment: return GL_FRAGMENT_SHADER;
				default: FAIL("Unsupported ShaderType");
				return 0;
			}
		}

		util::maybe<const std::string> read_gl_info_log(unsigned int handle) {
			auto info_log_length=0;
			glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &info_log_length);
			if( info_log_length>1 ) {
				auto info_log_buffer = std::string(info_log_length, ' ');

				glGetShaderInfoLog(handle, info_log_length, NULL, &info_log_buffer[0]);
				return std::move(info_log_buffer);
			};

			return util::nothing();
		}

		bool get_gl_proc_status(unsigned int handle, GLenum status_type) {
			GLint success = 0;
			glGetProgramiv(handle, status_type, &success);

			return success!=0;
		}
		bool get_gl_shader_status(unsigned int handle, GLenum status_type) {
			GLint success = 0;
			glGetShaderiv(handle, status_type, &success);

			return success!=0;
		}
	}

	Shader::Shader(Shader_type type, const std::string& source, const std::string name)throw(Shader_compiler_error) {
		char const * source_pointer = source.c_str();
		int len = source.length();

		_handle = glCreateShader(shader_type_to_GLenum(type));
		glShaderSource(_handle, 1, &source_pointer , &len);
		glCompileShader(_handle);


		auto log = read_gl_info_log(_handle);
		bool success = get_gl_shader_status(_handle, GL_COMPILE_STATUS);

		INFO("Compiling shader:"<<name);
		read_gl_info_log(_handle).process([](const auto& _){
			DEBUG("Shader compiler log: \n"<<_);
		});

		if(!success)
			throw Shader_compiler_error("Shader compiler failed for \""+name+"\": "+log.get_or_other("NO LOG"));
	}
	Shader::~Shader()noexcept {
		if(_handle!=0)
			glDeleteShader(_handle);
	}

	void Shader::_on_attach(Shader_program* prog)const {
		_attached_to.push_back(prog);
	}
	void Shader::_on_detach(Shader_program* prog)const {
		_attached_to.erase(std::remove(_attached_to.begin(), _attached_to.end(), prog), _attached_to.end());
	}
	Shader& Shader::operator=(Shader&& s) {
		if(_handle!=0)
			glDeleteShader(_handle);

		_handle = s._handle;
		_name = std::move(s._name);
		s._handle = 0;

		for(auto prog : _attached_to)
			prog->build();

		return *this;
	}

	Shader_program::Shader_program() {
		_handle = glCreateProgram();
	}
	Shader_program::~Shader_program()noexcept {
		detach_all();

		glDeleteProgram(_handle);
	}

	Shader_program& Shader_program::attach_shader(std::shared_ptr<const Shader> shader) {
		shader->_on_attach(this);
		_attached_shaders.emplace_back(shader);
		return *this;
	}
	Shader_program& Shader_program::detach_all() {
		for(auto& s : _attached_shaders)
			s->_on_detach(this);

		_attached_shaders.clear();
		return *this;
	}

	Shader_program& Shader_program::build()throw(Shader_compiler_error) {
		for(auto& s : _attached_shaders)
			glAttachShader(_handle, s->_handle);


		glLinkProgram(_handle);

		auto log = read_gl_info_log(_handle);
		bool success = get_gl_proc_status(_handle, GL_LINK_STATUS);

		log.process([](const auto& _){
			INFO("Linking shader: "<<_);
		});

		if(!success)
			throw Shader_compiler_error("Shader linker failed: "+log.get_or_other("NO LOG"));

		glValidateProgram(_handle);
		read_gl_info_log(_handle).process([](const auto& _){
			DEBUG("Shader validation log: \n"<<_);
		});

		for(auto& s : _attached_shaders)
			glDetachShader(_handle, s->_handle);

		_uniform_locations.clear();  // clear uniform cache

		return *this;
	}

	Shader_program& Shader_program::bind_all_attribute_locations(const Vertex_layout& vl) {
		vl.setup_shader(*this);
		return *this;
	}
	Shader_program& Shader_program::bind_attribute_location(const std::string& name, int l) {
		glBindAttribLocation(_handle, l, name.c_str());
		return *this;
	}


	Shader_program& Shader_program::bind() {
		glUseProgram(_handle);

		return *this;
	}
	Shader_program& Shader_program::unbind() {
		glUseProgram(0);

		return *this;
	}

#define SHADER_SETU(TYPE,CALL) \
	Shader_program& Shader_program::set_uniform(const std::string& name, TYPE value) {\
		auto it = _uniform_locations.find(name);\
		if (it == _uniform_locations.end()) {\
			it = _uniform_locations.emplace(name,\
			glGetUniformLocation(_handle, name.c_str())).first;\
		}\
		CALL;\
		return *this;\
	}

    SHADER_SETU(int,                           glUniform1i(it->second, value))
    SHADER_SETU(float,                         glUniform1f(it->second, value))
    SHADER_SETU(const glm::vec2&,              glUniform2fv(it->second, 1, glm::value_ptr(value)))
    SHADER_SETU(const glm::vec3&,              glUniform3fv(it->second, 1, glm::value_ptr(value)))
    SHADER_SETU(const glm::vec4&,              glUniform4fv(it->second, 1, glm::value_ptr(value)))
    SHADER_SETU(const glm::mat2&,              glUniformMatrix2fv(it->second, 1, GL_FALSE, glm::value_ptr(value)))
    SHADER_SETU(const glm::mat3&,              glUniformMatrix3fv(it->second, 1, GL_FALSE, glm::value_ptr(value)))
    SHADER_SETU(const glm::mat4&,              glUniformMatrix4fv(it->second, 1, GL_FALSE, glm::value_ptr(value)))
    SHADER_SETU(const std::vector<float>& ,    glUniform1fv(it->second, value.size(), value.data()))
    SHADER_SETU(const std::vector<glm::vec2>&, glUniform2fv(it->second, value.size(), reinterpret_cast<const GLfloat*>(value.data())))
    SHADER_SETU(const std::vector<glm::vec3>&, glUniform3fv(it->second, value.size(), reinterpret_cast<const GLfloat*>(value.data())))
    SHADER_SETU(const std::vector<glm::vec4>&, glUniform4fv(it->second, value.size(), reinterpret_cast<const GLfloat*>(value.data())))

#undef SHADER_SETU

} /* namespace renderer */
}
