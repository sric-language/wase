set -e

sric graphics/module.scm
sric gui/module.scm
sric demo/module.scm

# fan fmake ../jsonc/libjsonc.props -c emcc 
# fan fmake ../sric/output/sric.fmake -c emcc 
# fan fmake ../sric/output/serial.fmake -c emcc 

fmake ../sric/output/waseGraphics.fmake -c emcc 
fmake ../sric/output/waseGui.fmake -c emcc 
fmake native/wasm/demoWasm.fmake -c emcc
fmake native/wasm/waseWasm.fmake -c emcc
