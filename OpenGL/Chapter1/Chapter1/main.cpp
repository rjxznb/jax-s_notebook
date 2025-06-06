#include<iostream>
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
	// 事件循环：也叫render loop；
	while (!glfwWindowShouldClose(window)) {
		glfwSwapBuffers(window);
		// 如果用户按下了esc键，那么就会返回GLFW_PRESS，否则会返回GLFW_RELEASE；
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
		glClearColor(0.2f, 0.2f, 0.3f, 1.0f); // 需要在glClear前调用，该函数表示在对颜色缓冲区调用glClear的时候，采用改颜色清屏，也就是渲染成该颜色；
		glClear(GL_COLOR_BUFFER_BIT); // 清空缓冲区，我们这里指定为颜色缓冲区；
		glfwPollEvents();
	}
	glfwTerminate();
}