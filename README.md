
Sric/C++ cross-platform GUI library.

### C++
```
#include "waseGui.h"
using namespace sric;
using namespace waseGui;

int32_t main() {
    auto frame = new_<Frame>();
    {
        auto it = new_<Button>();
        it->setText("Button");
        it->onClick = ([=](RefPtr<Widget> w) {
            Toast::showText("hello world");
            });
        frame->add(std::move(it));
    }
    frame->show();
    return 0;
}
```

### Sric
```
import waseGui::*;
fun main(): Int {
    var frame = new Frame {
        new Button {
           .setText("Button");
           .onClick(fun(w:*Widget){
               Toast::showText("Hello World");
           });
        },;
    };
    frame.show();
    return 0;
}
```
