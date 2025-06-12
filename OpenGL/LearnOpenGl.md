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






## 1.3 着色器Shader
### 1.3.1 glsl
着色器是使用一种叫GLSL的类C语言写成的；着色器的开头总是要声明版本，接着是输入和输出变量、uniform和main函数。每个着色器的入口点都是main函数，在这个函数中我们处理所有的输入变量，并将结果输出到输出变量中；
一般着色器的结构如下所示：每个着色器使用这两个关键字设定输入和输出，只要**前一个着色器的输出变量与下一个着色器阶段的输入变量匹配，**他就会传递下去：
```glsl
#version number core
layout(location = 0) in type var1; # 前面的layout()可省略；
layout(location = 1) in type var2;
...
out type var3;

uniform var4;

void main(){
    ...
    var3 = xxx; # var3是输出变量；
}
```
我们也**可以忽略layout (location = 0)标识符，通过在OpenGL代码中使用`glGetAttribLocation`查询属性位置值(Location)，**但是会增加我们自己的工作量；

如果我们打算从一个着色器向另一个着色器发送数据，我们必须在发送方着色器中声明一个输出，在接收方着色器中声明一个类似的输入，当**类型和名字都一样**的时候，OpenGL就会**把两个变量链接到一起，**他们之间就能发送数据了（这是在链接程序对象时完成的）；

#### 1.3.1.1 数据类型
（1）基础类型：
类型	说明
void	空类型,即不返回任何值
bool	布尔类型 true,false
int	带符号的整数 signed integer
float	带符号的浮点数 floating scalar

（2）容器类型：
- 向量：
    - vecn	包含n个float分量的默认向量
    - bvecn	包含n个bool分量的向量
    - ivecn	包含n个int分量的向量
    - uvecn	包含n个unsigned int分量的向量
    - dvecn	包含n个double分量的向量

一个向量的分量可以通过vec.x这种方式获取，这里x是指这个向量的第一个分量，使用.x、.y、.z和.w来获取他们的第1、2、3、4个分量。GLSL也允许你对颜色使用rgba，或是对纹理坐标使用stpq访问相同的分量；`vec.x==vec.r==vec.s`都是获取向量的第一个分量；

向量这一数据类型也允许一些有趣而灵活的分量选择方式，叫做**重组(Swizzling)，**重组允许这样的语法：
```glsl
vec2 someVec;
vec4 differentVec = someVec.xyxx;
vec3 anotherVec = differentVec.zyw;
vec4 otherVec = someVec.xxxx + anotherVec.yxzy;
vec4 V = vec4(anotherVec, 1.0);
```
你可以**使用上面4个字母任意组合来创建一个和原来向量一样长的（同类型）新向量，**只要原来向量有那些分量即可，我们也可以**把一个向量作为一个参数传给不同的向量构造函数，**以减少需求参数的数量；


- 矩阵：
    - mat2, mat3, mat4	2x2, 3x3, 4x4 浮点数矩阵 float matrix

- 纹理：
    - sampler2D	2D纹理 a 2D texture
    - samplerCube	盒纹理 cube mapped texture

#### 1.3.1.2 uniform全局变量
Uniform是另一种从我们的**应用程序在 CPU 上传递数据到 GPU 上的着色器**的方式，但uniform和顶点属性有些不同；**首先，uniform是全局的(Global)，**他可以被着色器程序的任意着色器在任意阶段访问，并且每一个着色器访问的都是同一个地址的uniform变量；

其中 gl_Position 、 gl_FragColor 等这些**以 gl_ 开头的变量都是内置变量，通过给这些特殊的变量赋值，可以完成与硬件的通讯；**

下面我们用过uniform变量来实现通过代码来传入片段着色器颜色变量：
```glsl
#version 330 core
out vec4 FragColor; // 在代码中设置该变量的值；
uniform vec4 color;

void main(){
    FragColor = color;
}
```
如果**声明了一个uniform却在GLSL代码中没用过，编译器会静默移除这个变量，**导致最后编译出的版本中并不会包含他，这可能导致几个非常麻烦的错误，记住这点！
下面我们在代码中对这个uniform变量赋值：
```cpp
time_t t;
time(&t); // float t = glfwGetTime(); 或者替换为该函数获取程序运行的秒数；
float green_color = sin() / 2.0f + 0.5f; // 通过时间来改变绿色的颜色值；
int vertexColorLocation = glGetUniformLocation(shaderProgram, "vertexColor"); // 获取uniform变量的位置值；
glUseProgram(shaderProgram);
glUniform4f(vertexColorLocation, 0.0f, green_color, 0.0f, 1.0f); // 在代码中设置着色器中的uniform变量值；
```
我们首先用 `glGetUniformLocation` 查询 `uniform vertexColor` 的位置值，如果 `glGetUniformLocation` 返回-1就代表没有找到这个位置值，最后，我们可以通过 `glUniform4f` 函数设置vec4类型的 uniform变量值；

**注意：查询uniform地址不要求你之前使用过着色器程序，但是更新一个uniform之前你必须先使用程序(调用`glUseProgram`)，**因为他是在当前激活的着色器程序中设置uniform的；

opengl的函数有一个共同的特性，因为他是基于C开发的，所以他的函数并不支持重载，所以就通过一个特定的后缀，标识该函数处理变量的类型，比如glUniform4f，4f就表示设定的uniform的类型为4个float类型的变量（我们也可以使用fv版本，也就是vector向量），所有可能的后缀包括：
- f	函数需要一个float作为他的值
- i	函数需要一个int作为他的值
- ui	函数需要一个unsigned int作为他的值
- 3f	函数需要3个float作为他的值
- fv	函数需要一个float向量/数组作为他的值

### 1.3.1.3 为顶点引入更多属性
前面我们只学习了顶点存在一个vec3类型的坐标属性是如何在程序中链接顶点属性的，下面介绍如果设置多个属性；在这里我们通过为每个顶点引入坐标和颜色属性来举例：
```cpp
float vertices[]{
        // 位置              // 颜色
     0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
    -0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
     0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
}
```
下图为顶点数组的布局：
<img src="F:\VS\jax-s_notebook\笔记图片\vertex_attribute_pointer_interleaved.png" style="zoom:80%;" />
```glsl
// vertex shader
#version 330 core
layout(location=0) in vec3 aPos;
layout(location=1) in vec3 aColor;

out vec3 ourColor; // 输出到fragment shader

void main(){
    gl_Position = vec3(aPos, 1.0f);
    ourColor = aColor; // 将ourColor设置为我们从顶点数据那里得到的输入颜色
}

// fragment shader
#version 330 core
in vec3 ourColor;

out vec4 FragColor; // 输出到fragment shader

void main(){
    FragColor = vec4(ourColor, 1.0f); // 将ourColor设置为我们从顶点数据那里得到的输入颜色
}
```
下面就要链接起来各属性啦：
```cpp
// 坐标属性
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0); // 步长为6个float；
glEnableVertexAttribArray(0);
// 颜色属性
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)3*sizeof(float)); // 步长为6个float，并且起始偏移量为3*sizeof(float)；
glEnableVertexAttribArray(1); 
```
<img src="F:\VS\jax-s_notebook\笔记图片\shaders3.png" style="zoom:80%;" />
我们为三个顶点设置了三个颜色，但是渲染出来的效果并不是每一个顶点周围的颜色都是纯色的，这是在片段着色器中进行的所谓**片段插值(Fragment Interpolation)的结果；**也就是在光栅化的时候，最小单位是像素而不是顶点，所以一个三角形内有很多像素，那么这时候就通过插值计算的方式计算出处于顶点中间位置的像素他的颜色值，比如说，我们有一个线段，上面的顶点是绿色的，下面的顶点是蓝色的，如果一个片段着色器渲染一个位于线段70%位置的像素，他的颜色输入属性就会是30%蓝 + 70%绿；

## 1.4 纹理
如果想让图形看起来更真实，我们就必须有足够多的顶点，从而指定足够多的颜色，这将会产生很多额外开销，因为每个模型都会需求更多的顶点；
所以我们就可以通过使用纹理(texture)来解决这个问题，**纹理是一个2D图片（甚至也有1D和3D的纹理），**他可以用来添加物体的细节；除了图像以外，**纹理也可以被用来储存大量的数据，这些数据可以发送到着色器上；**

