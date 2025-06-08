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

# 1. Chapter 1 入门

## 1.1 Hello Window

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

window和viewport的关系：窗口实际上就是显示我们渲染的东西的界面，而视口是相对窗口而言的，将可以比窗口小，窗口其余的部分就不会显式对应的内容；

输入：通过使用GLFW的几个输入函数来完成，使用GLFW的glfwGetKey函数，**他需要一个窗口以及一个按键作为参数，这个函数将会返回这个按键是否正在被按下，如果被按下那么就返回GLFW_PRESS，否则返回GLFW_RELEASE；**

```cpp
if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
	glfwSetWindowShouldClose(window, true);
}
```

rendering：我们可以在render loop里面写入渲染的逻辑，之后每一次循环都执行同样的渲染内容，如：我们可以通过**调用glClear函数来清空屏幕的颜色缓冲，**通过传入一个buffer bits来指定我们想清空哪些buffer，我们可以**设置`GL_COLOR_BUFFER_BIT, GL_DEPTH_BUFFER_BIT, GL_STENCIL_BUFFER_BIT`；**

```cpp
glClearColor(0.2f, 0.2f, 0.3f, 1.0f); // 需要在glClear前调用，该函数表示在对颜色缓冲区调用glClear的时候，采用改颜色清屏，也就是渲染成该颜色；
glClear(GL_COLOR_BUFFER_BIT); // 清空缓冲区，我们这里指定为颜色缓冲区；
```

`glClearColor`函数 是一个**状态设置**函数，而 `glClear`函数则是一个**状态使用**的函数，他使用了当前的状态来获取应该清除为的颜色；

## 1.2 Hello Triangle

在学习此节之前，建议将这三个单词先记下来：

- 顶点数组对象：Vertex Array Object，VAO
- 顶点缓冲对象：Vertex Buffer Object，VBO
- 元素缓冲对象：Element Buffer Object，EBO 或 索引缓冲对象 Index Buffer Object，IBO

在OpenGL中，任何事物都在3D空间中，而屏幕和窗口却是2D像素数组，这导致OpenGL的大部分工作都是关于把3D坐标转变为适应你屏幕的2D像素。3D坐标转为2D坐标的处理过程是由OpenGL的图形渲染管线（Graphics Pipeline）执行（实际上指的是**一堆原始图形数据途经一个输送管道，期间经过各种变化处理最终出现在屏幕的过程）；**

### 1.2.1 graphics pipeline

图形渲染管线可以被划分为两个主要部分：**第一部分把你的3D坐标转换为2D坐标，第二部分是把2D坐标转变为实际的有颜色的像素；**2D坐标和像素也不同，**2D坐标在二维空间中表示一个明确的点，而像素收到屏幕/窗口的限制，是一个近似值；**

**图形渲染管线接受一组3D坐标，然后把它们转变为你屏幕上的有色2D像素输出，**图形渲染管线可以被划分为几个阶段，**每个阶段将会把前一个阶段的输出作为输入，所有这些阶段都是高度专门化的（它们都有一个特定的函数），**并且很容易**并行执行；**正是由于他们具有并行执行的特性，当今大多数显卡都有成千上万的小处理核心，他们在GPU上为每一个（渲染管线）阶段运行各自的小程序，从而在图形渲染管线中快速处理你的数据，**这些小程序叫做着色器(Shader)；**

有些着色器可以由开发者配置，因为允许用自己写的着色器来代替默认的，所以能够更细致地控制图形渲染管线中的特定部分了，**OpenGL着色器是用OpenGL着色器语言(OpenGL Shading Language, GLSL)写成的；**

下面就是图形渲染管线的各阶段：**其中蓝色的部分就是我们可以自定义的函数，**在现代OpenGL中，我们**必须定义至少一个顶点着色器和一个片段着色器（因为GPU中没有默认的顶点/片段着色器），**而**几何着色器通常使用他默认的着色器就行啦；**

![](F:\VS\jax-s_notebook\笔记图片\1749187709871.png)

图形管道包含大量的部分，每个部分处理将顶点数据转换为完全渲染像素的特定部分；作为图形管道的输入，我们**传递了一个包含三个3D坐标的列表，这些坐标应该在这里称为顶点数据的数组中形成一个三角形；**这个顶点的数据是用顶点属性表示的，可以包含任何我们想要的数据，但是为了简单起见，我们假设每个顶点只由一个3D位置和一些颜色值组成；

