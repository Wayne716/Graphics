### 1. 预览

`Ray Tracing the Next Week w/ C++ 11 features`

---
### 2. 动态模糊


模拟快门：为光线添加时间属性，摄像机在快门内的随机时间发射光线。<br>
球类动态化：球心由变量center变为函数返回值center(time())。<br>

光线发出后，与此刻运动到某位置的球求交。

![](images/moving_sphere.jpg)

---
### 3. 包围盒


包围盒由两个点表示，分别是三个轴的最小点和最大点。<br>
光线与包围盒求交：对每个轴分别计算进出盒子的时间区间再判断是否有效。<br>
然后对每一个hittable实现求包围盒的成员函数。<br>
对hittable_list需要实现两两包围盒的融合。<br>

BVH查询：
```
从根节点到叶节点每个节点都是包围盒，
若未击中直接返回，否则递归查询左子树或右子树。
```

BVH建树：
```
随机选一个轴
对区间中的物体的包围盒的小端按轴坐标排序
左右子树各放一半
```

---
### 4. 纹理

> 构造texture类，成员为返回颜色的纯虚函数
>> material类派生solid_color，替换掉原有的color(r,g,b)<br>
>>
>> material类派生checker_texture，实现棋盘纹理，成员变量为两个solid_color
>>> 求采样点的坐标的弦函数从而实现周期化，对正值负值用不同的solid_color返回颜色

![](images/two_spheres.jpg)

---
### 5. 噪声

噪音函数：
`对晶格的八个点生成随机的向量`

插值函数：`对晶格的八个点作三线性插值`

Perlin Noise：
```
对所求点指向晶格各点的向量与晶格各点的随机向量作点积
对点积的插值之和可以用缓和曲线过渡
```
![](images/hashed.jpg)
![](images/interpolate.jpg)
![](images/hermitian.jpg)
![](images/shift.jpg)
![](images/turbulence.jpg)
![](images/marble.jpg)

---
### 6. 映射

用stb_image.h的stbi_load()读取材质图片<br>
根据球体与光线的交点的极坐标求得uv<br>
根据uv求得stbi_load()返回的char* 中的位置<br>

![](images/earth.jpg)

---
### 7. 区域光

材质类新增发光函数成员<br>
```
对于不发光物体：发光函数直接返回
对于发光物体：不发生散射，发光由固定颜色纹理实现
```
对于光线与矩形的求交，先根据矩形所垂直的轴的分量求出时间，<br>
再验证其他两个分量在此刻是否在矩形的边界内。

![](images/arealight.jpg)
![](images/walls.jpg)

---
### 8. 变换

box继承hittable，以便变换类指向box。<br>
光线求交由成员hittable_list实现。<br>
box的构造函数中的参数包含物体的材质。<br>

平移： 
```
物体移动，视线也移动。
物体不动，将视线的起点减去移动的偏移量。
发出光线后，修正交点即再加上偏移量。
```
旋转：
```
绕y轴旋转矩阵：

|  cosθ  0  sinθ |
|   0    1   0   |
| -sinθ  0  cosθ |

修正光线的起点、方向，交点的位置，法向量。
```
![](images/box.jpg)
![](images/cornell.jpg)

---
### 9. 烟雾

各向同性材质的散射方向是随机的<br>
对物体的两次光线求交确认光线穿过该物体<br>
横穿的路径长度与密度相关的最长散射距离比较判断是否相交<br>

![](images/smoke.jpg)
