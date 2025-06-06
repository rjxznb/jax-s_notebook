#include<iostream>
#include<glad/glad.h>

// 初始化并启动着色程序，就是链接起两个着色器.o文件，之后启动；
bool start_shader_program(unsigned int& shaderProgram, unsigned int vertexshader, unsigned int fragshader) {
	shaderProgram = glCreateProgram(); // 创建着色器程序对象；
	glAttachShader(shaderProgram, vertexshader); // 将顶点着色器附加到着色程序上；
	glAttachShader(shaderProgram, fragshader); // 将片段着色器附加到着色程序上；
	glLinkProgram(shaderProgram); // 将编译后的着色程序链接到本程序；

	glUseProgram(shaderProgram); // 激活这个程序对象
	glDeleteShader(vertexshader); // 启动着色器程序之后就可以删除两个着色器对象啦；
	glDeleteShader(fragshader);

	// 查看是否链接成功；
	int success;
	char infolog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infolog);
		std::cout << "Vertex Shader Compile Error: " << infolog << std::endl;
		return false;
	}
	return true;
}

// 画出Vertex_Array_Object缓冲区对象里的顶点；
void Draw_VAO() {

}