为了**让OpenGL知道我们的坐标和颜色值构成的到底是什么，**OpenGL需要你去指定这些数据所表示的渲染类型，我们是希望**把这些数据渲染成一系列的点？一系列的三角形？还是仅仅是一个长长的线？**做出的这些提示叫做**图元(Primitive)，**任何一个绘制指令的调用都将把图元传递给OpenGL：**`GL_POINTS, GL_TRIANGLES, GL_LINE_STRIP`；**

- 第一个阶段是 `vertex shader`：图形渲染管线的第一个部分是顶点着色器(Vertex Shader)，他**把一个单独的顶点作为输入，顶点着色器主要的目的是把3D坐标转为另一种3D坐标；**
- 第二个阶段是**图元装配(`Primitive Assembly`)阶段：**将顶点着色器（或几何着色器）输出的所有顶点作为输入（如果是GL_POINTS，那么就是一个顶点），并**将所有的点装配成指定图元的形状；**
- 第三个阶段为**几何着色阶段`geometry shader`：**顶点着色器阶段的输出可以**选择性地传递给几何着色器(Geometry Shader)，**几何着色器**将一组顶点作为输入，这些顶点形成图元，**并且能够通过发出新的顶点来形成新的(或其他)图元来生成其他形状，在这个例子中，他从给定的形状中生成第二个三角形；
- 第四个阶段为**光栅化阶段(Rasterization Stage)：**图元装配阶段的输出会被传入，这里他会**把图元映射为最终屏幕上相应的像素，**生成供片段着色器(Fragment Shader)使用的片段(Fragment)；在片段着色器运行之前会**执行裁切(Clipping)，丢弃超出你的视图以外的所有像素，**用来提升执行效率；**OpenGL中的一个片段是OpenGL渲染一个像素所需的所有数据；**
- 第五个阶段为片段着色器阶段：主要目的是**计算一个像素的最终颜色，**通常，**片段着色器包含3D场景的数据（比如光照、阴影、光的颜色等等），这些数据可以被用来计算最终像素的颜色；**
- 最后一个阶段叫做**Alpha测试和混合(Blending)阶段：这个阶段检测片段的对应的深度（和模板(Stencil)）值，**用他们来判断**这个像素是其他物体的前面还是后面，决定是否应该丢弃、。**这个阶段**也会检查alpha值（alpha值定义了一个物体的透明度）并对物体进行混合(Blend)，**所以，即使在片段着色器中计算出来了一个像素输出的颜色，在渲染多个三角形的时候最后的像素颜色也可能完全不同；

### 1.2.2 顶点输入（顶点缓冲区对象 VBO）

OpenGL不是简单地把**所有的**3D坐标变换为屏幕上的2D像素；**OpenGL仅当3D坐标在3个轴（x、y和z）上-1.0到1.0的范围内时才处理他，**所有在这个范围内的坐标叫做**标准化设备坐标(Normalized Device Coordinates)，**如果一个顶点的 x、y 或 z 坐标超出了这个范围（-1.0 到 1.0），那么他会被裁剪掉，相关的片段（像素）不会被渲染到屏幕上，而**-1.0 - 1.0被称为裁剪空间，裁剪空间的作用就是便于投影计算；**

**投影变换**：通过模型视图投影矩阵（Model - View - Projection Matrix，MVP 矩阵），可以将 3D 场景中的坐标转换到裁剪空间；例如，在透视投影prospective projection，远处的物体被映射到裁剪空间的边缘或外部，从而实现远小近大的视觉效果。

由于我们希望渲染一个三角形，我们一共要指定三个顶点，每个顶点都有一个3D位置。我们会将它们以标准化设备坐标的形式（OpenGL的可见区域）定义为一个`float`数组：

```cpp
float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};
```

如上所示，我们渲染的是一个2D三角形，我们将他顶点的z坐标设置为0.0，这样子的话三角形每一点的深度(Depth，**通常深度可以理解为z坐标，他代表一个像素在空间中和你的距离**)都是一样的，从而使他看上去像是2D的；**一旦你的顶点坐标已经在顶点着色器中处理过，**他们就应该是**标准化设备坐标啦，**如下所示：

<img src="F:\VS\jax-s_notebook\笔记图片\ndc.png" style="zoom: 67%;" />

通过使用由 **`glViewport`函数提供的数据，进行视口变换(Viewport Transform)，**标准化设备坐标(Normalized Device Coordinates)会变换为屏幕空间坐标(Screen-space Coordinates)；

