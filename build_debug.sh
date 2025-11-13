set -e

sric graphics/module.scm -fmake -debug
sric gui/module.scm -fmake -debug
sric demo/module.scm

fan fmake native/waseNanovg.fmake -debug
fan fmake native/wasePlutovg.fmake -debug

fan fmake native/demoWin.fmake -debug -G
fan fmake native/demoNanovg.fmake -debug -G
