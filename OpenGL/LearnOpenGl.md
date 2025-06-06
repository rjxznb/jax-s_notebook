# Preview

OpenGL 之 `gult/freeglut/glew/glfw/glad` 的联系与区别：

OpenGL只有框架没有实现，换句话说就是OpenGL只有函数声明没有源文件实现，类似于接口和虚函数。所有的实现是显卡生产商提供。比如NVIDIA或者AMD就要自己实现OpenGL函数内容，所以不同的生产商可以对自己的产品提供优化，毕竟代码是自己写的；

OpenGL函数库相关的API有核心库(gl)，实用库(glu)，辅助库(aux)、实用工具库(glut)，窗口库(glx、agl、wgl)和扩展函数库等。gl是核心，glu是对gl的部分封装，glx、agl、wgl 是针对不同窗口系统的函数；glut是为跨平台的OpenGL程序的工具包，比aux功能强大（aux很大程度上已经被glut库取代）；

<img src="F:\VS\jax-s_notebook\笔记图片\a3bdb41c5615d3056e98c89064c74ffe.png" style="zoom:50%;" />

## 配置环境

OpenGl只是一个提供接口名称和实际功能的协议，实际的接口代码实现是由不同显卡供应商编写的，如：英伟达，AMD等等，实际上我们使用的时候需要下载对应的GL库，这里我们使用的是GLFW3.3版本的库，他实际上实现的就是opengl3.3的协议接口；

编译glfw3.3库源代码过程：

- 到glfw的github仓库下载tag为3.3的源代码，之后用cmake进行编译：注意在编译过程中会出现指定的cmake版本过低，我们需要**到 `CMakeLists.txt` 中将最低cmake版本修改为`cmake_minimum_required(VERSION 3.10)`，不然会报错；**

![](F:\VS\jax-s_notebook\笔记图片\1749178314879.png)

- 用vs打开cmake生成的sln文件程序，之后用vs进行编译；
- 将源代码的include头文件，和编译生成的glfw3.lib文件分别放到vs的include和lib目录下，新建一个项目之后，修改vs中将新建的项目属性，**选择链接器->输入->依赖项加入 `glfw3.lib` 和 `opengl32.lib`；**

![](F:\VS\jax-s_notebook\笔记图片\1749178302079.png)

除此之外，因为OpenGL只是一个标准/规范，具体的实现是由驱动开发商针对特定显卡实现的。由于OpenGL驱动版本众多，他大多数函数的位置都无法在编译时确定下来，需要在运行时查询。所以任务就落在了开发者身上，开发者需要在运行时获取函数地址并将其保存在一个函数指针中供以后使用，取得地址的方法因平台而异，在Windows上会是类似这样：

```c++
// 定义函数原型
typedef void (*GL_GENBUFFERS) (GLsizei, GLuint*);
// 找到正确的函数并赋值给函数指针
GL_GENBUFFERS glGenBuffers  = (GL_GENBUFFERS)wglGetProcAddress("glGenBuffers");
// 现在函数可以被正常调用了
GLuint buffer;
glGenBuffers(1, &buffer);
```

你可以看到代码非常复杂，而且很繁琐，我们需要对每个可能使用的函数都要重复这个过程。幸运的是，有些库能简化此过程，其中**GLAD**是目前最新，也是最流行的库；

**进入`glad.dav1d.de/`网站，选择opengl3.3，core profile模式，之后下载zip文件就好啦，他会自动帮我们生成头文件和源文件，将头文件放入vs的include目录，将`glad.c`文件放入我们的项目目录下之后参与编译就好啦；**

## immediate mode 和 core profile

- Immediate mode 是 OpenGL 中一种较为古老的绘图方式。不需要复杂的设置和缓冲区管理，可以直接通过函数调用绘制图形。因为在每次调用绘图函数时，数据都要从 CPU 发送到 GPU，而且每个绘制命令都会立即被执行，没有经过优化的批处理过程，这会导致频繁的数据传输和绘图命令执行，对于复杂的场景和大量的几何数据，性能会比较差；
- Core - profile 开发者需要自己编写顶点着色器（Vertex Shader）和片段着色器（Fragment Shader）来控制图形的渲染过程。并且，需要通过创建缓冲区对象来存储顶点数据等，然后将这些缓冲区对象绑定到相应的着色器程序中进行绘制。通过使用缓冲区对象，可以将大量数据一次性发送到 GPU，减少数据传输的次数。着色器程序可以在 GPU 上并行处理图形数据，充分发挥 GPU 的并行计算能力，从而提升渲染性能，能够高效地处理复杂的 3D 场景和大量图形数据；

