set -e

sric graphics/module.scm -fmake
sric gui/module.scm -fmake
sric demo/module.scm

fmake native/opengl/waseNanovg.fmake
fmake native/plutovg/wasePlutovg.fmake

fmake native/plutovg/demoWin.fmake
fmake native/opengl/demoNanovg.fmake

sric viewer/module.scm -fmake