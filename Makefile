#############################################################################
# Makefile for building: aria-guiserver
# Generated by qmake (2.01a) (Qt 4.8.6) on: lun may 5 16:54:24 2014
# Project:  aria-guiserver.pro
# Template: app
# Command: /usr/lib/x86_64-linux-gnu/qt4/bin/qmake -o Makefile aria-guiserver.pro
#############################################################################

####### Compiler, tools and options

CC            = gcc
CXX           = g++
DEFINES       = -DQT_WEBKIT -DQT_QT3SUPPORT_LIB -DQT3_SUPPORT -DQT_XML_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED
CFLAGS        = -m64 -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
CXXFLAGS      = -m64 -pipe -g -Wall -W -D_REENTRANT $(DEFINES)
INCPATH       = -I/usr/share/qt4/mkspecs/linux-g++-64 -I. -I/usr/include/qt4/QtCore -I/usr/include/qt4/QtGui -I/usr/include/qt4/QtXml -I/usr/include/qt4/Qt3Support -I/usr/include/qt4 -I/usr/local/Arnl/include -I/usr/local/Aria/ArNetworking/include -I/usr/local/Aria/include -I.
LINK          = g++
LFLAGS        = -m64
LIBS          = $(SUBLIBS)  -L/usr/local/Arnl/lib -L/usr/local/Aria/libQMAKE_CXXFLAGS_RELEASE\ +=\ -O2QMAKE_CXXFLAGS_DEBUG -L+= -L-O0TARGET\ =\ ndguiServerDESTDIR -L= -L../../bin -L/usr/lib/x86_64-linux-gnu -L/usr/local/Arnl/lib -L/usr/local/Aria/lib -lBaseArnl -lArnl -lArNetworking -lAria -lQt3Support -lQtXml -lQtGui -lQtCore -lpthread 
AR            = ar cqs
RANLIB        = 
QMAKE         = /usr/lib/x86_64-linux-gnu/qt4/bin/qmake
TAR           = tar -cf
COMPRESS      = gzip -9f
COPY          = cp -f
SED           = sed
COPY_FILE     = $(COPY)
COPY_DIR      = $(COPY) -r
STRIP         = strip
INSTALL_FILE  = install -m 644 -p
INSTALL_DIR   = $(COPY_DIR)
INSTALL_PROGRAM = install -m 755 -p
DEL_FILE      = rm -f
SYMLINK       = ln -f -s
DEL_DIR       = rmdir
MOVE          = mv -f
CHK_DIR_EXISTS= test -d
MKDIR         = mkdir -p

####### Output directory

OBJECTS_DIR   = ../neuraldis/.obj/

####### Files

SOURCES       = guiServer.cpp \
		../aria-tools/aractiongroupreactive.cpp \
		../aria-tools/arianetpacketwrapper.cpp \
		../aria-tools/arserverextinforobot.cpp \
		../aria-tools/arservermodereactive.cpp \
		../aria-tools/bumperinfowrapper.cpp \
		../aria-tools/ndguiserver.cpp \
		../aria-tools/ndrangedeviceinfowrapper.cpp \
		../aria-tools/robotinfowrapper.cpp \
		../aria-tools/sickinfowrapper.cpp \
		../aria-tools/sonarinfowrapper.cpp moc_ndguiserver.cpp
OBJECTS       = ../neuraldis/.obj/guiServer.o \
		../neuraldis/.obj/aractiongroupreactive.o \
		../neuraldis/.obj/arianetpacketwrapper.o \
		../neuraldis/.obj/arserverextinforobot.o \
		../neuraldis/.obj/arservermodereactive.o \
		../neuraldis/.obj/bumperinfowrapper.o \
		../neuraldis/.obj/ndguiserver.o \
		../neuraldis/.obj/ndrangedeviceinfowrapper.o \
		../neuraldis/.obj/robotinfowrapper.o \
		../neuraldis/.obj/sickinfowrapper.o \
		../neuraldis/.obj/sonarinfowrapper.o \
		../neuraldis/.obj/moc_ndguiserver.o
DIST          = /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/shared.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		aria-guiserver.pro
QMAKE_TARGET  = aria-guiserver
DESTDIR       = 
TARGET        = aria-guiserver

first: all
####### Implicit rules

.SUFFIXES: .o .c .cpp .cc .cxx .C

.cpp.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cc.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.cxx.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.C.o:
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o "$@" "$<"

.c.o:
	$(CC) -c $(CFLAGS) $(INCPATH) -o "$@" "$<"

####### Build rules

