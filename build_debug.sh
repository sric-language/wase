set -e

sric graphics/module.scm -fmake -debug
sric gui/module.scm -fmake -debug
sric demo/module.scm

fan fmake native/opengl/waseNanovg.fmake -debug
fan fmake native/plutovg/wasePlutovg.fmake -debug

fan fmake native/plutovg/demoWin.fmake -debug -G
fan fmake native/opengl/demoNanovg.fmake -debug -G
