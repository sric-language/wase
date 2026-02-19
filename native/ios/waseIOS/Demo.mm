//
//  Demo.mm
//  waseIOS
//
//  Created by yangjiandong on 2026/2/19.
//

#include "Demo.h"
#include "waseDemo.h"

void waseSetViewController(UIViewController *controller);

void run(UIViewController* ctrl) {
    waseSetViewController(ctrl);
    waseDemo::app->show();
}
