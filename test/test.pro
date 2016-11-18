######################################################################
# Automatically generated by qmake (3.0) Wed Nov 9 15:17:24 2016
######################################################################

QT += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
TEMPLATE = app

INCLUDEPATH += . ../include /usr/local/include /usr/include \
                /home/idris/dev/include ${GTEST_INCLUDE_DIRS}

LIBS += -L${GTEST_LIBRARIES} \
        -L/usr/local/lib -L/usr/lib -lGL -lGLU -lGLEW  -lgtest  \
        -L${HOME}/dev/lib -L/usr/local/lib -lassimp

# adding sources manually, because of qt include things
SOURCES +=      ../src/revisionUtils.cpp        \
                ../src/revisionNode.cpp         \
                ../src/model.cpp                \
                diffTest.cpp

CONFIG += console c++11
