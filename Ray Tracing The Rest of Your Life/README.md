### 1. 预览

`Ray Tracing The Rest of Your Life w/ C++ 11 features`

---
### 2. 估计

用圆和正方形面积来估计𝜋 <br>
`有效点 / 采样点 = 𝜋 / 4` <br>

由于收益递减法则，可以使用网格的方式采样。
```
for (int i = 0; i < N; ++i)
  for (int j = 0; j < N; ++j)
     x = (i + random_double) / N
     y = (j + random_double) / N
```

---
### 3. 一维蒙特卡洛


由Y ~ U\[0, 1]，对任意概率分布F(x)，令X = F<sup>-1</sup> (Y)，<br>
则X是服从F(x)分布的随机数，再除以其概率密度即为重要性采样。<br>

```
已知被积分函数f(x)及积分域，选一个积分域上的概率密度函数，
由概率分布函数的反函数产生随机数，求出函数值再除以概率密度。
概率密度函数拟合f(x)越精确，加权平均的结果收敛越快。
```


---
### 4. 球面蒙特卡洛

`φ为与x轴夹角，θ为与z轴夹角。`<br>

立体角 ω = A / r<sup>2</sup><br>

dA = (rdθ)·(rsinθdφ) = r<sup>2</sup>sinθdθdφ<br>
dω = dA / r<sup>2</sup> = sinθdθdφ<br>

积分cos<sup>2</sup>(θ)sin(θ)dθdφ = -cos<sup>3</sup>(θ)/3·2𝜋 = 2/3·2𝜋 = 4𝜋/3


> r<sup>2</sup> + z<sup>2</sup> = 1<br>
> tanθ = r / z<br>
> z<sup>2</sup> (tan<sup>2</sup>θ + 1) = 1<br>
> z<sup>2</sup> · sec<sup>2</sup>θ = 1<br>
> z = cosθ<br>

以概率密度函数1/4𝜋采样随机单位向量


---
### 5. 散射

PDF的积分必须为1，对PDF = cosθ，<br>
在半球上的积分cos(θ)·sin(θ)dθdφ = 𝜋<br>
令PDF = cosθ / 𝜋<br>

`Lambertian BRDF = ω · 散射方向PDF / cosθ =  ω / 𝜋`


---
### 6. 漫反射


散射的方向是半球的随机向量<br>
光线方向的PDF = cosθ / 𝜋<br>
返回的颜色 = 散射PDF · 递归散射 / 采样PDF


---
### 7. 随机向量


```
x = cos𝜙·sin𝜃
y = sin𝜙·sin𝜃
z = cos𝜃
```

对球体的均匀分布 
```
𝑟1 = [0, 𝜙]积分 1/2𝜋 𝑑𝑡 = 𝜙 / 2𝜋
𝑟2 = [0, 𝜃]积分 2𝜋·sin(𝑡) 𝑑𝑡 / 4𝜋 = (1 - cos𝜃) / 2
```

对半球的 p = cos𝜃 / 𝜋 
```
𝑟2 = [0, 𝜃]积分 cos(𝑡)/𝜋 · 2𝜋·sin(𝑡) 𝑑𝑡
cos𝜃 = sqrt(1 - 𝑟2)
```

---
### 8. 正交

以交点坐标的法向量为一个轴建立局部坐标系。
```
w = n
v = n x (0,1,0) if n not close to (1,0,0)
u = v x w
```
在局部坐标系中计算散射方向以及PDF


---
### 9. 直接光源采样

调整光的散射方向至光源平面中的随机一点<br>

> 𝑑𝑤 = 𝑑A · cos𝑎 / r<sup>2</sup> 𝑎为光源面与投影面的夹角<br>
> PDF · 𝑑𝑤 = dA / A<br>
> PDF = r<sup>2</sup> / (cos𝑎 · A)






