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
### 模板测试

* 由`glStencilMask(掩码)`设置写入缓冲开关<br>
  * `glStencilMask(0xFF)` 开启
  * `glStencilMask(0x00)` 关闭

* 由`glStencilOp(模板失败, 模板失败深度通过, 均通过)`设置写入缓冲的方式<br>
  * `glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE)` 均通过时写入Stencil Buffer

* 由`glStencilFunc(测试通过条件, 参照值, 掩码)`设置测试通过条件<br>
  * `glStencilFunc(GL_ALWAYS, 1, 0xFF)` 让原物体通过测试以写入Stencil Buffer
  * `glStencilFunc(GL_NOTEQUAL, 1, 0xFF)` 让纯色放大物体的边缘通过测试


---
### 混合

* 对纹理图像中的透明部分在着色器中丢弃掉<br>
  `if (FragColor.a < 0.1f) discard;`

* 由glBlendFunc(源颜色影响因子值, 目标颜色影响因子值)混合透明图像<br>
  `glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)`

* 先渲染不透明物体 再由远及近渲染透明物体<br>


---
### 面剔除

* 三角形点的环绕次序决定了面向摄像机的正面和背面<br>
  `glEnable(GL_CULL_FACE)`默认剔除背面渲染


---
### 帧缓冲

* 由`glBindFramebuffer(GL_FRAMEBUFFER, FBO)`进行离屏渲染<br>
  * 纹理(颜色附件)必须包含，初始化只需要分配空间，实时渲染时将颜色结果写入到纹理附件中。
  * 渲染缓冲对象是不可读的，适合不需要采样的深度测试和模板测试。

* 由`glBindFramebuffer(GL_FRAMEBUFFER, 0)`将已渲染的纹理颜色附件作为新的纹理，<br>
  渲染到铺展的屏幕平面上，再由卷积运算实现滤镜效果。
  
  
---
### 立方体贴图

* 立方体贴图由方向向量决定纹理坐标，可直接实现天空盒和通过反射或折射实现环境映射。<br>
  * `view = mat4(mat3(camera.GetViewMatrix()))`保留旋转缩放丢弃位移
  * `gl_Position = pos.xyww`透视除法在顶点着色器之后计算Z值为最大深度1(分量/w)


---
### 高级数据

* `glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY)`获得目标内存指针可以直接复制数据到缓冲<br>
  `glUnmapBuffer(GL_ARRAY_BUFFER)`解绑并释放该指针
  
  
* 由`glBufferSubData(GL_ARRAY_BUFFER, offset, sizeof(data), &data)`批量填充缓冲


---
### 高级GLSL

* Uniform block 布局空间为std140，而不像uniform共享空间、变量位置可变动。<br>
  * 由`glGetUniformBlockIndex(shaderProgram, blockName)`获取顶点着色器中的块的序号
  * `glUniformBlockBinding(shaderProgram, index, bindingPoint)`将块绑定到std140布局的绑定点上
  * `glBindBufferRange(GL_UNIFORM_BUFFER, bindingPoint, buffer, offset, sizeof(data))`<br>
    将缓冲对象绑定到std140布局的绑定点上


---
### 几何着色器

* 通过`gl_in[index].gl_Position`读取`layout (Primitive) in`传入的图形数据<br>

* `layout (Primitive, max_vertices) out`将结果送入下一阶段


---
### 实例化


* `glVertexAttribPointer`顶点属性最大允许128字节，mat4需要拆分成4个vec4。

* `glVertexAttribDivisor(index, instances)`设置渲染新实例的时候更新顶点属性，否则为每个顶点更新。  
  
* 定义顶点属性，将实例化数组传入顶点着色器中，这样只对普通顶点逐点更新。<br>
  * `glDrawArraysInstanced(GL_TRIANGLES, first, elementsCount, amount)`
  * `glDrawElementsInstanced(GL_TRIANGLES, elementsCount, type, indicesPointer, amount)`
  