# 1. Chapter 1 Hello Window

opengl的使用模式一般如下所示：**OpenGL中的对象是代表OpenGL的状态子集；**

```cpp
struct object_name {
    float option1;
    int option2;
    char[] name;
};

// The State of OpenGL：这不就是策略模式嘛，用一个上下文对象进项实际操作，里面维护了一个基类成员变量指针来实现多态；
struct OpenGL_Context {
    ...
    object_name* object_Window_Target; // 上下文对象里面会有一个对象指针用于实现多态；
    ...
};

// create object
unsigned int objectId = 0;
glGenObject(1, &objectId);
// bind/assign object to context
glBindObject(GL_WINDOW_TARGET, objectId);
// set options of object currently bound to GL_WINDOW_TARGET
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_WIDTH, 800);
glSetObjectOption(GL_WINDOW_TARGET, GL_OPTION_WINDOW_HEIGHT, 600);
// set context target back to default
glBindObject(GL_WINDOW_TARGET, 0);
```

每一个对象都可以被当成是一个组件，多个组件设置号状态之后可以自由组合，从而可以组合多种状态而不用重复构建；这个代码的过程就像是，**上下文对象实际上是固定的，他就像是一些工具，之后我们会用他们来给对应的对象设置一些状态，**用就对应绑定，用完了就对应解绑，工具用完之后改变的状态就保存在了对象里面，但是工具仍然是那个工具；

坐标变换：opengl的`glViewport`函数会将被处理的点映射回我们的屏幕上，由于opengl的坐标区间为(-1, 1)，所以我们指定多大的视口，他就会被同比例映射到什么位置，比如：我们处理后的坐标点为(-0.5, 0,5)，我们设置的视口大小为 `glViewPort(0, 0, 800, 600);`，那么映射的规则就是将 `(-1, 1)->(0, 800)`，-1就对应0, 1就对应800，从而(-0.5, 0.5)映射的结果就是(200, 450)；

注册回调函数：opengl提供很多事件处理函数，我们可以注册实现，当对应的事件发生之后就会调用，**比如：改变窗口大小的回调，`framebuffer_size_callback(GLFWwindow* window, int width, int height)`这个函数名随便起什么都行，这里方便理解就定义为xxx_callback，之后通过`glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);`将回调函数注册进去；**

```c++
// 实现opengl提供的回调：
void framebuffer_size_callback(GLFWwindow* window, int width, int height){
    glViewPort(0, 0, width, height);
}

glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
```

**render loop：`glfwWindowShouldClose`在关闭窗口时返回false，`glfwPollEvents`函数检查是否触发了任何事件（如键盘输入或鼠标移动事件），更新窗口状态，并调用相应的函数（我们可以通过回调方法注册），`glfwSwapBuffer`函数用于交换一个2D颜色缓冲区，该缓冲区内维护了窗口的每一个像素的颜色值，从而将其渲染出来；**

```c++
while(glfwWindowShouldClose(GLFWwindow*)){ // 该函数用于关闭窗口时，返回false；
    glfwSwapBuffer(window);
    glfwPollEvents();
} 
```

双缓冲区：**当应用程序在单个缓冲区中绘制时，生成的图像可能会显示闪烁问题，这是因为生成的输出图像不是立即绘制的，而是逐像素绘制的，**通常是从左到右、从上到下绘制的，由于此图像在呈现时不会立即显示给用户，因此结果可能包含伪影，为了避免这些问题，窗口应用程序为呈现应用双缓冲区，**前缓冲区包含在屏幕上显示的最终输出图像，而所有呈现命令都绘制到后缓冲区，一旦所有渲染命令完成，我们就将后缓冲区交换到前缓冲区，**这样图像就可以在不被渲染的情况下显示；

window和viewport的关系：窗口实际上就是显示我们渲染的东西的界面，而视口是相对窗口而言的，将可以比窗口小，窗口其余的部分就不会显式对应的内容