定义这样的**顶点数据**以后，我们会把他**作为输入发送给图形渲染管线的第一个处理阶段：顶点着色器，**他会**在GPU上创建内存**用于存储我们的顶点数据，还要**配置OpenGL如何解释这些内存，并且指定其如何发送给显卡，**顶点着色器接着会处理我们在内存中指定数量的顶点；

通过**顶点缓冲对象(Vertex Buffer Objects, VBO)管理这个内存，**他会在GPU内存（通常被称为显存）中储存大量顶点。使用这些缓冲对象的好处是我们可以**一次性的发送一大批数据到显卡上，**而不是每个顶点发送一次，**从CPU把数据发送到显卡相对较慢；**

**顶点缓冲对象**是我们在OpenGL教程中第一个出现的OpenGL对象，就像OpenGL中的其他对象一样，这个**缓冲有一个独一无二的ID，**所以我们**可以使用 `glGenBuffers`函数生成一个带有缓冲ID的VBO对象，**OpenGL有很多缓冲对象类型，**顶点缓冲对象的缓冲类型是`GL_ARRAY_BUFFER`，**OpenGL允许我们同时绑定多个缓冲，只要他们是不同的缓冲类型：

```cpp
unsigned int VBO; // 作为缓冲对象的句柄id；
glGenBuffers(1, &VBO); // 生成对象，之后都通过这个句柄来访问；
glBindBuffer(GL_ARRAY_BUFFER, VBO); // 将该缓冲区对象绑定到上下文对象进行设置；
```

之后我们**使用任何在GL_ARRAY_BUFFER目标上的缓冲调用都会用来配置当前绑定的缓冲(VBO)；**此时的VBO还是一个空的顶点缓冲区对象，这时我们可以**调用glBufferData函数，他会把之前定义的顶点数据复制到缓冲的内存中：**

```cpp
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
```

glBufferData是一个专门用来把用户定义的数据复制到当前绑定缓冲的函数。它的第一个参数是目标缓冲的类型：顶点缓冲对象当前绑定到GL_ARRAY_BUFFER目标上。第二个参数指定传输数据的大小(以字节为单位)；用一个简单的`sizeof`计算出顶点数据大小就行。第三个参数是我们希望发送的实际数据。**第四个参数指定了我们希望显卡如何管理给定的数据。他有三种形式：**

- GL_STATIC_DRAW ：数据不会或几乎不会改变。
- GL_DYNAMIC_DRAW：数据会被改变很多。
- GL_STREAM_DRAW ：数据每次绘制时都会改变。

**三角形的位置数据不会改变，每次渲染调用时都保持原样，那么他的使用类型最好是GL_STATIC_DRAW，**如果一个**缓冲中的数据将频繁被改变，那么使用的类型就是GL_DYNAMIC_DRAW或GL_STREAM_DRAW，**这样就能确保**显卡把数据放在能够高速写入的内存部分；**

现在我们已经**把顶点数据储存在显卡的内存中，**用VBO这个顶点缓冲对象管理，下面我们会**创建一个顶点着色器和片段着色器**来真正处理这些数据；

### 1.2.3 顶点着色器vertex shader

#### 1.2.3.1 编写顶点着色器

我们需要做的第一件事是用着色器语言GLSL(OpenGL Shading Language)编写顶点着色器，然后编译这个着色器，这样我们就可以在程序中使用他了：

当前这个顶点着色器可能是我们能想到的最简单的顶点着色器了，因为我们对输入数据什么都没有处理就把它传到着色器的输出了。**在真实的程序里输入数据通常都不是标准化设备坐标，所以我们首先必须先把它们转换至OpenGL的可视区域内。**

```glsl
#version 330 core
layout (location = 0) in vec3 aPos; // location=0表示

void main()
{
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0); # 可以看到他是通过齐次坐标来表示的坐标点；
}
```

