

fan fmake ../jsonc/libjsonc.props -c emcc -f
fan fmake ../sric/output/sric.fmake -c emcc -f
fan fmake ../sric/output/cstd.fmake -c emcc -f
fan fmake ../sric/output/serial.fmake -c emcc -f
fan fmake ../sric/output/waseGraphics.fmake -c emcc -f
fan fmake ../sric/output/waseGui.fmake -c emcc -f
fan fmake native/demoWasm.fmake -c emcc
