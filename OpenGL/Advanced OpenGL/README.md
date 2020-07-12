### 深度测试


* 清除颜色缓冲以及深度缓冲<br>
  `glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)`


* 深度缓冲将观察空间中的Z值非线性处理<br>
  F<sub>depth</sub> = (1/z - 1/near) / (1/far - 1/near)


* 深度冲突
  * 调整两个重叠面的间隙
  * 将近平面设置的远一些
  * 由24位设置到32位精度
  

---