- 每个着色器都起始于一个版本声明，opengl3.3对应330，之后core表示为profile core模式；
- **使用`in`关键字，在顶点着色器中声明所有的输入顶点属性(Input Vertex Attribute)，这里只声明了一个顶点属性，就是三维坐标，**GLSL有一个向量数据类型，他包含**1到4个`float`分量，包含的数量可以从他的后缀数字看出来，**在GLSL中一个向量有最多4个分量，每个分量值都代表空间中的一个坐标，他们**可以通过`vec.x`、`vec.y`、`vec.z`和`vec.w`（注意`vec.w`分量不是用作表达空间中的位置的，而是齐次坐标表示点的一种方式）来获取，**并且**通过`layout (location = 0)`设定了输入变量（属性）的位置值(Location)，`layout` 关键字用于指定该属性在输入的缓冲区对象中的存储位置或布局，注意：这里设置的位置值只是我们自行设定而已，和字节对齐无关，他的作用就是在后面设置顶点属性的时候传入`glVertexAttribPointer`函数对应的属性位置的，从而让着色器知道哪个location的属性是对应的属性；**
- 为了设置顶点着色器的输出，我们必须把位置数据赋值给预定义的gl_Position变量，他在幕后是`vec4`类型的；在main函数的最后，我们将gl_Position设置的值会成为该顶点着色器的输出。由于我们的输入是一个3分量的向量，我们必须把它转换为4分量的。我们可以把`vec3`的数据作为`vec4`构造器的参数，同时由于我们表示的是一个点而不是向量，所以把`w`分量设置为`1.0f`；

#### 1.2.3.2 编译顶点着色器

现在，我们暂时将顶点着色器的源代码硬编码在代码文件顶部的C风格字符串中：

```c++
const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";
```

为了能够让OpenGL使用他，我们必须在运行时动态编译他的源代码。

我们首先要做的是创建一个着色器对象，注意还是用ID来引用的，所以我们储存这个顶点着色器为`unsigned int`，然后用glCreateShader创建这个着色器，但是这里不是通过上下文对象来存储当前的着色器对象，而是直接在每个函数中传入这个句柄，和前面的缓冲对象的使用方式有些区别：

```cpp
vertexShader = glCreateShader(GL_VERTEX_SHADER); // 创建着色器对象，传入创建的着色器类型，返回句柄ID；
glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr); // 把源码传入顶点着色器对象；
glCompileShader(vertexShader); // 编译源码；
```

`glShaderSource`函数把要编译的着色器对象作为第一个参数，第二参数指定了传递的源码字符串数量，这里只有一个，第三个参数是顶点着色器真正的源码；

你可能会希望检测在调用 `glCompileShader`后编译是否成功了，如果没成功的话，你还会希望知道错误是什么，这样你才能修复他们，**检测编译时错误**可以通过以下代码来实现：

```cpp
int  success;
char infoLog[512];
glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
```

首先我们定义一个整型变量来表示是否成功编译，还定义了一个储存错误消息（如果有的话）的容器。然后我们用 `glGetShaderiv`检查是否编译成功。如果编译失败，我们会用`glGetShaderInfoLog`获取错误消息，然后打印他：

```cpp
if(!success)
{
    glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
}
```

### 1.2.4 片段着色器fragment shader

片段着色器所做的是计算像素最后的颜色输出。为了让事情更简单，我们的片段着色器将会一直输出橘黄色。

在计算机图形中颜色被表示为有**4个元素的数组：红色、绿色、蓝色和alpha(透明度)分量，通常缩写为RGBA，**当在OpenGL或GLSL中定义一个颜色的时候，我们把颜色**每个分量的强度设置在0.0到1.0之间，**比如：说我们设置红为1.0f，绿为1.0f，我们会得到两个颜色的混合色，即黄色；

```cpp
#version 330 core
out vec4 FragColor
void main(){
    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}
```

**片段着色器只需要一个输出变量，**这个变量是一个**4分量向量，**他表示的是**最终的输出颜色，**我们应该自己将其计算出来，**声明输出变量可以使用`out`关键字：**

之后采用和定义以及编译 vertex_shader 同样的方式来定义以及编译fragment shader对象；

### 1.2.5 链接为着色器程序

着色器程序对象(Shader Program Object)是**多个着色器合并之后并最终链接完成的版本，**如果要使用刚才编译的着色器我们必须**把他们链接(Link)为一个着色器程序对象，**然后在**渲染对象的时候激活这个着色器程序，已激活着色器程序的着色器将在我们发送渲染调用的时候被使用，**当链接着色器至一个程序的时候，他会**把每个着色器的输出链接到下个着色器的输入，当输出和输入不匹配的时候，你会得到一个连接错误；**

创建以及链接着色器程序的方式和前面类似：

```cpp
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
```

### 1.2.6 链接顶点属性

顶点着色器允许我们指定任何形式的顶点属性输入，我们**必须手动指定输入数据的哪一个部分对应顶点着色器的哪一个顶点属性，**由于我们的例子**只有三维坐标，**所以顶点缓冲数据会被解析为下面这样子：

