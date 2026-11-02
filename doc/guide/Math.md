# 数学

math 提供向量、4×4 矩阵、四元数和角度转换，适合位置、方向和基础变换计算。

## 向量

常用类型为 `Vec2 / Vec3 / Vec4`（浮点）和 `Veci2`（整数）。默认构造得到零向量；`Vec3{1.0f}` 会将所有分量设为 1。

```cpp
#include <namica/math/Vector.h>
#include <iostream>

int main()
{
    namica::Vec3 const offset{3.0f, 4.0f, 0.0f};
    auto direction{offset.normalized()};
    std::cout << direction.x() << ", " << direction.y() << '\n';
    std::cout << offset.dot(offset) << '\n';
    return 0;
}
```

本例方向约为 `(0.6, 0.8, 0)`，点积为 25。`normalized()` 返回副本；`normalize()` 修改原对象。分量可以通过 `x()/y()/z()/w()` 或 `[index]` 访问，颜色场景还可使用 `r()/g()/b()/a()`。

`a * b` 是逐分量相乘，点积需要调用 `a.dot(b)`。浮点比较可以使用 `isEqual(other, tolerance)`；归一化仅提供给浮点向量。

## 矩阵与变换顺序

`Mat4{}` 是零矩阵；创建单位矩阵必须写 `Mat4{1.0f}`。矩阵按列存储，`matrix(row, column)` 按行列访问，`matrix[column][row]` 先取列。

```cpp
#include <namica/math/Matrix.h>
#include <namica/math/Utils.h>
#include <iostream>

int main()
{
    namica::Mat4 transform{1.0f};
    transform.translate(namica::Vec3{10.0f, 0.0f, 0.0f})
        .rotate(namica::radians(90.0f), namica::Vec3{0.0f, 0.0f, 1.0f})
        .scale(namica::Vec3{2.0f, 2.0f, 2.0f});

    auto point{transform * namica::Vec4{1.0f, 0.0f, 0.0f, 1.0f}};
    std::cout << point.x() << ", " << point.y() << ", " << point.z() << '\n';
    return 0;
}
```

结果约为 `(10, 2, 0)`。变换函数向当前矩阵右乘，所以本例矩阵为 `T * R * S`，作用到列向量时先缩放、再旋转、最后平移。位置使用 `w = 1`，方向使用 `w = 0`，方向不受平移影响。

`translate / rotate / scale / inverse` 修改原矩阵；`translated / rotated / scaled / inversed` 返回副本。旋转角度使用弧度，`radians` 和 `degrees` 在角度与弧度间转换。

### 透视投影

下面是可放入函数体的片段，需要包含 `Matrix.h` 和 `Utils.h`：

```cpp
namica::Mat4 const projection{namica::Mat4::perspective(
    namica::radians(60.0f), 16.0f / 9.0f, 0.1f, 100.0f)};
```

参数依次为垂直视野角（弧度）、宽高比、近平面距离和远平面距离。当前约定为右手系、NDC 深度范围 `[-1, 1]`；传入有效视野角、正宽高比和 `0 < near < far`，接口不会替调用方校验这些条件。

## 四元数旋转

`Quat{}` 表示无旋转；直接构造时分量顺序为 `w, x, y, z`。`angleAxis` 的轴需要调用方提供单位向量：

```cpp
#include <namica/math/Quaternion.h>
#include <namica/math/Utils.h>
#include <iostream>

int main()
{
    auto rotation{namica::Quat::angleAxis(
        namica::radians(90.0f), namica::Vec3{0.0f, 0.0f, 1.0f})};
    auto direction{rotation * namica::Vec3{1.0f, 0.0f, 0.0f}};
    std::cout << direction.x() << ", " << direction.y() << '\n';
    return 0;
}
```

结果约为 `(0, 1, 0)`。`toMatrix()` 可生成对应旋转矩阵；旋转向量或转换矩阵时应使用单位四元数。`fromEuler(Vec3{x, y, z})` 接收弧度，当前组合为 `qZ * qY * qX`，对向量依次应用 X、Y、Z 旋转。

## 需要注意的边界

零向量归一化后仍为零；零四元数归一化后也保持不变，不能据此得到有效旋转。极大数值的向量归一化可能溢出。

矩阵求逆在行列式恰为零时保持原值，且不返回成功标记；不能把返回一个矩阵视为求逆成功。需要求逆时应由调用方确保输入可逆，并考虑接近奇异时的数值误差。分量下标也需要调用方保证有效。

公开接口：[Vector.h](../../runtime/include/namica/math/Vector.h) · [Matrix.h](../../runtime/include/namica/math/Matrix.h) · [Quaternion.h](../../runtime/include/namica/math/Quaternion.h) · [Utils.h](../../runtime/include/namica/math/Utils.h)。返回[用户指南](README.md)。
