#include "ShaderProgram.h"
#include "ChiGraphics/Utilities.h"
#include <iterator>
#include <stdexcept>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include "ChiGraphics/Utilities.h"

namespace CHISTUDIO {

ShaderProgram::ShaderProgram(const std::unordered_map<GLenum, std::string>& InShaderFilenames)
{
	assert(InShaderFilenames.count(GL_VERTEX_SHADER) == 1);
	assert(InShaderFilenames.count(GL_FRAGMENT_SHADER) == 1);
	for (auto& kv : InShaderFilenames) {
		std::string shader_path = GetShaderGLSLDir() + kv.second;
		std::ifstream ifs(shader_path, std::ifstream::in);
		std::string shader_code(std::istreambuf_iterator<char>{ifs}, {});
		ShaderHandles[kv.first] = LoadShader(kv.first, shader_code, shader_path);
	}

	ShaderProgram_ = glCreateProgram();
	GL_CHECK_ERROR();

	for (auto& kv : ShaderHandles) {
		GL_CHECK(glAttachShader(ShaderProgram_, kv.second));
	}

	GL_CHECK(glLinkProgram(ShaderProgram_));
	GLint link_status;
	GL_CHECK(glGetProgramiv(ShaderProgram_, GL_LINK_STATUS, &link_status));
	if (link_status != GL_TRUE) 
	{
		GLchar err_log_buf[kErrorLogBufferSize];
		GL_CHECK(glGetProgramInfoLog(ShaderProgram_, kErrorLogBufferSize, nullptr,
			err_log_buf));
		std::cerr << "Shader linking error: " << err_log_buf << std::endl;
		return;
	}

	// Cleanup after linking.
	for (auto& kv : ShaderHandles) {
		GLuint handle = kv.second;
		GL_CHECK(glDetachShader(ShaderProgram_, handle));
		GL_CHECK(glDeleteShader(handle));
	}
}

ShaderProgram::~ShaderProgram()
{
	GL_CHECK(glDeleteProgram(ShaderProgram_));
}

void ShaderProgram::Bind() const
{
	GL_CHECK(glUseProgram(ShaderProgram_));
}

void ShaderProgram::Unbind() const
{
	GL_CHECK(glUseProgram(0));
}

GLint ShaderProgram::GetAttributeLocation(const std::string& InName) const
{
	GLint loc = glGetAttribLocation(ShaderProgram_, InName.c_str());
	GL_CHECK_ERROR();
	return loc;
}

void ShaderProgram::SetUniform(const std::string& InName, const glm::mat4& InValue) const
{
	GLint loc = glGetUniformLocation(ShaderProgram_, InName.c_str());
	GL_CHECK_ERROR();
	GL_CHECK(glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(InValue)));
}

void ShaderProgram::SetUniform(const std::string& InName, const glm::mat3& InValue) const
{
	GLint loc = glGetUniformLocation(ShaderProgram_, InName.c_str());
	GL_CHECK_ERROR();
	GL_CHECK(glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(InValue)));
}

void ShaderProgram::SetUniform(const std::string& InName, const glm::vec3& InValue) const
{
	GLint loc = glGetUniformLocation(ShaderProgram_, InName.c_str());
	GL_CHECK_ERROR();
	GL_CHECK(glUniform3fv(loc, 1, glm::value_ptr(InValue)));
}

void ShaderProgram::SetUniform(const std::string& InName, float InValue) const
{
	GLint loc = glGetUniformLocation(ShaderProgram_, InName.c_str());
	GL_CHECK_ERROR();
	GL_CHECK(glUniform1f(loc, InValue));
}

void ShaderProgram::SetUniform(const std::string& InName, int InValue) const
{
	GLint loc = glGetUniformLocation(ShaderProgram_, InName.c_str());
	GL_CHECK_ERROR();
	GL_CHECK(glUniform1i(loc, InValue));
}

GLuint ShaderProgram::LoadShader(GLenum InType, std::string InShaderCode, const std::string& InShaderFilename)
{
	GLuint shader_handle = glCreateShader(InType);
	GL_CHECK_ERROR();
	auto version_pos = InShaderCode.find("#version");
	if (version_pos == std::string::npos) {
		throw std::runtime_error("Shader file " + InShaderFilename +
			" has no #version!");
	}
	auto version_end = InShaderCode.find('\n', version_pos);
	std::string version =
		InShaderCode.substr(version_pos, version_end + 1 - version_pos);
	InShaderCode = InShaderCode.substr(version_end + 1);
	std::vector<const char*> codes = { version.c_str(), InShaderCode.c_str() };

	codes.insert(codes.begin() + 1, "#define ASSIGNMENT_5_STARTER\n");
	GL_CHECK(glShaderSource(shader_handle, (GLsizei)codes.size(), codes.data(),
		nullptr));
	GL_CHECK(glCompileShader(shader_handle));

	GLint compile_status;
	GL_CHECK(glGetShaderiv(shader_handle, GL_COMPILE_STATUS, &compile_status));
	if (compile_status != GL_TRUE) {
		char err_log_buf[kErrorLogBufferSize];
		GL_CHECK(glGetShaderInfoLog(shader_handle, kErrorLogBufferSize, nullptr,
			err_log_buf));
		std::cerr << "Shader compilation error: " << err_log_buf << std::endl;
		return 0;
	}

	return shader_handle;
}

}