#include<iostream>
#include<glad/glad.h>

// 顶点着色器硬编码，输入只有一个3维向量aPos，输出三维齐次坐标表示的顶点，就是一个四维的向量表示gl_Postion到片段着色器，作为他的输入；
const char* vertexShaderSource = "#version 330 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main(){\n"
	"	gl_Postion=vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}";

// 初始化shader对象，返回值为是否编译成功；
bool initShader(unsigned int& vertexShader) {
	vertexShader = glCreateShader(GL_VERTEX_SHADER); // 创建着色器对象，传入创建的着色器类型，返回句柄ID；
	glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr); // 把源码传入顶点着色器对象；
	glCompileShader(vertexShader); // 编译vertex shader源码；
	int success;
	char infolog[512];
	// 查看编译vertex shader是否成功；
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infolog); // 如果不成功就获取编译错误；
		std::cout << "Vertex Shader Compile Error: " << infolog<<std::endl;
		return false;
	}
	return true;
}

// 链接哪个顶点属性并启用该属性；
void SetVertexAttrib(int location) {
	// 定义好属性指针；
	glVertexAttribPointer(location, 3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (void*)0);
	// 启用该位置的属性；
	glEnableVertexAttribArray(location);
}