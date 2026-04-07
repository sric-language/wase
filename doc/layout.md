使用布局组件来控制widget的位置和大小，相比绝对坐标更容易自适应不同的屏幕大小。有些UI框架的布局和容器是分开的，但在vase中他们是同一个。布局容器包括HBox、VBox、Pane、FlowBox、EdgePane等。

### 布局概述
布局系统有父容器决定子控件的位置和大小，自己对自己的位置和大小只有建议权，没有决定权。

widget自己的x,y,widht,height属性是运行时的值，一般用layout字段来告诉父组件自己期望的位置和大小。

layout.width和layout.height可以设置为固定值或者百分比。或者设置为Layout::matchParent或0表示填充父亲，Layout::wrapContent或-1表示和内容一样大。layout的很多字段在不同的布局容器下作用不同，后面会逐个容器介绍。

layout个字段的值默认是逻辑像素单位，具体大小会根据设备自动缩放。也可以设置单位，例如layout.wUnit=Unit::Percent将把layout.width的长度单位改为百分比。


### 布局实现

在setLayout中会调用各个子控件的prefContentSize来获取子控件期望的大小。布局不会每次都执行，调用relayout函数来触发刷新布局。

如果要自定义布局容器需要继承WidgetGroup，重写layoutChildren和prefContentSize方法。

##### 边距

距离产生美，vase和css类似的边距概念。

padding表示内边距，它是控件四周的填充区域，是属于控件自身的一部分。例如给按钮设置大的padding会把按钮撑胖。

margin表示外边距，不属于控件自身。和layout字段一样用来告诉父容器自己应该和其他控件离开一定的距离。

在HiML中，类似于css，可以写"8", "8 8", "8 8 8 8"这三种形式，按照上右下左顺序。

### VBox、HBox线性容器
VBox是最常用的布局容器，在他内部的控件都垂直排列。HBox是水平排列控件。

#####  间距

VBox和HBox的spacing属性表示个子控件之间的间距。

#####  权重

子控件的layout.weight表示和自己兄弟控件的相对大小，这个值是相对比较的没有量纲，默认值是1。

例如：
```
      waseGui::HBox
      {
        waseGui::Button { text = btn1 }
        waseGui::Button { text = btn2, layout = waseGui::Layout { weight = 3.0 } }
        waseGui::Button { text = btn3 }
      }
```
中间的button会比较大




#####  对齐

HBox的align表示对齐，用来指定全体子控件对齐方式。

子控件的layout.vAlign和layout.hAlign用来指定单个控件的垂直和水平对齐方式。

对齐对于Layout::matchParent的控件不起作用。
```
    waseGui::VBox
    {
      margin = "50 60"
      waseGui::Button
      {
        text = btn3
        layout.width = -1
        layout.hAlign = waseGui::Align::Center
      }
    }
```
#####  偏移

使用layout.offsetX和layout.offsetY来指定在标准位置上增加偏移量。例如：
```
    waseGui::VBox
    {
      margin = 50
      waseGui::Button
      {
        text = btn1
        layout.offsetX = 80
        layout.offsetY = 50
        layout = waseGui::Layout { width = -1 }
      }
      waseGui::Button
      {
        text = btn2
        layout = waseGui::Layout { width = 600 }
      }
    }
```
### Pane布局
支持指定位置和对齐的布局。

指定位置使用layout.offsetX和layout.offsetY和VBox类似。

##### 对齐
```
    waseGui::Pane
    {
      padding = "50 60"
      margin = "50 60"
      layout = waseGui::Layout { height = 0 }
      waseGui::Button
      {
        text = "btn2"
        layout.vAlign = waseGui::Align::Center
        layout.hAlign = waseGui::Align::Center
        layout.width = -1
      }
    }
```
btn2的layout.vAlign和layout.hAlign都是Align.center那么它显示在正中心。


##### 组合对齐和偏移

把控件对齐后，使用偏移可以再进行微调
```
    waseGui::Pane
    {
      padding = 50
      margin = 50
      layout = waseGui::Layout { height = 0 }
      waseGui::Button
      {
        text = "btn3"
        layout.width = -1
        layout.hAlign = waseGui::Align::End
        layout.vAlign = waseGui::Align::End
        layout.offsetX = -20
        layout.offsetY = -30
      }
    }
```
水平和垂直都使用Align.end表示对齐到右下角，offsetX和offsetY的负值表示反向调整。


### EdgeBox布局
EdgeBox表示四边布局，例如：
```
    waseGui::EdgeBox
    {
      top = waseGui::Button {  text = "top"; padding = 8 }
      left = waseGui::Button { text = "left"; padding = 8 }
      right = waseGui::Button { text = "right"; padding = 8 }
      bottom = waseGui::Button { text = "bottom"; padding = 8 }
      center = waseGui::Button { text = "center"; padding = 8 }
    }
```
只支持5个部位，这五个部位都是可以为空的。

### FlowBox布局
如果元素过多会自动换行
```sric
    new FlowBox
    {
      .margin = Insets { .initAll(50); };
      .hAlign = Align::Center;
      for (var i=0; i<20; ++i) {
        new Button{
          .setText(String::format("%d", i));
          .layout.width = 100;
        },;
      }
    };
```
这个目前的实现比较简陋，需要更多功能可以自己定义布局容器。

### ScrollPane
ScrollPane可以给控件增加滚动条。支持拖滚动条也支持滑动屏幕。滚动条虽然看起来很小，但其实触控范围很大。

