### 1. 预览

`Ray Tracing the Next Week w/ C++ 11 features`

---
### 2. 动态模糊

模拟快门：为光线添加时间属性，摄像机在快门内的随机时间发射光线。
更新球类：球心由变量center变为函数返回值center(time())。
光线发出后，与此刻运动到某位置的球求交。