假设此时我们有一个纹理图片：
<img src="F:\VS\jax-s_notebook\笔记图片\wall.jpg" style="zoom: 60%;" />
为了能够**把纹理映射(Map)到三角形上，**我们需要**指定三角形的每个顶点各自对应纹理的哪个部分，**这样**每个顶点就会关联着一个纹理坐标(Texture Coordinate)，**用来标明该**从纹理图像的哪个部分采样；**

**纹理坐标在x和y轴上，范围为0到1之间（注意我们使用的是2D纹理图像），使用纹理坐标获取纹理颜色叫做采样(Sampling)，**纹理坐标起始于(0, 0)，也就是纹理图片的左下角，终止于(1, 1)，即纹理图片的右上角。下面的图片展示了我们是如何把纹理坐标映射到三角形上的：我们为三角形的三个顶点分别指定了1个纹理坐标点；
<img src="F:\VS\jax-s_notebook\笔记图片\tex_coords.png" style="zoom:80%;" />
我们希望三角形的左下角对应纹理的左下角，因此我们**把三角形左下角顶点的纹理坐标设置为(0, 0)；**同理右下方的顶点设置为(1, 0)；三角形的上顶点对应于图片的上中位置所以我们把它的纹理坐标设置为(0.5, 1.0)；纹理坐标看起来就像这样：

```cpp
float texCoords[] = {
    0.0f, 0.0f, // 左下角
    1.0f, 0.0f, // 右下角
    0.5f, 1.0f  // 上中
};
```

### 1.4.1 纹理环绕方式(Texture Wrapping)
纹理坐标的范围通常是从(0, 0)到(1, 1)，那如果我们把顶点的纹理坐标设置在范围之外会发生什么，OpenGL默认的行为是重复这个纹理图像（类似于对1取余），但OpenGL提供了更多的选择：
环绕方式	
- GL_REPEAT	对纹理的默认行为。重复纹理图像。
- GL_MIRRORED_REPEAT	和GL_REPEAT一样，但每次重复图片是镜像放置的。
- GL_CLAMP_TO_EDGE	纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
- GL_CLAMP_TO_BORDER	超出的坐标为用户指定的边缘颜色。
<img src="F:\VS\jax-s_notebook\笔记图片\texture_wrapping.png" style="zoom:80%;" />

以使用`glTexParameter*`函数对单独的一个坐标轴设置（s、t（如果是使用3D纹理那么还有一个r）他们和x、y、z是等价的）：
```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
```
- 第一个参数指定了**纹理目标：**我们使用的是2D纹理，因此纹理目标是GL_TEXTURE_2D；
- 第二个参数需要我们指定设置的**选项与应用的纹理轴：**我们打算配置的是WRAP选项，并且指定S和T轴；
- 最后一个参数需要我们传递一个**环绕方式(Wrapping)，**在这个例子中OpenGL会给当前激活的纹理设定纹理环绕方式为`GL_MIRRORED_REPEAT`；
如果我们选择 `GL_CLAMP_TO_BORDER`选项，我们还需要指定一个边缘的颜色，这需要**使用glTexParameter函数的fv后缀形式，**用GL_TEXTURE_BORDER_COLOR作为它的选项，并且传递一个float数组作为边缘的颜色值：
```cpp
float border[]{1.0f, 1.0f, 0.0f, 1.0f};
glTexParameterfv(GL_TEXTURE_2D, GL_GL_CLAMP_TO_BORDER, border);
```

### 1.4.2 纹理过滤(Texture Filtering)
纹理坐标**不依赖于分辨率(Resolution)，**他可以是任意浮点值，所以OpenGL需要知道**怎样将纹理像素(Texture Pixel，也叫Texel)映射到纹理坐标；**注意：我们不要把纹理坐标和纹理像素搞混，**纹理坐标是你给模型顶点设置的那个数组，OpenGL以这个顶点的纹理坐标数据去查找纹理图像上的像素，**然后进行采样提取纹理像素的颜色；

OpenGL也有对于纹理过滤(Texture Filtering)的选项。纹理过滤有很多个选项，但是现在我们只讨论最重要的两种：GL_NEAREST和GL_LINEAR；
- GL_NEAREST（也叫邻近过滤，Nearest Neighbor Filtering）是OpenGL默认的纹理过滤方式；当设置为GL_NEAREST的时候，**OpenGL会选择中心点最接近纹理坐标的那个像素；**下图中你可以看到四个像素，加号代表纹理坐标，左上角那个纹理像素的中心距离纹理坐标最近，所以他会被选择为样本颜色：
<img src="F:\VS\jax-s_notebook\笔记图片\filter_nearest.png" style="zoom:80%;" />
- GL_LINEAR（也叫线性过滤，(Bi)linear Filtering）他会**基于纹理坐标附近的纹理像素，计算出一个插值，近似出这些纹理像素之间的颜色；一个纹理像素的中心距离纹理坐标越近，那么这个纹理像素的颜色对最终的样本颜色的贡献越大；**如下图所示：
<img src="F:\VS\jax-s_notebook\笔记图片\filter_linear.png" style="zoom:80%;" />

