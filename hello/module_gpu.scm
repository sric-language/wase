name = hello
summary = hello
outType = exe
version = 1.0
depends = sric 1.0, cstd 1.0, waseGraphics 1.0, waseGui 1.0, serial 1.0, cstd 1.0
srcDirs = ./


fmake.depends = jsonc 2.0, glfw 3.3.8, glew 2.2.0
fmake.srcDirs = ../native/nanovg/, ../native/
fmake.incDirs = ../native/
fmake.extIncDirs = ../native/nanovg/
fmake.defines = NVGSWU_GL3,PLUTOVG_BUILD_STATIC
fmake.gcc.extConfigs.cppflags = -std=c++17
fmake.win32.extLibs = OpenGL32.lib,GLU32.lib,XInput.lib,Winmm.lib,kernel32.lib,user32.lib,gdi32.lib,winspool.lib,comdlg32.lib,advapi32.lib,shell32.lib,ole32.lib,oleaut32.lib,uuid.lib,odbc32.lib,odbccp32.lib,ws2_32.lib,winmm.lib,wldap32.lib
fmake.win32.excludeSrc = .*(Win32Window|Plutovg).*
