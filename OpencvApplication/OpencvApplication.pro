QT += core gui 3dcore 3drender 3dinput 3dextras 3dquick

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    apriltag_quad_thresh.cpp \
    aruco.cpp \
    calibrator.cpp \
    charuco.cpp \
    dictionary.cpp \
    main.cpp \
    mainwindow.cpp \
    renderer3d.cpp \
    videocapturer.cpp \
    zmaxheap.cpp

HEADERS += \
    apriltag_quad_thresh.hpp \
    aruco.hpp \
    calibrator.h \
    charuco.hpp \
    dictionary.hpp \
    mainwindow.h \
    precomp.hpp \
    predefined_dictionaries.hpp \
    predefined_dictionaries_apriltag.hpp \
    renderer3d.h \
    unionfind.hpp \
    videocapturer.h \
    zarray.hpp \
    zmaxheap.hpp

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../opencv/build/x64/vc15/lib/ -lopencv_world410
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../opencv/build/x64/vc15/lib/ -lopencv_world410d
else:unix: LIBS += -L$$PWD/../../../../opencv/build/x64/vc15/lib/ -lopencv_world410

INCLUDEPATH += $$PWD/../../../../opencv/build/include
DEPENDPATH += $$PWD/../../../../opencv/build/include


lin