<img src="F:\VS\jax-s_notebook\笔记图片\vertex_attribute_pointer.png" style="zoom:80%;" />

- 位置数据被储存为32位（4字节）浮点值。
- 每个位置包含3个这样的值。
- 在这3个值之间没有空隙（或其他值）。这几个值在数组中紧密排列(Tightly Packed)。
- 数据中第一个值在缓冲开始的位置。

有了这些信息我们就可以**使用 `glVertexAttribPointer`函数 告诉OpenGL该如何解析顶点数据：**我们需要**对顶点的每一个输入属性都调用一遍该函数，但是我们这里只有一个输入，所以只需要调用一次就好啦；**

```cpp
getVertexAttribPointer(0, 3, GL_FLOAT, 3*sizeof(float), (void*)0);
```

- 第一个参数指定我们要配置的顶点属性是哪个location，还记得我们**在顶点着色器中使用`layout(location = 0)`定义了 `aPos`顶点属性，他可以把该顶点属性的位置值设置为`0`，因为我们希望把数据传递到这一个顶点属性中，所以这里我们传入`0`；**
- 第二个参数指定**顶点属性的大小，顶点属性是一个`vec3`，他由3个值组成，所以大小是3；**
- 第三个参数指定**数据的类型，**这里是GL_FLOAT(GLSL中`vec*`都是由浮点数值组成的)；
- 下个参数定义我们**是否希望数据被标准化(Normalize)；**如果我们**设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间，**我们把他设置为GL_FALSE，因为我们本身设置的就是-1，1之间标准化后的数据；
- 第五个参数叫做步长(Stride)，他告诉我们在连续的顶点属性组之间的间隔；由于下组aPos属性在3个`float`之后，我们把步长设置为`3 * sizeof(float)`；要注意的是由于我们知道这个**数组是紧密排列的（在两个顶点属性之间没有空隙）我们也可以设置为0来让OpenGL计算具体步长是多少（只有当数值是紧密排列时才可用，因为一个顶点的属性都是相同的，所以字节大小也相同，直接跳过一个顶点的全部属性表示的字节数就是下个顶点的该属性的起始字节位置）；**
- 最后一个参数的类型是`void*`，所以需要我们进行这个奇怪的强制类型转换，他表示**aPos属性在缓冲中起始位置的偏移量(Offset)，由于位置数据在数组的开头，所以这里是0；**

**每个顶点属性从一个VBO管理的内存中获得他的数据，而具体是从哪个VBO（程序中可以有多个VBO）获取则是通过在调用`glVertexAttribPointer`时绑定到 `GL_ARRAY_BUFFER`的VBO决定的，**由于在调用 `glVertexAttribPointer`之前绑定的是先前定义的VBO对象，顶点属性`0`现在会链接到他的顶点数据；

前面的操作是定义好了一个属性指针对象，也就是告诉OpenGL该如何解析顶点数据，我们现在应该**使用 `glEnableVertexAttribArray`函数，以每个顶点属性的location位置值作为参数，启用顶点属性，因为顶点属性默认是禁用的，如果不启动就无法使用，之后就可以渲染啦：**

```cpp
// 启用位置0的顶点属性；
glEnableVertexAttribArray(0);
```

但是又会产生一个问题，因为我们需要为每一个顶点的每一个属性都绑定上一个属性指针，那么此时就会很麻烦，如果有超过5个顶点属性，上百个不同物体就会很麻烦，有没有一些方法可以使我们**把所有这些状态配置储存在一个对象中，并且可以通过绑定这个对象来恢复状态呢？**

那就是通过顶点数组对象(Vertex Array Object, VAO)啦；

### 1.2.7 顶点数组对象 VAO

顶点数组对象(Vertex Array Object, VAO)可以像顶点缓冲对象那样被绑定，**任何随后的顶点属性调用都会储存在这个VAO中；**当配置顶点属性指针时，你只需要将那些调用执行一次，之后再绘制物体的时候只需要绑定相应的VAO就行了。这使**在不同顶点数据和属性配置之间切换变得非常简单，**只需要绑定不同的VAO就行了：

**注意：**OpenGL的核心模式**要求**我们使用VAO，所以他知道该如何处理我们的顶点输入，如果我们**绑定VAO失败，OpenGL会拒绝绘制任何东西；**

一个顶点数组对象会储存以下这些内容：

- glEnableVertexAttribArray和glDisableVertexAttribArray的调用。
- 通过glVertexAttribPointer设置的**顶点属性配置；**
- 通过glVertexAttribPointer调用与顶点属性关联的**顶点缓冲对象；**

