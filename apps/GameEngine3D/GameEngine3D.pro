TEMPLATE = app
TARGET +=
DEPENDPATH += . src/ ../../libs/maths ../../libs/utils ../../libs/geometry ../../libs/physics
INCLUDEPATH += . src/ ../../libs/maths ../../libs/utils ../../libs/geometry ../../libs/physics

QMAKE_CFLAGS+=-pg
QMAKE_CXXFLAGS+=-pg
QMAKE_LFLAGS+=-pg

CONFIG -= qt
CONFIG += release opengl
LIBS += -lglut

# Input
HEADERS += main.h \
           collision.h inertia.h ConvexHull.h GenericShip.h GraphicsMaths.h Integrator.h \
           OBBTree.h OBB.h Object.h Polytope.h RigidBody.h SpaceFlightBody.h SupportMappable.h \
           special.h Vector2D.h Vector3D.h Matrix3x3.h Quaternion.h \
           BasicCamera.h QuakeCamera.h QuatCamera.h \
           Keybinding.h MiscUtils.h GLUtils.h TimeFuncs.h CrossPlatform.h Constants.h
SOURCES += main.cpp \
           collision.cpp inertia.cpp ConvexHull.cpp GenericShip.cpp GraphicsMaths.cpp Integrator.cpp \
           OBBTree.cpp OBB.cpp Object.cpp Polytope.cpp RigidBody.cpp SpaceFlightBody.cpp SupportMappable.cpp \
           special.cpp Vector2D.cpp Vector3D.cpp Matrix3x3.cpp Quaternion.cpp \
           BasicCamera.cpp QuakeCamera.cpp QuatCamera.cpp \
           Keybinding.cpp MiscUtils.cpp GLUtils.cpp TimeFuncs.cpp
