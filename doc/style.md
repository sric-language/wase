现代GUI库基本都同时支持用代码或者配置文件来定义视图。 静态页面用配置文件便于用可视化设计工具操作。动态的内容用代码来写足够灵活。 Wase也不例外，但不使用XML，而是用专门的序列化语法HiML。

### 界面文件
定义视图文件style.himl:
```
waseGui::VBox
{
  style = paneBack
  padding = "50 50 50 50"
  layout = waseGui::Layout { height = 0 }
  spacing = 15

  waseGui::Label { text = Hello }

  waseGui::Button {
    id = button
    style = myButton
    text = "Push Button2"
  }
}
```
类似于JSON格式，具体的语法见HiML格式说明。

### 样式文件
样式使用类似与CSS的配置文件定义style.himl:
```
myButton = waseGui::RoundButtonStyle {
    color = #C71585
    fill = false
    stroke = true
    fontColor = #ef4d4dff
    outlineColor = #ef4d4dff
    roundRadius = 40
}
```
每个控件有style字段，通过它匹配到样式名称。 更多可设置的属性见`waseGui::RoundButtonStyle`以及其父类`waseGui::WidgetStyle`。

值得注意的是padding和margin写在视图文件中，而不是样式文件中。

### 代码中加载视图文件
加载配置文件示例：
```
import sric::*
import waseGui::*
import waseGraphics::*
import serial::*

fun main(): Int {
    printf("Hello World\n")

    var frame = new Frame
    frame.loadView("res/view.himl")
    frame.loadStyle("res/style.himl")

    frame.findById("button").onClick = fun(w:* Widget) {
        Toast::showText("hello world")
    }
    frame.show();
    return 0
}
```
也可以先解析成Widget对象，再手动添加到窗口中:
```
var widget = Frame::loadViewFile("res/view.himl");
```
这样一个复杂的界面一般要分成多个himl文件，然后在代码中组合起来。

### 界面实时预览工具
目前视图文件需要手写，在没有开发设计工具之前，有一个waseViewer的实时预览工具。

'C:\Users\user\fmakeRepo\gcc\debug\waseViewer\bin\waseViewer' res/view.himl -s res/style.himl
当你修改了对应的视图文件，保存后，预览界面会自动更新。


### 布局
Wase支持灵活的布局系统，将在后续的章节介绍。