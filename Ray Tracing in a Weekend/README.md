### 1. 预览

`Ray Tracing in One Weekend w/ C++ 11 features`

---
### 2. 生成图像

用0-255表示RGB的值 编写PPM文件
```
P3             // 编码格式
256 256        // 宽度和高度
255            // 最大像素值
155 155 155    // 图像数据部分
..  ..  ..     // 左上开始每行一段
```
制作进度条:  `std::cerr << '\r' << ... << std::flush;`

---
### 3. 向量

构建三维向量类，成员函数有：`取相反数，向量加法，向量倍乘、倍除，取模。`<br>
重载向量类的运算符：`输出，向量加减，逐一相乘，向量倍乘、倍除。`<br>
其他函数：`点积，叉积，单位向量。`<br>

声明别名：`坐标和颜色`

`color c(155, 155, 155);`

---
### 4. 光线

射线函数：`P(t) = o + t·d`<br>
光线从视点向投影面逐个像素射出。

用光线Y轴分量对白色和蓝色作插值渲染天空色。<br>
`(1.0 - t) * white + t * blue;`


---
### 5. 球

联立光线和球的表达式解二次方程<br>
判定函数:`bool hit_sphere(const point3& center, double radius, const ray& r)`

---
### 6. 法向量

将光线与球的交点的法向量映射为RGB渲染出球体<br>
`球面法向量 = 球面上的点 - 球心`


> 构建`hittable`基类，成员函数为与光线求交的纯虚函数。<br>
>
> 其中`hit_record`是引用传参，成员包含：`与光线的交点，和光线相交物体的法向量，射线参数t，法向量调整函数。`<br>
>
>> 由`hittable`派生`hittable_list`  成员为：`vector<shared_ptr<hittable>>`<br>
>>
>> 由`hittable`派生`sphere`  实现球类与光线求交<br>


将场景中的所有物体装入list(vector)中，光线射出后与场景中的物体逐一求交(zbuffer)。

---
### 7. 反走样

通过多重采样周围的像素反走样，随机生成 \[0, 1) ：
```
static std::random_device seed;
static std::mt19937 generator(seed());
static std::uniform_real_distribution<double> distribution(0.0, 1.0);
return distribution(generator);
```

整合camera类，成员包含：光线生成函数`ray get_ray(double u, double v)`

光线从视点向投影面逐个像素值射出，由N条光线采样距离该像素\[0, 1)距离的像素，<br>
与场景中的物体逐一求交，累加后取均值。

---
### 8. 漫反射

旧版hack：在与交点相切的单位半径的球体中随机取点作为反射方向。概率分布为：cos<sup>3</sup>(𝜙)

Lambertian：取相切球面上的点。 靠近法向量的散射方向的概率会高，但是分布更均匀。 概率分布为：cos(𝜙)<br>
> 取随机θ ∈\[0, 2π], z ∈\[-1, 1]<br>
> 散射方向：(r·cos(θ),  r·sin(θ),  z)<br>
> 其中 r<sup>2</sup> + z<sup>2</sup> = 1<br>

替代方法：取交点作为球中心。随机向各方向散射，取法向量半球中的结果。

---
### 9. 金属

> 构建`material`基类，成员函数为散射的纯虚函数，参数为：`ray, hit_record, &attenuation, &sacttered`<br>
>> 由`material`派生`lambertian` 实现光线的漫反射<br>
>>
>> 由`meterial`派生`metal` 实现光线的镜面反射，磨砂处理由随机调整反射光线实现。
>>> `散射方向 = 反射方向 + 模糊率 * 单位球中的随机点`

将`material`添加到`hit_record`的成员参数中，<br>
光线击中物体后，由指针判定散射方法，对散射的光线递归求交。