all: Makefile $(TARGET)

$(TARGET):  $(OBJECTS)  
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJCOMP) $(LIBS)
	{ test -n "$(DESTDIR)" && DESTDIR="$(DESTDIR)" || DESTDIR=.; } && test $$(gdb --version | sed -e 's,[^0-9][^0-9]*\([0-9]\)\.\([0-9]\).*,\1\2,;q') -gt 72 && gdb --nx --batch --quiet -ex 'set confirm off' -ex "save gdb-index $$DESTDIR" -ex quit '$(TARGET)' && test -f $(TARGET).gdb-index && objcopy --add-section '.gdb_index=$(TARGET).gdb-index' --set-section-flags '.gdb_index=readonly' '$(TARGET)' '$(TARGET)' && rm -f $(TARGET).gdb-index || true

Makefile: aria-guiserver.pro  /usr/share/qt4/mkspecs/linux-g++-64/qmake.conf /usr/share/qt4/mkspecs/common/unix.conf \
		/usr/share/qt4/mkspecs/common/linux.conf \
		/usr/share/qt4/mkspecs/common/gcc-base.conf \
		/usr/share/qt4/mkspecs/common/gcc-base-unix.conf \
		/usr/share/qt4/mkspecs/common/g++-base.conf \
		/usr/share/qt4/mkspecs/common/g++-unix.conf \
		/usr/share/qt4/mkspecs/qconfig.pri \
		/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri \
		/usr/share/qt4/mkspecs/features/qt_functions.prf \
		/usr/share/qt4/mkspecs/features/qt_config.prf \
		/usr/share/qt4/mkspecs/features/exclusive_builds.prf \
		/usr/share/qt4/mkspecs/features/default_pre.prf \
		/usr/share/qt4/mkspecs/features/debug.prf \
		/usr/share/qt4/mkspecs/features/default_post.prf \
		/usr/share/qt4/mkspecs/features/warn_on.prf \
		/usr/share/qt4/mkspecs/features/shared.prf \
		/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf \
		/usr/share/qt4/mkspecs/features/qt.prf \
		/usr/share/qt4/mkspecs/features/unix/thread.prf \
		/usr/share/qt4/mkspecs/features/moc.prf \
		/usr/share/qt4/mkspecs/features/resources.prf \
		/usr/share/qt4/mkspecs/features/uic.prf \
		/usr/share/qt4/mkspecs/features/yacc.prf \
		/usr/share/qt4/mkspecs/features/lex.prf \
		/usr/share/qt4/mkspecs/features/include_source_dir.prf \
		/usr/lib/x86_64-linux-gnu/libQt3Support.prl \
		/usr/lib/x86_64-linux-gnu/libQtXml.prl \
		/usr/lib/x86_64-linux-gnu/libQtGui.prl \
		/usr/lib/x86_64-linux-gnu/libQtCore.prl
	$(QMAKE) -o Makefile aria-guiserver.pro
/usr/share/qt4/mkspecs/common/unix.conf:
/usr/share/qt4/mkspecs/common/linux.conf:
/usr/share/qt4/mkspecs/common/gcc-base.conf:
/usr/share/qt4/mkspecs/common/gcc-base-unix.conf:
/usr/share/qt4/mkspecs/common/g++-base.conf:
/usr/share/qt4/mkspecs/common/g++-unix.conf:
/usr/share/qt4/mkspecs/qconfig.pri:
/usr/share/qt4/mkspecs/modules/qt_webkit_version.pri:
/usr/share/qt4/mkspecs/features/qt_functions.prf:
/usr/share/qt4/mkspecs/features/qt_config.prf:
/usr/share/qt4/mkspecs/features/exclusive_builds.prf:
/usr/share/qt4/mkspecs/features/default_pre.prf:
/usr/share/qt4/mkspecs/features/debug.prf:
/usr/share/qt4/mkspecs/features/default_post.prf:
/usr/share/qt4/mkspecs/features/warn_on.prf:
/usr/share/qt4/mkspecs/features/shared.prf:
/usr/share/qt4/mkspecs/features/unix/gdb_dwarf_index.prf:
/usr/share/qt4/mkspecs/features/qt.prf:
/usr/share/qt4/mkspecs/features/unix/thread.prf:
/usr/share/qt4/mkspecs/features/moc.prf:
/usr/share/qt4/mkspecs/features/resources.prf:
/usr/share/qt4/mkspecs/features/uic.prf:
/usr/share/qt4/mkspecs/features/yacc.prf:
/usr/share/qt4/mkspecs/features/lex.prf:
/usr/share/qt4/mkspecs/features/include_source_dir.prf:
/usr/lib/x86_64-linux-gnu/libQt3Support.prl:
/usr/lib/x86_64-linux-gnu/libQtXml.prl:
/usr/lib/x86_64-linux-gnu/libQtGui.prl:
/usr/lib/x86_64-linux-gnu/libQtCore.prl:
qmake:  FORCE
	@$(QMAKE) -o Makefile aria-guiserver.pro