如下图所示：

<img src="F:\VS\jax-s_notebook\笔记图片\vertex_array_objects.png" style="zoom: 80%;" />

代码流程如下所示：

```cpp
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

// render loop：
while (!glfwWindowShouldClose(window)) {
    ...
    // 绘制：
    start_shader_program(shader_program); // 启动着色器程序；
    glBindVertexArray(VAO); // 绑定VAO就好啦，因为vao里面有属性pointer，有vbo；
    glDrawArrays(GL_TRIANGLES, 0, 3);
    ...
}

// 释放对象；
glDeleteVertexArrays(1, &VAO);
glDeleteBuffers(1, &VBO);
glDeleteProgram(shader_program);
```

### 1.2.8 绘制图元

要想绘制我们想要的物体，OpenGL给我们**提供了glDrawArrays函数，**他使用当前激活的着色器，之前定义的顶点属性配置，和VBO的顶点数据（通过VAO间接绑定）来绘制图元：

```cpp
glDrawArrays(GL_TRIANGLES, 0, 3);
```

glDrawArrays函数**第一个参数是我们打算绘制的OpenGL图元的类型，这里传递GL_TRIANGLES表示画三角形；**第二个参数指定了**顶点数组的起始索引，**我们这里填`0`，最后一个参数指定我们打算**绘制多少个顶点，这里是`3`（我们只从我们的数据中渲染一个三角形，他只有3个顶点长）；**

### 1.2.9 元素缓冲对象

























# glsl教程

GLSL 是一种面向过程的编程语言，有着与 C 语言类似的语法，但没有 C 语言复杂的指针概念。 常用基本的类型如下：

类型	说明
void	空类型,即不返回任何值
bool	布尔类型 true,false
int	带符号的整数 signed integer
float	带符号的浮点数 floating scalar
vec2, vec3, vec4	n维浮点数向量 n-component floating point vector
bvec2, bvec3, bvec4	n维布尔向量 Boolean vector
ivec2, ivec3, ivec4	n维整数向量 signed integer vector
mat2, mat3, mat4	2x2, 3x3, 4x4 浮点数矩阵 float matrix
sampler2D	2D纹理 a 2D texture
samplerCube	盒纹理 cube mapped texture



gl_Position 、 gl_FragColor 等这些以 gl_ 开头的变量都是内置变量，通过给这些特殊的变量赋值，可以完成与硬件的通讯。









# Games101

齐次坐标：为了能够用一个矩阵来表示平移这种变化方式；

## model view transformation

orthgraphic Projection

![](F:\VS\jax-s_notebook\笔记图片\6c7882eaa384b1640af06b7125d18e5.jpg)



![](F:\VS\jax-s_notebook\笔记图片\9d6a1ff67698be2664966f4dd289894.jpg)

perspective Projection

<img src="F:\VS\jax-s_notebook\笔记图片\屏幕截图 2025-06-06 131052.png" style="zoom:50%;" />



视图变换view transformation，模型视图变换model view 移动相机到原点
3d->2d orthographic projection and perspective projection正交不会近大远小用于工程制图和透视投影用于人眼，正交投影就是从摄像机射出的所有投影的线都是平行的，而透视投影则是从一个点做切线进行投影

投影通过计算出观测矩阵（如何进行变换，就是一个齐次坐标矩阵）得到模型视图变换后的坐标，这个坐标就是标准坐标系下的坐标，也就是三个轴坐标范围在-1到1

mvp：model view projection model对应移动相机位置，view对应调整相机角度，projection对应将物体投影到-1，1

像素：pixel是picture element的简称，他是屏幕的最小单位，一个像素内部的颜色都是一致的；
rasterlization：光栅化在德语是屏幕的意思，所以就表示将投影后的图像画到屏幕上
屏幕坐标系：屏幕左下角是原点，向上为y正方向，向右为x正方向
opengl三维坐标系遵循左手系

视口变换：从3维-1到1的标准坐标系坐标放缩投射到2维屏幕上

显示在屏幕上的就是显存中的一块缓冲区数据：

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250608104633.png" style="zoom:50%;" />

光栅化最简单的方法：采样；采样的含义：sampling a function就是将1到100这些密集的点都输入到函数之后离散化为不同的结果

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250608104702.png" style="zoom:50%;" />

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250608104707.png" style="zoom:50%;" />

如何判断一个像素中点是否在三角形内部？用叉积；



