cmake_minimum_required( VERSION 2.8.11)
set ( CMAKE_BUILD_TYPE Debug )
add_definitions ( -Wall )
find_package ( Qt5 REQUIRED )
include ( ${QT_USE_FILE} )
include_directories (
	${CMAKE_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} ${QT_QTXML_INCLUDE_DIR}
    ${QT_QTPRINTSUPPORT_INCLUDE_DIR} ${QT_QTSVG_INCLUDE_DIR}
	/usr/local/Arnl/include
	/usr/local/Aria/ArNetworking/include
	/usr/local/Aria/include)
set ( aria-guiserver_HDRS
	../aria-tools/aractiongroupreactive.h
	../aria-tools/arianetpacketwrapper.h
	../aria-tools/arserverextinforobot.h
	../aria-tools/arservermodereactive.h
	../aria-tools/bumperinfowrapper.h
	../aria-tools/ndrangedeviceinfowrapper.h
	../aria-tools/robotinfowrapper.h
	../aria-tools/sickinfowrapper.h
	../aria-tools/sonarinfowrapper.h
	)

set ( aria-guiserver_SRCS
	guiServer.cpp
	../aria-tools/aractiongroupreactive.cpp
	../aria-tools/arianetpacketwrapper.cpp
	../aria-tools/arserverextinforobot.cpp
	../aria-tools/arservermodereactive.cpp
	../aria-tools/bumperinfowrapper.cpp
	../aria-tools/ndrangedeviceinfowrapper.cpp
	../aria-tools/robotinfowrapper.cpp
	../aria-tools/sickinfowrapper.cpp
	../aria-tools/sonarinfowrapper.cpp
	)

QT4_WRAP_CPP(MOCS ${aria-guiserver_MOCS})

add_executable(aria-guiserver ${aria-guiserver_SRCS} ${UIS} ${RSCS} ${TRS} ${MOCS})
#target_link_libraries ( aria-guiserver  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
#       ${QT_QTXML_LIBRARY} ${QT_QT3SUPPORT_LIBRARY}
#        /usr/local/Aria/lib/libAria.so /usr/local/Aria/lib/libArNetworking.so
#        /usr/local/Arnl/lib/libBaseArnl.so /usr/local/Arnl/lib/libArnl.so)
target_link_libraries ( aria-guiserver  ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY}
        ${QT_QTXML_LIBRARY}
        /usr/local/Arnl/lib/libAriaForArnl.so /usr/local/Arnl/lib/libArNetworkingForArnl.so
        /usr/local/Arnl/lib/libBaseArnl.so /usr/local/Arnl/lib/libArnl.so)
