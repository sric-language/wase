QT = core
QT += gui
QT += widgets

CONFIG += c++17 cmdline

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG(debug, debug|release) {
    DEFINES += _DEBUG
} else {
    DEFINES += NDEBUG
}

SOURCES += \
        main.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

CONFIG(debug, debug|release) {
    FMAKE_REP_LIB = C:/Users/user/fmakeRepo/gcc/debug
} else {
    FMAKE_REP_LIB = C:/Users/user/fmakeRepo/gcc/release
}

INCLUDEPATH += \
  $$FMAKE_REP_LIB/jsonc/include \
  $$FMAKE_REP_LIB/sric/include \
  $$FMAKE_REP_LIB/waseGraphics/include \
  $$FMAKE_REP_LIB/waseGui/include \
  $$FMAKE_REP_LIB/waseQt/include \
  $$FMAKE_REP_LIB/serial/include \

LIBS += -L$${FMAKE_REP_LIB}/jsonc/lib
LIBS += -L$${FMAKE_REP_LIB}/sric/lib
LIBS += -L$${FMAKE_REP_LIB}/waseGraphics/lib
LIBS += -L$${FMAKE_REP_LIB}/waseGui/lib
LIBS += -L$${FMAKE_REP_LIB}/waseQt/lib
LIBS += -L$${FMAKE_REP_LIB}/serial/lib

LIBS += -lwaseGui
LIBS += -lwaseGraphics
LIBS += -lwaseQt
LIBS += -lserial
LIBS += -lsric
LIBS += -ljsonc


LIBS += -lopengl32 -lgdi32 -lwinmm
