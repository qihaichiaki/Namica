## OpenGL Shader

### OpenGL Version
```C++
#version 330 core
```
* 使用的是 opengl 3.3 core profile

### layout
```C++
layout(location = 0) in vec3 position;
```

* layout(location = 0) 位置0, 用于GPU上传来的数据
* location对应位VAO中一个顶点数据中每个元素的index

### 变量
```C++
in/out TYPE NAME; 
```
* in/out 输入, 输出参数
* TYPE 变量类型 比如:vec3 
* NAME 变量名
* 在vertex shader中out, 会传递到fragment shader中去;fragment shader则是输出对应像素的颜色 

### 变量类型
* vec 向量类型: vec3, vec4

### shader main
```C++
void main()
{

}
```

### vertex shader gl变量
```c++
    gl_Position = vec4(position.x, position.y. position.z, 1.0);
```
* vec4 gl_Position 表示当前点的齐次坐标, 最后一个为w分量, 后续会参与到透视除法(坐标每个值除以w)的操作, 通过置为1.0即可. 后续利用矩阵预算得到的w不一样, 通过此来得到近大远小的效果


### fragment shader out变量
```C++
// FragColor
out vec4 fragmentColor;
```

### uniform
```C++
uniform type name;
```

* 全局GPU变量, 可以通过CPU传输到GPU
* 注意没有输入输出修饰符(in/out)

* type为sampler2D为贴图对象, 在片段着色器中通过uv坐标进行采样获取
    * vec4 texColor = texture(sampler2DObj, uv);
* sampler2D在GPU中存在多个纹理单元, 通过上传uniform变量告诉GPU用的什么槽位的纹理(glUniform1i)