那么这两种纹理过滤方式有怎样的视觉效果呢？让我们看看在一个很大的物体上应用一张低分辨率的纹理会发生什么吧：**GL_NEAREST产生了颗粒状的图案，我们能够清晰看到组成纹理的像素，而GL_LINEAR能够产生更平滑的图案，很难看出单个的纹理像素，GL_LINEAR可以产生更真实的输出；**
<img src="F:\VS\jax-s_notebook\笔记图片\texture_filtering.png" style="zoom:80%;" />
**当进行放大(Magnify)和缩小(Minify)操作的时候可以设置纹理过滤的选项：**比如你可以在纹理被缩小的时候使用邻近过滤，被放大时使用线性过滤。我们需要使用glTexParameter*函数为放大和缩小指定过滤方式；
```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

### 1.4.3 多级渐远纹理(Mipmaps)
想象一下，假设我们有一个包含着上千物体的大房间，每个物体上都有纹理。有些物体会很远，但其纹理会拥有与近处物体同样大小的分辨率，由于远处的物体很小，所以可能只占有很少的像素片段，OpenGL从高分辨率纹理中为这些片段获取正确的颜色值就很困难，因为他需要对一个跨过纹理很大部分的片段只拾取一个纹理颜色，在小物体上这会产生不真实的感觉；

OpenGL使用一种叫做**多级渐远纹理(Mipmap)**的概念来解决这个问题，他简单来说就是**一系列的纹理图像，后一个纹理图像是前一个的二分之一；**多级渐远纹理背后的理念很简单：**距观察者的距离超过一定的阈值，OpenGL会使用不同的多级渐远纹理，即最适合物体的距离的那个；**

让我们看一下多级渐远纹理是什么样子的：
<img src="F:\VS\jax-s_notebook\笔记图片\mipmaps.png" style="zoom:80%;" />
手工为每个纹理图像创建一系列多级渐远纹理很麻烦，而OpenGL有一个`glGenerateMipmap`函数，在创建完一个纹理后调用他，OpenGL就会承担接下来的所有工作啦；

在渲染中切换多级渐远纹理级别(Level)时，OpenGL在两个不同级别的多级渐远纹理层之间会产生不真实的生硬边界。你也可以在两个不同多级渐远纹理级别之间使用NEAREST和LINEAR过滤。为了指定不同多级渐远纹理级别之间的过滤方式，你可以使用下面四个选项中的一个代替原有的过滤方式：

过滤方式	描述
- GL_NEAREST_MIPMAP_NEAREST	使用最邻近的多级渐远纹理来匹配像素大小，并使用邻近插值进行纹理采样
- GL_LINEAR_MIPMAP_NEAREST	使用最邻近的多级渐远纹理级别，并使用线性插值进行采样
- GL_NEAREST_MIPMAP_LINEAR	在两个最匹配像素大小的多级渐远纹理之间进行线性插值，使用邻近插值进行采样
- GL_LINEAR_MIPMAP_LINEAR	在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样

一个常见的错误是，**将放大过滤的选项设置为多级渐远纹理过滤选项之一,这样没有任何效果，**因为多级渐远纹理主要是使用在纹理被缩小的情况下的：纹理放大不会使用多级渐远纹理，为放大过滤设置多级渐远纹理的选项会**产生一个GL_INVALID_ENUM错误代码；**

### 1.4.4 加载与创建纹理
使用纹理之前要做的第一件事是把它们加载到我们的应用中，纹理图像可能被储存为各种各样的文件格式，我们这里通过 stb_image.h 库来加载各种格式的图像：
```cpp
#define STB_IMAGE_IMPLEMENTATION // 通过定义该宏预处理器会修改头文件，让其只包含相关的函数定义源码;
#include "stb_image.h" // 宏定义一定要放在引入头文件之前；
// 加载图片
int width, height, nrChannels;
unsigned char *data = stbi_load("container.jpg", &width, &height, &nrChannels, 0);
```
纹理也是一个opengl对象，所以我们还是通过ID来表示一个纹理对象：
```cpp
unsigned int texture;
glGenTextures(1, &texture); // 第一个参数为生成的纹理数量；
```
之后就像其他对象一样，我们需要**绑定他到上下文，**让之后任何的纹理指令都可以配置当前绑定的纹理：
```cpp
glBindTexture(GL_TEXTURE_2D, texture);
```
之后就可以通过前面加载的图片生成一个纹理对象啦，纹理可以通过 `glTexImage2D` 来生成：
```cpp
glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
glGenerateMipmap(GL_TEXTURE_2D);
```
下面介绍一下glTexImage2D函数的几个参数：
- 第一个参数指定纹理目标类型，这里是2D纹理，所以会将图片加载到当前绑定到GL_TEXTURE_2D的纹理对象上，绑定到1D和3D的纹理对象不会受影响；
- 第二个参数是指定多级渐远纹理的级别，我们可以通过逐步+1该参数来手动生成不同级别的纹理图像，这里填0表示基本级别，后面我们直接通过glGenerateMipmap来自动生成多级渐远纹理；
- 第三个参数表示我们希望把纹理存储为何种格式，我们的图像只有RGB值，因此我们也把纹理储存为RGB值；
- 第四个和第五个参数设置最终的纹理的宽度和高度，我们使用图像对应的大小；
- 第六个参数一直为0，无意义，历史遗留问题；
- 七第八个参数定义了源图的格式和存储的数据类型，我们用unsigned char存储；
- 最后一个参数表示真正的图像数据；

当调用glTexImage2D时，当前绑定的纹理对象就会被附加上纹理图像。然而，目前只有基本级别(Base-level)的纹理图像被加载了，如果要使用多级渐远纹理，我们必须手动设置所有不同的图像（不断递增第二个参数）；或者，直接在生成纹理之后调用glGenerateMipmap。这会为当前绑定的纹理自动生成所有需要的多级渐远纹理；

用完图像对象之后，释放图像的内存：
```cpp
stbi_image_free(data);
```




















































# Games101

齐次坐标：为了能够用一个矩阵来表示平移这种变化方式；

总体流程：view->projection->view_port->rasterization

## 1 model view transformation

orthgraphic Projection

![](F:\VS\jax-s_notebook\笔记图片\6c7882eaa384b1640af06b7125d18e5.jpg)



![](F:\VS\jax-s_notebook\笔记图片\9d6a1ff67698be2664966f4dd289894.jpg)

perspective Projection

<img src="F:\VS\jax-s_notebook\笔记图片\屏幕截图 2025-06-06 131052.png" style="zoom:50%;" />



视图变换view transformation，模型视图变换model view 移动相机到原点
3d->2d orthographic projection and perspective projection正交不会近大远小用于工程制图和透视投影用于人眼，正交投影就是从摄像机射出的所有投影的线都是平行的，而透视投影则是从一个点做切线进行投影

投影通过计算出观测矩阵（如何进行变换，就是一个齐次坐标矩阵）得到模型视图变换后的坐标，这个坐标就是标准坐标系下的坐标，也就是三个轴坐标范围在-1到1

mvp：model view projection model对应移动相机位置，view对应调整相机角度，projection对应将物体投影到-1，1；

## 2 rasterization
像素：pixel是picture element的简称，他是屏幕的最小单位，一个像素内部的颜色都是一致的；
rasterlization光栅化定义：光栅化在德语是屏幕的意思，所以就表示将投影后的图像画到屏幕上

屏幕坐标系：屏幕左下角是原点，向上为y正方向，向右为x正方向
opengl三维坐标系遵循左手系

视口变换：从3维-1到1的标准坐标系坐标放缩投射到2维屏幕上

显示在屏幕上的就是显存中的一块缓冲区数据：

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250608104633.png" style="zoom:50%;" />


三角形：任何多边形都可以通过三角形进行表示，三角形是最简单的一个多边形，所以我们看到的任何图像都是通过一个个三角形进行表示的；


光栅化最简单的方法：采样；采样的含义：sampling a function就是将1到100这些密集的点都输入到函数之后离散化为不同的结果；例如图片的像素就是在连续的到达传感器的一些信号进行采样，将其离散化为一个个离散的像素，视频就是在时间维度进行采样，将其离散化为24个图片；
那么光栅化中的采样就是遍历屏幕中的像素点，看其中心坐标是否在三角形内部；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250608104702.png" style="zoom:50%;" />

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250608104707.png" style="zoom:50%;" />

如何判断一个像素中点是否在三角形内部？用叉积；
那一个屏幕像素很多，那我们如果遍历一整个屏幕的像素点判断其是否在三角形内部就会浪费很多时间，因为有很多像素他都离三角形很远，因此我们只需要遍历三角形的bounding box就好啦，也就是`min(x), max(x), min(y), min(y)` 这四个点组成的矩形；

锯齿(jaggies)：因为一个三角形的像素都是正方形，那么光栅化最大的问题就是走样问题aliasing，如下图所示，他在边界不够平滑；
<img src="F:\VS\jax-s_notebook\笔记图片\9643b2e8-85e2-4bf0-bdd9-7eda4ac1249a.png" style="zoom:50%;" />

### 2.1 反走样antialiasing（抗锯齿）：
sampling artifacts（采样瑕疵）：采样造成的瑕疵，下面是各种可能出现瑕疵的情况；
- 锯齿jaggies；sampling in space
- 摩尔纹：拿手机拍屏幕；undersampling images
- 车轮效应wagon wheel effect； sampling in time
原因：信号变化的速度太快，导致采样的速度跟不上；
解决方法：
（1）blur模糊：将三角形/信号先进行模糊化，之后再采样，这样在边界的锯齿就会好很多；
<img src="F:\VS\jax-s_notebook\笔记图片\d1a9d351-9e0d-43a9-9023-80dc99e1755a.png" style="zoom:50%;" />
注意：先做采样，再模糊化效果不好；

为了解释走样的原因，下面先学习一下和采样相关的数学知识：

傅立叶展开：任何函数都能转换为不同频率的`cos`相加来表示，如下图所示

<img src="F:\VS\jax-s_notebook\笔记图片\1749519711798.png" style="zoom:50%;" />

傅里叶变换：傅里叶变换就是利用傅里叶展开之后得到的f(x)来求积分，之后就也就将一个函数**从时域变到频域；**

<img src="F:\VS\jax-s_notebook\笔记图片\1749519903810.png" style="zoom:50%;" />

那么如下所示，有五个不同频率的函数，如果采用同样的采样频率进行采样，并对相邻采样点连线，发现当**信号频率很高之后就发生了走样；**

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250610093655.png" style="zoom:15%;" />

下面再了解一下滤波的概念：

（1）滤波的第一种方式就是去掉一定频率的信号；

假设此时有一个图像：**左侧为时域的图像，右侧为频域的信号；**

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250610094918.png" style="zoom:15%;" />

- 高通滤波：将低频信息抹去，剩下的就是图像中各物体的轮廓；我们可以发现，**高频为什么是图像的边界，**因为信号发生了突变；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250610094911.jpg" style="zoom:20%;" />

- 下面为低通滤波后的图像：可以看到很模糊，从而就是没有边界；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250610094926.png" style="zoom:15%;" />

（2）滤波的第二种方式可以理解为**卷积或平均；**

下面开始引入**采样：**采样可以理解为**重复频域上的内容；**

**时域上的乘积等于频域上做卷积，**采样就是**用另外一个冲击函数乘上原函数，**如左下图：左侧为函数在时域上的表示，右侧为函数在频域上的表示，**乘上冲击函数之后就相当于在频域上进行重复，那当采样频率低的时候，相邻重复的频率之间就会重叠所以会发生走样，如有下图所示：**

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250610094936.png" style="zoom: 14%;" /><img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250610094940.png" style="zoom:14%;" />

反走样（抗锯齿）：模糊之后就相当于**去掉高频的信号，**之后就可以发现采样之间重叠的频率没啦

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250610094945.png" style="zoom:15%;" />

实际操作中如何**通过低通滤波将三角形变模糊：**通过1 pixel大小的**卷积核对三角形图像进行卷积，**卷积的大小就是对**三角形图像在该像素内的覆盖面积，越大值越大，从而颜色越深，覆盖面积越小值越小，颜色越浅；**

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250610094949.png" style="zoom:15%;" />

如何**求三角形在一个像素内的覆盖面积：**通过**mssa多采样点的近似求解方法，**只是近似，对一个像素**再进一步划分为很多小的像素，**每一个小的像素有一个中心点，通过**判断每一个小像素是否在三角形内，**就可以求解出覆盖面积；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250610094953.jpg" style="zoom:13%;" /><img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250610094957.jpg" style="zoom:13%;" /><img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250610095001.png" style="zoom:%15;" />

其他的抗锯齿方法：fxaa（不是采样层面而是图像层面，光栅化之后通过图像匹配，找到锯齿的像素，之后将他替换）

超分和抗锯齿很类似：超分指的是分辨率很大，但是采样图像的时候像素比较低，如何还原图像就是超分，一般通过深度学习，说白了就是讲一个图像拉大，之后就会变糊全是锯齿如何变清晰就是超分要做的事情

### 2.2 深度缓存Z-Buffering

下面就进入下一个问题，对于前后物体遮挡如何光栅化，这方面主要涉及的概念就是可见性，深度（z轴表示的坐标）；

解决不同物体之间的深度关系的方法：

（1）painter algorithm：画家算法

先光栅化远的物体，之后逐步overwrite重叠的pixel；**先按照对象的深度进行排序，就能确定物体的远近关系，之后再光栅化；**

但**有些情况存在图形之间两两覆盖，**如下图所示，这种情况就**没办法指定图形之间的深度关系，所以画家算法基本上现在就已经不再使用啦；**

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250610095005.png" style="zoom:15%;" />

（2）Z-Buffering：

在实际渲染的时候，我们会有两个数组缓存：

- frame buffer：存储图像每一个像素的**颜色信息；**
- Z-Buffer：深度图，存储每一个**像素深度最浅的位置（深度就是图像到摄像机的距离，越近的就是深度越浅）；**

左侧的就是颜色图，右侧就是深度图，**越近颜色越深，越远颜色越浅；**

<img src="F:\VS\jax-s_notebook\笔记图片\1749521624613.png" style="zoom:25%;" />

`Z-Buffer Algorithm`：初始化的时候让每一个像素深度为无穷大，之后遍历每一个三角形内的像素点，查看该深度值是否比之前的当前位置的深度值小，如果小的话那么就同步更新深度图和颜色图，否则什么都不做；

<img src="F:\VS\jax-s_notebook\笔记图片\1749521885484.png" style="zoom:30%;" />

<img src="F:\VS\jax-s_notebook\笔记图片\1749522189289.png" style="zoom:50%;" />

在采用msaa进行抗锯齿的时候是将一个像素划分为多个像素，那么此时如果配合zbuffer一起使用的时候，就需要按照最小的采样点进行深度判断；

## 3 Shading着色

渲染一个图像主要包括三部分：**高光(specular highlights)，漫反射(diffuse reflection) 和 环境光照(ambient lighting)，**之后将这三部分都做出来就能做出一种材质；



<img src="F:\VS\jax-s_notebook\笔记图片\1749523210805.png" style="zoom:25%;" />

### 3.1 Diffuse Reflection

**三个问题：多少光到达shading point，多少光能够被shading point吸收，为什么反射会存在颜色**

shading point着色点：为一个点上色；

在一个物体局部的一个点，我们可以假设他附近为平面而不是曲面，因为范围足够小，之后再定义几个其他几个概念，这些方向向量（L光源方向，n法线方向，v观测方向）都是单位向量表示的，因为我们指向获取他的方向而不是大小；

<img src="F:\VS\jax-s_notebook\笔记图片\1749523413787.png" style="zoom:25%;" />

此外，**着色shading阶段不考虑阴影shadow，他只考虑shading point这一点自己，**后续的阶段再考虑遮挡带来的阴影；

如何考虑一个点的亮和暗，我们可以测试一下，旋转一个物体，对于同一个点，在不同旋转的阶段亮暗程度不同，下面解释一下原因：如下图所示，为了表示方便，我们假设光线是平行的，那么当**旋转一个立方体之后，可以发现图二打在立方体表面的光由6根变为了3根，**所以这就是变暗的原因，那么转换为数学表示就是，当**光源和物体表面一个点的法线夹角约小他越亮（可以通过余弦定理来计算出光能量在不同夹角时的大小）；**可以类比为地球自转，当中午的时候，太阳当头最热，因为太阳光和我们当前所在位置是垂直关系；

<img src="F:\VS\jax-s_notebook\笔记图片\1749523863243.png" style="zoom:33%;" />



**Light Falloff：**假设**在真空中光线传播，**此时有一个光源，他会向四周传播光并且随着**传播距离变远能量不会衰减**（在和光源同一半径距离的光大小相等，我们可以将其想象为一个球壳），那么由于**能量守恒定律，**距离光源近的球壳，和距离光源远的球壳**总体的光能量相同，**因此，我们可以发现，**越远的球壳上的点他的能量越小，从而越暗；**

`lambertian(diffuse) shading`漫模型：光漫反射到物体表面之后我们能够看到的能量就可以通过上面两个步骤来计算得出：通过**光源距离物体的远近以及光和物体表面的夹角计算；**

<img src="F:\VS\jax-s_notebook\笔记图片\1749524888146.png" style="zoom:50%;" />

我们看上面的公式中还存在一个**漫反射系数kd，他用于表示物体表面对光的吸收率，**由于不同物体对光的吸收率不同，吸收什么颜色他就呈现什么颜色，kd为1就表示什么光都不吸收全都反射出去，那么就为白色，kd为0表示什么光都吸收那么就为黑色，再进一步我们将其表示为一个向量，那么**kd就是`rgb`；**

那么由于**漫反射指的是光打在物体表面之后会向四面八方发生发射，**所以只要是**光源位置不变，物体位置不变，无论我们从哪里看，看到物体表面的颜色和亮度都是一样的，**所以看到上面的漫反射公式`Ld`**和向量v的角度无关；**

### 3.2 Specular Term

前面我们知道物体**漫反射diffuse reflection**后的颜色，亮度 **和 视角v方向无关，**那么v到底起到什么作用？

**就是高光，**当我们v的方向和**镜面反射**的方向（在下图中没体现镜面反射方向）越近的时候，他就会发生高光现象；那么我们只需要对**入射角L和出射角v计算角平分线，就能够得到半程向量的方向h，**如果**h和发现n越接近就表示越容易看到高光；**为什么通过半程向量和法线来判断是否出现高光，而不用镜面反射和v计算呢，因为前者更简单；

<img src="F:\VS\jax-s_notebook\笔记图片\1749527039843.png" style="zoom:33%;" />

可以看到Ls的max存在一个指数p，因为如果**不加指数，那么当夹角为45度的时候，cosa都很大，所以仍然能看到高光，**而我们知道现实生活中偏差到了45度就很难看到高光，所以通过对**cosa加上指数p，来让其变化更抖，从而在45度，值会得很小，**因为cosa小于1，所以p次方之后会变小很多；

### 3.3 Ambient Term

**环境光**和法线以及观测角度无关，所以他就是一个**常量，**当然这只是简化之后的结果

<img src="F:\VS\jax-s_notebook\笔记图片\1749528433383.png" style="zoom:33%;" />



将**三项加一起**就得到了Blinn-Phong着色模型的着色结果：那么对物体所有的点都进行同样的操作就得到了渲染后的结果；

<img src="F:\VS\jax-s_notebook\笔记图片\1749528987065.png" style="zoom:33%;" />

### 3.4 shading frequencies着色频率

（1）平面着色flat shading

第一种方法，**每一个三角形都作为一个平面，计算出法线，之后对整个三角形进行着色；**

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611105744.png" style="zoom:15%;" />

（2）顶点着色vertex shading

第二种方法，**对每一个顶点求法线，之后三角形内部通过插值的方式进行着色，**效果比第一种好，但是右侧的图高光就看不出来啦；

如何求一个顶点的法线？就是**通过对其周围的面的法线求平均；**

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611110023.png" style="zoom:15%;" />

（3）像素着色phong shading

第三种方法就是**对每一个三角形内部的像素计算法线进行着色，**我们要区分前面讲过的`blinn-phong`着色模型，这里讲的全部都是着色频率的方法，而不是着色模型；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611105934.jpg" style="zoom:20%;" />

前面讲到着色频率是针对以面/点/像素为最小单位进行着色，而当我们的图形中已经存在足够多的顶点时，用flat shading的方式就已经能渲染出足够好的结果啦，因为一个三角形足够小；

那**逐像素的法线如何求解？**如下图所示，假设我们已经计算出两个顶点的法线，那么如何得到中间平滑过渡的法线呢？通过插值的方法，通过重心坐标；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611110108.png" style="zoom:15%;" />

还有一点就是**法线表示的只是方向，所以还需要对所有方向的法线进行归一化；**

### 3.5 Graphics Pipline图形管线

图形管线又可以被称为**实时渲染；**

图形管线Graphics Pipeline：输入在三维空间的顶点坐标，之后将其转换为在屏幕上的坐标，然后将其连接成为三角形，之后由于屏幕空间是离散的，也就是他是由一个个的像素组成的，所以就需要对三角形进行光栅化将三角形转换为像素表示，这里的一个方块就是一个像素，fragment其实就可以理解为一个像素，但是他除了能够表示像素之后还带有该像素的属性信息，如果通过msaa的方式来进行光栅化，那么一个小像素就表示一个fragment，三维到二维屏幕是通过投影实现的；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611110113.png" style="zoom:15%;" />

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611110339.jpg" style="zoom:25%;" />

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611110347.jpg" style="zoom:25%;" />

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611110351.jpg" style="zoom:25%;" />

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611110356.jpg" style="zoom:25%;" />

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611110402.png" style="zoom:15%;" />

我们可以看到shading可以发生在两个阶段，原因就是如果**采用的是phong shading的话就发生在fragment processing阶段，**因为他是对每一个像素计算法线，如果**采用的是vertex shading的话，那么就发生在vertex processing阶段；**

vertex shader是顶点着色器，fragment shader是像素着色器，此外，我们**通过glsl语言写的着色器程序在上色的时候是针对所有顶点/像素采用同样的操作，而不需要我们自己在里面通过for循环遍历所有的点/像素进行上色，**我们只需要考虑他如何处理一个顶点/像素，之后他会自己应用到所有的顶点/像素；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611110751.png" style="zoom:15%;" />

这段fragment shader代码展示的就是一个简易版本的phong着色器的实现；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611111741.png" style="zoom:15%;" />

### 3.6 texture mapping纹理映射

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611113043.png" style="zoom:15%;" />

如上图所示，为什么**着色器相同，但是不同的点他的颜色不同？**因为他们的**材质不同，所以导致他们的漫反射系数kd不同；**

一个3d图形的表面实际上就是一个2d的图像，而**纹理我们就可以理解为一个2d的图像；**

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611113136.png" style="zoom:15%;" />

**纹理映射就是将纹理图像中的点映射到三角形中的一个点，**并且尽可能地少扭曲，那么就很完美了，**纹理坐标和三角形顶点坐标之间的映射关系这是由美工做的，**在我们看来，在使用的时候就已经得到了每一个三角形顶点的纹理坐标啦；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611113237.png" style="zoom:15%;" />

**纹理坐标的范围是`(0, 1)`，**不管他的分辨率或者长宽比是多少；

**纹理可以被重复使用，**好的纹理可以自己无缝衔接，也就是自己的右侧可以衔接上自己的左侧；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611113326.png" style="zoom:15%;" />

重心坐标`Barycentric Coordinates`：**重心坐标是一个三角形内部的坐标点的另外一种表示方式，**不管该点处于什么坐标系，他的重心坐标都可以表示为和三角形三个顶点的连线距离乘以三个系数，(a, b, r)，而这三个系数就是重心坐标；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611113953.png" style="zoom:15%;" />

此外**三个系数相加等于1，并且这个系数等于该顶点相对的三角形面积除以三角形总面积；重心的重心坐标就是(1/3, 1/3, 1/3)；**

<img src="F:\VS\jax-s_notebook\笔记图片\屏幕截图 2025-06-10 221251.png" style="zoom:33%;" />

**三角形内部的插值（interpolation across triangle）：**就是通过三角形内部的点的重心坐标来求解出该点对应的更加准确的属性值；

为什么要进行插值，我们要插值什么内容？

有很多情况需要计算插值，比如：我们知道了**顶点的属性，想要求解顶点内部像素的一些属性的值，**那么此时就需要通过插值来计算，比如，**phong着色方法，**就是对像素内部计算法线，计算的方法就是对三角形顶点先计算出法线，之后**采用插值的方式求解出顶点之间的法线方向，**或者是在**纹理映射**的时候，我们会指定图像顶点和贴图顶点之间的映射关系，那么**三角形顶点内部和纹理之间的关系就是通过插值计算出来的；**

**（1）linear interplation线性插值：**

那么**线性插值就是利用了重心坐标，**他的思想就是对于三角形内部的任意一个点，都可以通过重心坐标乘上三个顶点的属性得到插值后的值；

<img src="F:\VS\jax-s_notebook\笔记图片\屏幕截图 2025-06-10 221543.png" style="zoom:33%;" />

**注意：这种通过重心坐标计算插值的方法不能在投影之后计算，因为重心坐标在投影之后会发生改变，**比如在3维空间，我们需要**先在3维空间中计算出重心坐标做插值，然后再进行投影到对应的2维结果； **

**重心坐标在纹理映射中的应用：三角形顶点内部的任何一个像素(x, y)可以通过插值的方式来得到他所对应的纹理坐标(u, v)，就是通过重心坐标乘上三个顶点的纹理坐标并相加，**之后得到的加权值就是他的纹理坐标，然后通过这个纹理坐标到纹理图像中去看该坐标位置的颜色值，他就可以作为漫反射系数kd代入blinn-phong着色模型中进行着色，然后就可以得到着色后的结果；

<img src="F:\VS\jax-s_notebook\笔记图片\屏幕截图 2025-06-10 223151.png" style="zoom:50%;" />

（2）bilinear interplation双线性插值：这里的双其实是两步插值，而不是两次线性插值；

假如有一个**很高清的图像1920x1080，但是纹理却很低分辨率800x600，**那此时再进行纹理映射的时候，如果只**通过线性插值计算三角形内每个像素的纹理坐标uv的话，就很容易让相邻像素都映射到同一个纹理坐标**（由于虽然求出的纹理坐标不同都是float值，但是由于纹理分辨率太小所有都映射到同一个纹理像素内了，那么此时就会按照最接近的texel的颜色进行映射），从而导致相邻的**图像像素（pixel）就都是同一个纹理像素（texel）**的颜色，看起来就比较糊；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611113958.png" style="zoom:15%;" />

那么此时就可以通过双线性插值，**该方法通过执行三次 linear interplation来实现更细节的插值方法：**就是先将原图像的点映射得到纹理坐标，之后其周围最近的4个像素，将像素中心点连接成一个矩形，然后先计算红点（纹理坐标点）正上方对应矩形上边两点的线性插值结果，下边同理，之后得到插值后的两个点，最后对这两个点在计算红点的线性插值结果从而得到双线性插值的结果，该插值按照距离作为权重考虑了周围4个像素的属性，有效解决原图多个像素映射到纹理图同一点的问题；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611114007.png" style="zoom:15%;" /><img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611114023.png" style="zoom:15%;" />

相反，在**纹理分辨率远大于图像的分辨率的时候，反而会引起更严重的问题，**就是走样：摩尔纹，如下图所示，**近距离的点映射后的效果还可以，但是远距离出现了走样的现象；**

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611115133.png" style="zoom:15%;" />

因为**对于远处的点而言，由于透视投影导致的近大远小，远处的一个点实际上应该对应纹理很大一块面积（比如：10个像素），**如果**还按照单个像素（线性插值）或四个像素（双线性插值）进行映射**的话，那么就会造成**远处的一个像素只插值了很小一部分纹理图像，丢失了很多纹理图像的像素信息，所以就会走样；**用前面采样的角度来说，就是**一个像素内的频率其实很高（包含很大面积的纹理像素），但是如果我们只采样一个点所在的像素来表示这一整块纹理图像的话，就会出现走样的问题；**

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250611115137.png" style="zoom:15%;" />

解决方法一：可以同样采用**msaa，将原图像中的一个像素划分多个小像素进行纹理映射，**但是时间复杂度太大；

解决方法二：**通过mipmap范围查询，**直接**获取一个原图像素映射的全部纹理像素的平均值**（注意：不同远近的原图像素他所对应的纹理像素的大小是不同的，越远他对应的面积越大）

**mipmap特点：速度快（可以直接查询），仅支持近似正方形的查询；**

在渲染之前，先将一个纹理图片**生成不同分辨率的纹理图像，每级都缩小一半他的分辨率，**如下图所示：为了方便看，虽然每个图的分辨率不同，我们都将他放到一样大，实际上应该是面积一半的关系；

<img src="F:\VS\jax-s_notebook\笔记图片\1749616153046.png" style="zoom:33%;" />

我们再回到刚才的问题，如果如何将图片中表示的一个远距离的像素点映射到纹理图像的一大块面积上？**通过mipmap查询的方法，我们需要先计算得出该点映射到纹理图像的面积，之后通过该面积查询到对应等级的纹理图像进行映射；**

第一步：我们需要先**计算得出原图像中的一个点他对应的纹理图像的面积到底是多大：**如下面所示，第一张图**左下角箭头指向的红点**是我们实际要纹理映射的点，那么我们就**将其相邻的顶点采用插值的方法都映射到纹理图像，**之后**计算任意两点在纹理图像中的距离，**如第二张图所示，我们这里使用的是左下角红点和右下角红点之间的距离**作为边长L（一般会选择距离最远的作为边长），**之后看第三张图，我们再**按照刚才计算得出的L来作为正方形的边长，以左下角的红点为中心计算面积，**这个就是**该点映射到纹理图像中的面积；**

<img src="F:\VS\jax-s_notebook\笔记图片\1749616945748.png" style="zoom:25%;" /><img src="F:\VS\jax-s_notebook\笔记图片\1749616971450.png" style="zoom:25%;" />



<img src="F:\VS\jax-s_notebook\笔记图片\1749616989540.png" style="zoom:25%;" />

第二步：因为**不同等级的mipmaps实际上是面积（分辨率）缩减一半，**level越大面积越小，那么这时候查询的原理就是，我们**想要找到该正方形只映射到一个像素大小的mipmaps纹理图像层，之后就可以直接映射到该层纹理图像的一个像素，**所以**计算出的层 `level D=log2(L)`；**

虽然这种方法已经在很大程度上缓解了映射速度的问题，但是**对于那些分界为不同层的纹理图像的原图像的像素点而言，近一点的那个点就需要映射回第0层的像素，他相邻的远一点的点就需要直接映射到第1层的像素，**这样仍然会**产生一道大缝，**那么我们就会想，**有没有0.9层呢，**如果让近一点的那个点**映射到0.9层而不是0层**的话是不是就好很多了呢，此时就又用到了**插值的思想：**

**Trilinear interpolation：**先对原图像顶点映射到相邻两层mipmap纹理图计算双线性插值，之后对这两层的双线性插值结果在计算出一次插值（第三次插值）；

<img src="F:\VS\jax-s_notebook\笔记图片\1749618454139.png" style="zoom:25%;" />

但是mipmaps仍然存在一个问题，由于mipmaps**只能按照近似正方形**来计算原图一个点映射到纹理图的面积，那么就是**在面对原图像映射到纹理图的面积形状为和正方形相差很大的长条形状的图形时，**就会导致插值得到的效果不好，如下图所示，如果按照mipmaps的方法计算正方形面积，由于是按照相邻顶点最长的距离作为边长，就会导致计算得到的面积过大，从而导致overblurred，过于模糊的问题；

<img src="F:\VS\jax-s_notebook\笔记图片\1749618903192.png" style="zoom:25%;" />

`anisotropic filtering`（各向异性过滤）会**额外生成纵向边长一半和横向边长一半的纹理层，**就是可以用于解决长条型状的面积问题，但是这种方法只能解决纵向或者横向摆放的矩形，对于上图左上角的斜着的矩形仍然效果不好；该方法需要设置一个x，x表示生成多少层，就是n X n的一圈；

<img src="F:\VS\jax-s_notebook\笔记图片\1749619660747.png" style="zoom: 50%;" />

`EWA filtering`可以解决这种问题，但是需要多次查询，所以时间复杂度会变大；

### 3.7 texture applicaiton纹理应用

纹理其实可以理解为存储在GPU显存里的一块缓冲区，他支持点映射查询，mipmaps图像查询，实际上里面存储的可以是任意内容，基于此我们就可以用这个特性去存储很多其他内容；

<img src="F:\VS\jax-s_notebook\笔记图片\1749620553078.png" style="zoom:25%;" />

- 环境光：可以看到茶壶上有环境光的倒影；

<img src="F:\VS\jax-s_notebook\笔记图片\1749620710754.png" style="zoom:25%;" />

我们可以把反射的环境光记录在球体上，之后在使用的时候将其展开；

  <img src="F:\VS\jax-s_notebook\笔记图片\1749621009636.png" style="zoom: 33%;" />

但是当展开的时候会有一些扭曲，因为球体本身形状的特性，我们可以记录在一个立方体cube map；

- 凹凸/纹理贴图`bump mapping`：纹理不仅能存储颜色，还可以用于**存储凹凸贴图，也就是指存储原图中每个像素高度的相对移动距离，**通过他能够**调整原图像像素的相对高度，从而达到改变该点法线的效果，**而在blinn-phong着色模型中，法线改变之后着色就会发生改变，从而达到只通过纹理贴图来改变；

<img src="F:\VS\jax-s_notebook\笔记图片\1749629702084.png" style="zoom:25%;" /><img src="F:\VS\jax-s_notebook\笔记图片\1749630229110.png" style="zoom:25%;" />

我们先看在**2维平面**如何计算凹凸贴图后的**法线：**该图为在平面如何计算法线normal，dp为高，1为宽，之后计算出**切线表示的向量为(dp, 1)，由于法线和切线是垂直关系，那么法线向量就是(-dp, 1)，**之后再进行**标准化**变为单位向量就可以啦；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250612093320.png" style="zoom:33%;" />

对于**三维场景**而言，计算**凹凸贴图后的法线：**的过程如下所示：

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250612093626.png" style="zoom:25%;" />

- 位移贴图displacement mapping：他和凹凸贴图唯一的区别就是凹凸贴图不会真正地移动顶点的位置（只通过凹凸贴图改变顶点相对高度计算法线，欺骗人们的眼睛），而**位移贴图是真正地移动顶点位置**之后再着色，效果更真实；但是**位移贴图要求模型的分辨率或者可以称为是采样率足够高，**只有这样才能覆盖着法线贴图给出的高度，避免法线贴图的频率大于模型的频率而导致的走样发生；

  <img src="F:\VS\jax-s_notebook\笔记图片\1749692535022.png" style="zoom:33%;" />

动态细分：为了避免一开始先将模型设置过多的点导致细分过多的点而引出了动态细分，就是当逐步位移贴图的时候**当不够用了再细分三角形增加顶点来使模型更真实，后面讲几何的时候会细说；**

- 三维纹理：通过噪声函数来实现；

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250612094104.png" style="zoom:15%;" />

## 4 Geometry几何

<img src="F:\VS\jax-s_notebook\笔记图片\1749692612178.png" style="zoom:25%;" />

表示几何的方式主要分为两大类，隐式和显式表示；

隐式表示就是**通过公式**的方式去表示一个图形，比如：x^2+y^2+z^2=1就是一个半径为1的球；隐式**比较容易确定 xyz 点是否在表面，**因为直接带入公式成立即可，但是**很难采样，也就是很难枚举，很难确定该公式表示的是什么图形；**

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250612094426.png" style="zoom:15%;" />

显式表示就是**要么直接给出所有的点，要么就是通过映射的方式给出点，也就是通过采样的方式**直接通过无数的点来表示出一个图形，**很容易采样，很容易看出该图形是什么形状，但是很难确定一个xyz坐标表示的点是否在图形表面；**

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250612094419.png" style="zoom:15%;" />

<img src="F:\VS\jax-s_notebook\笔记图片\微信图片_20250612094429.png" style="zoom:15%;" />

### 4.1 隐式表示方式

特点：对于**复杂图形很难用隐式表示，**因为一个规则的公式无法表示复杂图形所有的位置；

<img src="F:\VS\jax-s_notebook\笔记图片\1749697962665.png" style="zoom:25%;" />

（1）代数表示：

<img src="F:\VS\jax-s_notebook\笔记图片\1749693122373.png" style="zoom:33%;" />

（2）布尔运算：多个饮食表示的图形之间可以进行集合运算；

<img src="F:\VS\jax-s_notebook\笔记图片\1749693227563.png" style="zoom:33%;" />

（3）混合距离函数Blend Distance Function：

<img src="F:\VS\jax-s_notebook\笔记图片\1749696584511.png" style="zoom:25%;" /><img src="F:\VS\jax-s_notebook\笔记图片\1749696528001.png" style="zoom:25%;" />

得到混合距离函数之后就可以**根据混合距离函数值为0的点画出物体的表面；**

（4）水平集level set：思想和距离函数完全一致，只不过表示方式是通过格子的值来表示的，找到等于0的位置就是物体的表面，右下角为水平集在三维的应用（和纹理结合），常用于医学找到密度相同的位置标红；

<img src="F:\VS\jax-s_notebook\笔记图片\1749697516054.png" style="zoom:25%;" /><img src="F:\VS\jax-s_notebook\笔记图片\1749697647333.png" style="zoom:25%;" />

（5）分型自相似fractals（类似于递归）：

<img src="F:\VS\jax-s_notebook\笔记图片\1749697803351.png" style="zoom:25%;" />

### 4.2 显式表示方式

（1）point clound点云：需要特别密集的点来表示几何图形，表征方式很简单，经常被转换为不同的多边形面`polygon mesh`；

（2）polygon mesh多边形面：应用最为广泛，通过**三角形或四边形**表示各种各样复杂的物体，下面是**表示单个物体/场景的文件：是一个文本文件；**

<img src="F:\VS\jax-s_notebook\笔记图片\1749700070312.png" style="zoom: 67%;" />

**左上角v开头的行表示单个顶点的坐标，vt开头表示纹理坐标（每个面都有一组纹理坐标，图中有重复），vn表示面的法线（每个面都有一个法线向量，图中有重复），f开头表示三个顶点连接成一个三角形，**如：`f 5/1/1 1/2/1 4/3/1` ，**`5/1/1`表示第五个顶点，对应第一个纹理坐标，第一个法线，这是作为一个三角形的一个顶点，**1/2/1为三角形的第二个顶点，4/3/1为第三个顶点，三个顶点连线组成一个三角形；

### 4.3 曲线Curve

应用场景：作为摄像机/模型移动的预设路线，定义一些字体；

<img src="F:\VS\jax-s_notebook\笔记图片\1749700641353.png" style="zoom:25%;" />

#### 4.3.1 Bezier Curve贝塞尔曲线

**一系列控制点能确定一个唯一的平滑曲线，**该曲线需要满足的性质如下：

- 曲线**起始位置为p0，结束位置为p3，**其他位置不要求曲线经过；

- **起始的切线方向为 `p1 - p0`，**曲线**结束的切线方向为 `p3 - p2`；**

<img src="F:\VS\jax-s_notebook\笔记图片\1749701023779.png" style="zoom:33%;" />

上例为四个点确立一条贝塞尔曲线，那么如何通过任意多的点来确立：由于两个点如果符合上述性质，那么只能是一条直线，所以从三个点开始介绍：

- 三个点：在两条边中找到当**处于时间t时，从起始端点开始出发向结束端点移动的位置，**如下图，b0 - b1这条线段从b0开始移动，经过时间t之后到达了b0^1的位置，b1 - b2这条线段从b1开始移动，经过时间t之后到达了b1^1的位置，类似于对两条边分别做线性插值找到对应位置的点，之后连线b0^1和b1^1两个点，之后采用同样的方式在b0^1 - b1^1这条线段上从b0^1开始移动时间t之后到达的位置点b0^2，那么最后得到的一个点 b0^2 就是贝塞尔曲线在时间t的位置，之后枚举t就可以画出任意时刻贝塞尔曲线所在的位置，那么就确定了唯一一条曲线；

<img src="F:\VS\jax-s_notebook\笔记图片\1749701577334.png" style="zoom:25%;" /><img src="F:\VS\jax-s_notebook\笔记图片\1749701869134.png" style="zoom:25%;" /><img src="F:\VS\jax-s_notebook\笔记图片\1749702125909.png" style="zoom:23%;" />

- 四个点：

<img src="F:\VS\jax-s_notebook\笔记图片\1749702161078.png" style="zoom:25%;" />

总结为代数公式：伯恩斯坦多项式

<img src="F:\VS\jax-s_notebook\笔记图片\1749704500056.png" style="zoom:25%;" />

贝塞尔曲线的性质：

<img src="F:\VS\jax-s_notebook\笔记图片\1749704718044.png" style="zoom:25%;" />

最后一条为凸包性质：凸包指的是给定的控制点最外圈围成的一个多边形，我们可以将其想象成一个木板上钉满了钉子，之后用个橡皮筋拉大无限大扩到所有的钉子之外，之后松手，橡皮筋围成的多边形就是凸包，贝塞尔曲线保证会在控制点形成的凸包以内：

<img src="F:\VS\jax-s_notebook\笔记图片\1749705764400.png" style="zoom:25%;" />

#### 4.3.2 Piecewise Curve分段贝塞尔曲线

**将多个控制点划分为段，四个点为一段：**如下图所示，将七个控制点分为两段，其中左侧的四个绿色的为第一段，右侧四个黄色的为第二段，其中中间的那个为公用的点；但是**为了使多段的贝塞尔曲线能够连接为一个平滑的贝塞尔曲线（满足c1连续性，一阶导数连续，c0就只是原函数连续，导数不一定连续，所以不保证光滑），**我们需要保证**中间公用的点到达两侧点的方向相反，距离相等，**就是下图的l1和l2；

<img src="F:\VS\jax-s_notebook\笔记图片\1749705921878.png" style="zoom:33%;" />

<img src="F:\VS\jax-s_notebook\笔记图片\1749706204103.png" style="zoom:25%;" />

分段贝塞尔曲线就是 **ps钢笔工具的实现原理；**

如下图所示，就是**不满足c1连续性的分段贝塞尔曲线，**可以看到

<img src="F:\VS\jax-s_notebook\笔记图片\1749706320213.png" style="zoom:33%;" />

**1,2,3,4四个控制点形成的贝塞尔曲线 和 4,5,6,7四个控制点形成的贝塞尔曲线就是不光滑的，**因为线段 3-4 和 线段 4-5 不在同一条直线并且距离不相等；

### 4.4 曲面Surfaces

#### 4.4.1 贝塞尔曲面Bezier Surfaces

由如何通过贝塞尔曲线得到贝塞尔曲面：

<img src="F:\VS\jax-s_notebook\笔记图片\1749707935572.png" style="zoom:33%;" />



通过双线性插值的思想：如下图所示，**当前有一个4x4的控制点矩阵，之后我们可以对每一行的4个控制点求解出对应的贝塞尔曲线，然后再对每一个贝塞尔曲线的时间t的点在计算生成贝塞尔曲线，将所有时间的贝塞尔曲线都计算得出之后就能形成贝塞尔曲面；**

<img src="F:\VS\jax-s_notebook\笔记图片\1749707967199.png" style="zoom:25%;" /><img src="F:\VS\jax-s_notebook\笔记图片\1749708435046.png" style="zoom:25%;" />

### 4.5 Geometry Processing几何处理

前面学习乐几何的几种表示方式，下面拿最常见的网格mesh表示方法来讲解几何的处理方式；

#### 4.5.1 Mesh Operation

网格表示的几何处理方法主要分为三类：**网格细分mesh subdivision，网格简化mesh simplification，网格正则化mesh regulation：**

<img src="F:\VS\jax-s_notebook\笔记图片\1749708719824.png" style="zoom:33%;" />

##### 4.5.1.1 subdivision细分：

（1）loop subdivision（发明人叫loop）：在前面displacement mapping的时候提到过该方法，**细分主要分为两步，首先引入更多的三角形，但是单单引入三角形是不够的，我们还要移动一些三角形的顶点位置，使得原来的图形更加光滑；**

<img src="F:\VS\jax-s_notebook\笔记图片\1749709798748.png" style="zoom:25%;" />

- 增加三角形：将一个三角形按照下面的方法划分为四个小三角形；

  <img src="F:\VS\jax-s_notebook\笔记图片\1749710040539.png" style="zoom:25%;" />

- 调整顶点位置：将顶点划分为两组，旧的顶点和新的顶点，按照不同的规则对他们进行移动；拿下面举例：

  新顶点：看下图中的一个**白点为一个新增的顶点，他被上下两个三角形所共享，之后按照下面的规则进行调整，可以理解为他就是对周围一些顶点的位置取插值平均，这样才能平滑；**

  <img src="F:\VS\jax-s_notebook\笔记图片\1749710185851.png" style="zoom:25%;" />

  旧顶点：下面重心的白点就是一个旧的顶点，他会**考虑周围旧顶点以及自身顶点的位置进行加权更新；**

  <img src="F:\VS\jax-s_notebook\笔记图片\1749710338398.png" style="zoom:25%;" />

（2）Catmull-Clark Subdivision：刚才的**loop Subdivision方法只适用于全是三角形的几何表示方法，**对于存在**四边形的几何表示时是不适用**的，而**Catmull-Clark Subdivision就是解决了这个问题，**对于任意的形状的mesh表示都可以实现细分；

<img src="F:\VS\jax-s_notebook\笔记图片\1749710748649.png" style="zoom:33%;" />

介绍一些概念：

非四边形面：就是非四边形的mesh；

奇异点：度不为4的点；

细分步骤：

- 增加网格：对每一条边都取中点，对每一个面也取一个内部的点，将几个点连起来就可以；可以看到增加了很多四边形；

<img src="F:\VS\jax-s_notebook\笔记图片\1749711003250.png" style="zoom:33%;" />

可以发现在一次细分之后，奇异点变为了4个，只要是原来的面不是四边形，那么在其内部新增的点一定是奇异点，也就是**在细分之前有几个非四边形面，那么在细分之后，这些非四边形面都会被转化为一个奇异点，并且之后细分就再也不会再增加；**

（2）调整顶点位置：

<img src="F:\VS\jax-s_notebook\笔记图片\1749711643956.png" style="zoom: 25%;" />

##### 4.5.1.2 simplication

对于物体离得远的时候，我们就没必要用过多的三角形将其渲染的过于细节，所以我们就可以将一个模型进行mesh simplication；

（1）collapsing an edge边坍缩：可以想象成**将一条边的两个点捏成一个点；**

<img src="F:\VS\jax-s_notebook\笔记图片\1749712281526.png" style="zoom:33%;" />

如何保证在减少点之后尽可能不改变原有的形状？通过引入一个指标，**Quadric Error Metrics二次误差度量，**他和L2 distance很像，如左下图所示，我想要将上面的三个顶点减少为一个顶点，此时就要让新顶点距离其简化前相邻的两个顶点的三角形的距离之和最小；

<img src="F:\VS\jax-s_notebook\笔记图片\1749712735611.png" style="zoom: 33%;" />

上面为对于一个边是如何找到最优的位置，那么对于一整个图形而言，我们到底坍缩哪一条边，具体的思路如下：**遍历图中的所有边，之后看坍缩该边之后他的最优二次误差度量是多大，按照从小到大进行排序，优先坍缩误差最小的那些边；**但是这会产生很多问题，在**坍缩完一条边之后，所有和他相邻的边都会发生变化，**那么对于这些边而言，之前计算出的二次误差度量都失效**还需要再次重新计算，**所以我们还需要**在一轮之后，更新那些和其相关的那些边的指标，**所以我们就想到了一种数据结构，**堆！**

### 4.6 Shadows阴影

#### 4.6.1 Shadow mapping

**该方法是通过光栅化来渲染出阴影的方法，**他是在**图像空间中的一种算法，**在进行阴影计算的时候**不涉及场景几何信息，而且必须要处理走样的问题；**

**动机：如果一个点不在阴影里，那么他肯定能够同时被摄像机以及光源所看到；**

硬阴影：**一个点只存在两种状态，在阴影里和不在阴影里，**所以就只存在满足上述条件和不满足上述条件的两种情况，也就是非0即1；

shadow mapping实现：

（1）render from light：首先就是**从光源**看向场景，记录得到的**各像素的深度（zbuffering），**并且不进行着色；

<img src="F:\VS\jax-s_notebook\笔记图片\1749714465891.png" style="zoom:25%;" />

（2）render from eye：之后就**从摄像机/眼睛看向场景，**我们可以看每一个点，将其**还原到从光源视角投影的图片的位置，**然后计算出**该点到达光源实际的深度，**因为我们刚才缓存了从光源视角下场景所有点的深度，**对比从该点到光源的实际深度以及从光源得到的深度图的深度，**如果**二者一致，那么就表示他前面没有物体遮挡，不在阴影里，**否则如果**实际深度大于深度图里的深度，那么就表示他处于阴影里；**

<img src="F:\VS\jax-s_notebook\笔记图片\1749714674615.png" style="zoom: 25%;" /><img src="F:\VS\jax-s_notebook\笔记图片\1749715302865.png" style="zoom:25%;" />

举例：如下图所示，左上角的一个很小的白点就是光源；

<img src="F:\VS\jax-s_notebook\笔记图片\1749715595398.png" style="zoom: 33%;" />

之后我们**从光源看向场景，计算出深度图：**

<img src="F:\VS\jax-s_notebook\笔记图片\1749715543088.png" style="zoom: 25%;" /><img src="F:\VS\jax-s_notebook\笔记图片\1749715564951.png" style="zoom:25%;" />

下面切换回摄像机/眼睛视角：**算出实际深度 对比 深度图中的深度，**但是摄像机是按照一个像素为最小单位采样还原回摄像机的投影图，对于里面的点会有些许的误差，并且由于点的坐标都是通过float来表示，那么**float判断相等是几乎不可能的事情，那么我们就通过看实际的距离是不是大于深度图中的深度加上一个bias误差来判断；**

下面就是计算得出的**shadow map图：**对于**shadow map的分辨率不能过于小于原图的分辨率，否则会出现锯齿化过于严重；**

<img src="F:\VS\jax-s_notebook\笔记图片\1749715790513.png" style="zoom: 50%;" />

因此，**shadow mapping需要对场景渲染两遍，第一遍正常渲染，第二遍通过shadow map渲染阴影；**

对于游戏中的**阴影质量选项其实调节的就是shadow map的分辨率大小，**质量越高分辨率越高；

shadow mapping的问题：仅支持硬阴影，由于每个点根据是否在阴影内都只划分成两种状态，所以会导致边缘非常锐利；

对于**点光源而言，硬阴影没有任何毛病，**但是对于**光源本身有一定大小**的情况而言，**比如：太阳，**那么就会**存在软阴影的情况；**

<img src="F:\VS\jax-s_notebook\笔记图片\1749716543521.png" style="zoom:33%;" />

































