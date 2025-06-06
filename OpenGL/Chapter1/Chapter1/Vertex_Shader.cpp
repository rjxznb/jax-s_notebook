#include<iostream>
#include<glad/glad.h>

// 顶点着色器硬编码
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