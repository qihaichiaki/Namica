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

* layout(location = 0) 位置0
* in 输入
* vec3 变量类型
* position 变量名

### shader main
```C++
void main()
{

}
```

### vertex shader gl变量
```c++
    gl_Position = vec4{position.x, position.y. position.z, 1.0};
```
* vec4 gl_Position 表示当前点的齐次坐标, 最后一个为w分量, 后续会参与到透视除法(坐标每个值除以w)的操作, 通过置为1.0即可. 后续利用矩阵预算得到的w不一样, 通过此来得到近大远小的效果