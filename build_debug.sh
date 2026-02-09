set -e

sric graphics/module.scm -fmake -debug
sric gui/module.scm -fmake -debug
sric demo/module.scm

fmake native/opengl/waseNanovg.fmake -debug
fmake native/plutovg/wasePlutovg.fmake -debug

fmake native/plutovg/demoWin.fmake -debug -G
fmake native/opengl/demoNanovg.fmake -debug -G

sric viewer/module.scm -fmake -debug