dist: 
	@$(CHK_DIR_EXISTS) ../neuraldis/.obj/aria-guiserver1.0.0 || $(MKDIR) ../neuraldis/.obj/aria-guiserver1.0.0 
	$(COPY_FILE) --parents $(SOURCES) $(DIST) ../neuraldis/.obj/aria-guiserver1.0.0/ && $(COPY_FILE) --parents ../aria-tools/aractiongroupreactive.h ../aria-tools/arianetpacketwrapper.h ../aria-tools/arserverextinforobot.h ../aria-tools/arservermodereactive.h ../aria-tools/bumperinfowrapper.h ../aria-tools/ndguiserver.h ../aria-tools/ndrangedeviceinfowrapper.h ../aria-tools/robotinfowrapper.h ../aria-tools/sickinfowrapper.h ../aria-tools/sonarinfowrapper.h ../neuraldis/.obj/aria-guiserver1.0.0/ && $(COPY_FILE) --parents guiServer.cpp ../aria-tools/aractiongroupreactive.cpp ../aria-tools/arianetpacketwrapper.cpp ../aria-tools/arserverextinforobot.cpp ../aria-tools/arservermodereactive.cpp ../aria-tools/bumperinfowrapper.cpp ../aria-tools/ndguiserver.cpp ../aria-tools/ndrangedeviceinfowrapper.cpp ../aria-tools/robotinfowrapper.cpp ../aria-tools/sickinfowrapper.cpp ../aria-tools/sonarinfowrapper.cpp ../neuraldis/.obj/aria-guiserver1.0.0/ && (cd `dirname ../neuraldis/.obj/aria-guiserver1.0.0` && $(TAR) aria-guiserver1.0.0.tar aria-guiserver1.0.0 && $(COMPRESS) aria-guiserver1.0.0.tar) && $(MOVE) `dirname ../neuraldis/.obj/aria-guiserver1.0.0`/aria-guiserver1.0.0.tar.gz . && $(DEL_FILE) -r ../neuraldis/.obj/aria-guiserver1.0.0


clean:compiler_clean 
	-$(DEL_FILE) $(OBJECTS)
	-$(DEL_FILE) *~ core *.core


####### Sub-libraries

distclean: clean
	-$(DEL_FILE) $(TARGET) 
	-$(DEL_FILE) Makefile


check: first

mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

compiler_moc_header_make_all: moc_ndguiserver.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_ndguiserver.cpp
moc_ndguiserver.cpp: ../aria-tools/ndguiserver.h
	/usr/lib/x86_64-linux-gnu/qt4/bin/moc $(DEFINES) $(INCPATH) ../aria-tools/ndguiserver.h -o moc_ndguiserver.cpp

compiler_rcc_make_all:
compiler_rcc_clean:
compiler_image_collection_make_all: qmake_image_collection.cpp
compiler_image_collection_clean:
	-$(DEL_FILE) qmake_image_collection.cpp
compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean 

####### Compile

../neuraldis/.obj/guiServer.o: guiServer.cpp ../aria-tools/arserverextinforobot.h \
		../aria-tools/arservermodereactive.h \
		../aria-tools/aractiongroupreactive.h \
		../robot/abstractsensor.h \
		../robot/sensorline.h \
		../robot/robotposition.h \
		../robot/robotsprite.h \
		../world/drawings.h \
		../world/rtti.h \
		../robot/spritecollisioner.h \
		../neuraldis/ndmath.h \
		../world/abstractworld.h \
		../world/quadtree.h \
		../world/quadnode.h \
		../world/collisionsolver.h \
		../neuraldis/xmloperator.h \
		../world/lamp.h \
		../world/radialfieldsource.h \
		../world/calculadorbresenham.h \
		../control/inputgenerator.h \
		../neuraldis/measure.hpp \
		../neuraldis/timestamp.hpp \
		../neuraldis/ndpose2d.hpp \
		../neuraldis/ndpose.hpp \
		../neuraldis/ndpose3dquat.hpp \
		../neuraldis/ndpose3dsph.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../neuraldis/.obj/guiServer.o guiServer.cpp

