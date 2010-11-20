######################################################################
# Automatically generated by qmake (2.00a) Thu Sep 21 13:40:32 2006
######################################################################

TEMPLATE = app
TARGET +=
DEPENDPATH += . ../Utils ../Maths ../Maths/tnt
INCLUDEPATH += . ../Utils ..//Maths ../Maths/tnt
CONFIG -= qt
CONFIG += release opengl
LIBS += -lglut
QMAKE_CXXFLAGS_RELEASE += -pedantic -ansi

# Input
HEADERS += Mesh.h \
           PlyReader.h \
           MiscUtils.h \
           Vector3D.h \
           Matrix3x3.h \
           Quaternion.h \
           special.h \
           BasicCamera.h \
           QuakeCamera.h \
           Keybinding.h
SOURCES += main.cpp \
           Mesh.cpp \
           PlyReader.cpp \
           MiscUtils.cpp \
           Vector3D.cpp \
           Matrix3x3.cpp \
           Quaternion.cpp \
           special.cpp \
           BasicCamera.cpp \
           QuakeCamera.cpp \
           Keybinding.cpp
