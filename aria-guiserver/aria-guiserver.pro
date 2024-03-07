# Archivo generado por el gestor qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectorio relativo al directorio principal del proyecto: ./src/aria-guiserver
# El destino es una aplicacion: ndguiServer

QT += xml widgets svgwidgets
QT -= gui
INCLUDEPATH += /usr/local/Arnl/include \
               /usr/local/Aria/ArNetworking/include \
               /usr/local/Aria/include 
OBJECTS_DIR = ../neuraldis/.obj 
QMAKE_LIBDIR = /usr/local/Arnl/lib \
               /usr/local/Aria/lib
QMAKE_CXXFLAGS_RELEASE += -O2
QMAKE_CXXFLAGS_DEBUG += -O0
TARGET = ndguiServer
#DESTDIR = ../../bin
#CONFIG += debug \
#          warn_on
TEMPLATE = app 
SOURCES += guiServer.cpp \
 ../aria-tools/aractiongroupreactive.cpp \
 ../aria-tools/arianetpacketwrapper.cpp \
 ../aria-tools/arserverextinforobot.cpp \
 ../aria-tools/arservermodereactive.cpp \
 ../aria-tools/bumperinfowrapper.cpp \
 #../aria-tools/ndguiserver.cpp \
 ../aria-tools/ndrangedeviceinfowrapper.cpp \
 ../aria-tools/robotinfowrapper.cpp \
 ../aria-tools/sickinfowrapper.cpp \
 ../aria-tools/sonarinfowrapper.cpp
#CONFIG -= release

HEADERS += ../aria-tools/aractiongroupreactive.h \
../aria-tools/arianetpacketwrapper.h \
../aria-tools/arserverextinforobot.h \
../aria-tools/arservermodereactive.h \
../aria-tools/bumperinfowrapper.h \
#../aria-tools/ndguiserver.h \
../aria-tools/ndrangedeviceinfowrapper.h \
../aria-tools/robotinfowrapper.h \
../aria-tools/sickinfowrapper.h \
../aria-tools/sonarinfowrapper.h

LIBS += -L/usr/local/Arnl/lib \
  -L/usr/local/Aria/lib \
  -lBaseArnl \
  -lArnl \
  -lArNetworking \
  -lAria

