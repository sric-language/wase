set -e

sric graphics/module.scm -fmake
sric gui/module.scm -fmake
sric demo/module.scm

fan fmake native/waseNanovg.fmake
fan fmake native/wasePlutovg.fmake

fan fmake native/demoWin.fmake
fan fmake native/demoNanovg.fmake