../neuraldis/.obj/aractiongroupreactive.o: ../aria-tools/aractiongroupreactive.cpp ../aria-tools/aractiongroupreactive.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../neuraldis/.obj/aractiongroupreactive.o ../aria-tools/aractiongroupreactive.cpp

../neuraldis/.obj/arianetpacketwrapper.o: ../aria-tools/arianetpacketwrapper.cpp ../aria-tools/arianetpacketwrapper.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../neuraldis/.obj/arianetpacketwrapper.o ../aria-tools/arianetpacketwrapper.cpp

../neuraldis/.obj/arserverextinforobot.o: ../aria-tools/arserverextinforobot.cpp ../aria-tools/arserverextinforobot.h \
		../aria-tools/arservermodereactive.h \
		../aria-tools/aractiongroupreactive.h \
		../robot/abstractsensor.h \
		../robot/sensorline.h \
		../robot/robotposition.h \
		../robot/robotsprite.h \
		../world/drawings.h \
		../world/rtti.h \
		../robot/spritecollisioner.h \
		../neuraldis/ndmath.h \
		../world/abstractworld.h \
		../world/quadtree.h \
		../world/quadnode.h \
		../world/collisionsolver.h \
		../neuraldis/xmloperator.h \
		../world/lamp.h \
		../world/radialfieldsource.h \
		../world/calculadorbresenham.h \
		../control/inputgenerator.h \
		../neuraldis/measure.hpp \
		../neuraldis/timestamp.hpp \
		../neuraldis/ndpose2d.hpp \
		../neuraldis/ndpose.hpp \
		../neuraldis/ndpose3dquat.hpp \
		../neuraldis/ndpose3dsph.hpp
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../neuraldis/.obj/arserverextinforobot.o ../aria-tools/arserverextinforobot.cpp

../neuraldis/.obj/arservermodereactive.o: ../aria-tools/arservermodereactive.cpp ../aria-tools/arservermodereactive.h \
		../aria-tools/aractiongroupreactive.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../neuraldis/.obj/arservermodereactive.o ../aria-tools/arservermodereactive.cpp

../neuraldis/.obj/bumperinfowrapper.o: ../aria-tools/bumperinfowrapper.cpp ../aria-tools/bumperinfowrapper.h \
		../aria-tools/arianetpacketwrapper.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../neuraldis/.obj/bumperinfowrapper.o ../aria-tools/bumperinfowrapper.cpp

../neuraldis/.obj/ndguiserver.o: ../aria-tools/ndguiserver.cpp ../aria-tools/ndguiserver.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../neuraldis/.obj/ndguiserver.o ../aria-tools/ndguiserver.cpp

../neuraldis/.obj/ndrangedeviceinfowrapper.o: ../aria-tools/ndrangedeviceinfowrapper.cpp ../aria-tools/ndrangedeviceinfowrapper.h \
		../aria-tools/arianetpacketwrapper.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../neuraldis/.obj/ndrangedeviceinfowrapper.o ../aria-tools/ndrangedeviceinfowrapper.cpp

../neuraldis/.obj/robotinfowrapper.o: ../aria-tools/robotinfowrapper.cpp ../aria-tools/robotinfowrapper.h \
		../aria-tools/arianetpacketwrapper.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../neuraldis/.obj/robotinfowrapper.o ../aria-tools/robotinfowrapper.cpp

../neuraldis/.obj/sickinfowrapper.o: ../aria-tools/sickinfowrapper.cpp ../aria-tools/sickinfowrapper.h \
		../aria-tools/ndrangedeviceinfowrapper.h \
		../aria-tools/arianetpacketwrapper.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../neuraldis/.obj/sickinfowrapper.o ../aria-tools/sickinfowrapper.cpp

../neuraldis/.obj/sonarinfowrapper.o: ../aria-tools/sonarinfowrapper.cpp ../aria-tools/sonarinfowrapper.h \
		../aria-tools/ndrangedeviceinfowrapper.h \
		../aria-tools/arianetpacketwrapper.h
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../neuraldis/.obj/sonarinfowrapper.o ../aria-tools/sonarinfowrapper.cpp

../neuraldis/.obj/moc_ndguiserver.o: moc_ndguiserver.cpp 
	$(CXX) -c $(CXXFLAGS) $(INCPATH) -o ../neuraldis/.obj/moc_ndguiserver.o moc_ndguiserver.cpp

####### Install

install:   FORCE

uninstall:   FORCE

FORCE:
