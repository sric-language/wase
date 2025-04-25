set -e

sric graphics/module.scm
sric gui/module.scm
sric demo/module.scm

fan fmake ../jsonc/libjsonc.props -c emcc 
fan fmake ../sric/output/sric.fmake -c emcc 
fan fmake ../sric/output/cstd.fmake -c emcc 
fan fmake ../sric/output/serial.fmake -c emcc 
fan fmake ../sric/output/waseGraphics.fmake -c emcc 
fan fmake ../sric/output/waseGui.fmake -c emcc 
fan fmake native/demoWasm.fmake -c emcc
