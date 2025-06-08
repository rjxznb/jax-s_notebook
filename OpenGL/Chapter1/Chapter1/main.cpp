#include<iostream>
#include "shader_program.h"
#include<glad/glad.h> // 一定要先放glad.h，再放glfw3.h
#include<glfw/glfw3.h>
using namespace std;

extern const char* vertexShaderSource; // 着色器硬编码；

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	glfwInit();
	// 初始化一些配置的选项；
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// 创建GLFW窗口；
	auto window = glfwCreateWindow(800, 600, "hello_world", NULL, NULL);
	// 创建上下文显示窗口；
	glfwMakeContextCurrent(window);
	// 在实际使用opengl函数之前需要初始化glad，就是将glfw库的依据函数名加载函数地址的函数glfwGetProcAddress传入；
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	// 在渲染视口之前需要告诉opengl视口尺寸，以及位置，我们可以将视口尺寸设置为小于GLFW的尺寸，我们可以在OpenGL视口之外显示其他元素；
	glViewport(0, 0, 800, 600);
	// 为该窗口注册resize的回调函数；
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 编译并启动着色器
	unsigned int shader_program; // 着色器程序句柄ID；
	compile_link_exec_shader_program(shader_program); // 编译并链接着色器；

	// 创建顶点缓冲区数组对象；
	unsigned int VAO;
	glGenVertexArrays(1, &VAO);
	
	// 绑定VAO到上下文对象；
	glBindVertexArray(VAO);

	// 创建顶点缓冲对象VBO，并将顶点数组vertices存入该缓冲对象VBO；
	// 顶点数组：这里只有xyz坐标属性，这个数组中有三个顶点；
	float vertices[]{ -0.5f, -0.5f, 0.0f,
						0.5f, -0.5f, 0.0f,
						0.0f, 0.5f, 0.0f,
					};
	unsigned int VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	// 设置顶点属性指针：
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0); // 这是大坑，这个函数名一定要对，不是glVertexAttribIPointer，在pointer之前没有I；
	glEnableVertexAttribArray(0);

	// 设置完之后将VAO和VBO脱离上下文对象；
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	// 事件循环：也叫render loop；
	while (!glfwWindowShouldClose(window)) {
		// 如果用户按下了esc键，那么就会返回GLFW_PRESS，否则会返回GLFW_RELEASE；
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		glClearColor(0.2f, 0.2f, 0.3f, 1.0f); // 需要在glClear前调用，该函数表示在对颜色缓冲区调用glClear的时候，采用改颜色清屏，也就是渲染成该颜色；
		glClear(GL_COLOR_BUFFER_BIT); // 清空缓冲区，我们这里指定为颜色缓冲区；

		// 绘制：
		start_shader_program(shader_program); // 启动着色器程序；
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shader_program);

	glfwTerminate();
}