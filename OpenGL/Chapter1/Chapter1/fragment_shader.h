#pragma once
#include<glad/glad.h>

// 片段着色器源码；
const char* fragementshadersource = "#version 330 core\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
"}\0";

// 和vertex_shader对象一样的初始化逻辑；
bool init_fragment_shader(unsigned int& FragShader) {
	FragShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(FragShader, 1, &fragementshadersource, NULL);
	glCompileShader(FragShader);
	int success;
	char infolog[512];
	// 查看编译vertex shader是否成功；
	glGetShaderiv(FragShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(FragShader, 512, NULL, infolog); // 如果不成功就获取编译错误；
		std::cout << __builtin_FILE() << " : " << __LINE__ << " : " << "Vertex Shader Compile Error: " << infolog << std::endl;
		return false;
	}
	return true;
}