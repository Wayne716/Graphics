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

```
已知被积分函数f(x)及积分域
选一个积分域上的概率密度函数
对所有的采样取f(random)/p(random)平均
```
