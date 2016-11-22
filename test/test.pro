QT += core gui opengl
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
TEMPLATE = app

INCLUDEPATH += . ../include /usr/local/include /usr/include \
                /home/idris/dev/include

LIBS += -L/usr/local/lib -L/usr/lib -lGL -lGLU -lGLEW  \
        -L${HOME}/dev/lib -L/usr/local/lib -lassimp -lgtest \
        -L${GTEST_LIBRARIES}

# adding sources manually, because of qt include things
SOURCES +=      diffTest.cpp                    \
                ../src/revisionUtils.cpp        \
                ../src/revisionNode.cpp         \
                ../src/revisionDiff.cpp         \
                ../src/model.cpp                \
                ../src/diffRig.cpp              \
                ../src/modelRig.cpp             \
                ../src/rig.cpp                  \
                ../src/boneAnim.cpp             \
                ../src/boneAnimDiff.cpp         \
                ../src/viewerUtilities.cpp      \
                ../src/bone.cpp                 \
                ../src/boneAnimDelta.cpp           

CONFIG += qt console c++11