set -e

sric graphics/module.scm -fmake
sric gui/module.scm -fmake
sric demo/module.scm

fan fmake native/opengl/waseNanovg.fmake
fan fmake native/plutovg/wasePlutovg.fmake

fan fmake native/plutovg/demoWin.fmake
fan fmake native/opengl/demoNanovg.fmake

sric viewer/module.scm -fmake