#include<iostream>
#include<glad/glad.h> // 一定要先放glad.h，再放glfw3.h
#include<glfw/glfw3.h>
using namespace std;

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
	auto window = glfwCreateWindow(100, 600, "hello_world", NULL, NULL);
	// 创建上下文显示窗口；
	glfwMakeContextCurrent(window);
	// 在实际使用opengl函数之前需要初始化glad，就是将glfw库的依据函数名加载函数地址的函数glfwGetProcAddress传入；
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
	// 在渲染视口之前需要告诉opengl视口尺寸，以及位置，我们可以将视口尺寸设置为小于GLFW的尺寸，我们可以在OpenGL视口之外显示其他元素；
	glad_glViewport(0, 0, 100, 200);
	// 为该窗口注册resize的回调函数；
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	// 事件循环：也叫render loop；
	while (!glfwWindowShouldClose(window)) {
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwTerminate();
}