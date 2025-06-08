#include"fragment_shader.h"
#include"vertex_shader.h"
#include<glad/glad.h>

// 初始化并启动着色程序，就是链接起两个着色器.o文件，之后启动；
bool link_and_exec_shader_start_program(unsigned int& shaderProgram, unsigned int& vertexshader, unsigned int& fragshader) {
	shaderProgram = glCreateProgram(); // 创建着色器程序对象；
	glAttachShader(shaderProgram, vertexshader); // 将顶点着色器附加到着色程序上；
	glAttachShader(shaderProgram, fragshader); // 将片段着色器附加到着色程序上；
	glLinkProgram(shaderProgram); // 将编译后的着色程序链接到本程序；
	
	// 查看是否链接成功；
	int success;
	char infolog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infolog);
		std::cout << __builtin_FILE() << " : " << __LINE__ << " : " << "Vertex Shader Compile Error: " << infolog << std::endl;
		return false;
	}

	glDeleteShader(vertexshader); // 链接到着色器程序之后就可以删除两个着色器对象啦；
	glDeleteShader(fragshader);

	
	return true;
}

// 一键编译和链接：从编译顶点和片段着色器，到链接着色器程序；
void compile_link_exec_shader_program(unsigned int& shaderProgram) {
	unsigned int VertexShader, FragShader;
	if (!init_vertex_shader(VertexShader)) std::cout << __builtin_FILE() << " : " << __LINE__ << " : " << "init_vertex_shader_error" << std::endl;
	if (!init_fragment_shader(FragShader)) std::cout << __builtin_FILE() << " : "<< __LINE__ << " : " << "init_vertex_shader_error" << std::endl;
	if (!link_and_exec_shader_start_program(shaderProgram, VertexShader, FragShader)) std::cout << __builtin_FILE() << " : " << __LINE__ << " : " << "link_and_exec_shader_start_program_error" << std::endl;
}

// 启动着色器程序；
void start_shader_program(unsigned int& shaderProgram) {
	glUseProgram(shaderProgram); // 激活这个程序对象
}

// 画出Vertex_Array_Object缓冲区对象里的顶点；
void Draw_VAO() {

}
