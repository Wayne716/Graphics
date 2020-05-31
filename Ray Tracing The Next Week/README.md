
    章节1: Motion Blur
    
    在快门时间断内随机采样。再多重采样。
    光线记录“当前时间”，与运动到此刻的物体求交。
    
    新添加类：移动球体（起止时间和中点位置）
    - 可以用属性不变的移动球体来表示静止球体
    
    新添加属性：光线“时间记录”
    - 在每次散射时转发“时间记录”

![alt text](https://github.com/Wayne716/Graphics/blob/master/Ray%20Tracing%20The%20Next%20Week/images/blur.png?raw=true)

    章节2: Bounding Volume Hierarchies
    
    新添加类：包围盒类（与光线求交）
    - 时间段求交集：取最大的起点和最小的终点
    - 轴方向为负时要交换所求的起止时间
    - 用比较运算符替代Max/Min避免NaN
    
    新添加类：BVH树
    - 随机选轴排序递归建树
    
    新添加属性：包围盒
    - 球类用球心和半径计算包围盒
    - 移动球类用起止点的两个包围盒的合并盒
    - 对列表中的每个物体迭代地求合并包围盒



    章节3: Solid Textures
    
    棋盘材质用坐标的正弦或余弦来实现周期性，分为两个固定着色材质。
    
    新添加类：材质
    - 材质替换掉颜色衰减向量
    
![alt text](https://github.com/Wayne716/Graphics/blob/master/Ray%20Tracing%20The%20Next%20Week/images/two_spheres.png?raw=true)
