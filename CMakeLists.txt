cmake_minimum_required(VERSION 2.6)

PROJECT(Pobvnc)
SET (AUTHOR "Fabio Di Matteo" INTERNAL "Author")
SET (VERSION "0.1.0")

INCLUDE (FindPkgConfig)

#Controlla se c'e' il programma pkg-config
IF (NOT PKG_CONFIG_FOUND)
	MESSAGE (FATAL_ERROR "pkg-config not found...")
ENDIF (NOT PKG_CONFIG_FOUND)



#Controlla se ci sono le Gtk+-2.0
pkg_check_modules (GTK REQUIRED gtk+-2.0>=2.24)
IF (NOT GTK_FOUND)
	MESSAGE(FATAL_ERROR "You don't seem to have gtk2 >= 2.24 development libraries installed...")
ENDIF (NOT GTK_FOUND)

#Controlla se ci sono le Gio-2.0
pkg_check_modules (GIO REQUIRED gio-2.0>=2.0)

IF (NOT GIO_FOUND)
	MESSAGE(FATAL_ERROR "You don't seem to have GIO-2 >= 2.0 development libraries installed...")
ENDIF (NOT GIO_FOUND)

ADD_DEFINITIONS()


#Include
INCLUDE_DIRECTORIES (. ${GTK_INCLUDE_DIRS})

#Lincaggio alle librerie
LINK_DIRECTORIES (${GTK_LIBRARY_DIRS} )
LINK_LIBRARIES (${GTK_LIBRARIES} )

#Generating resources on Linux
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	
	EXECUTE_PROCESS(
		WORKING_DIRECTORY source
		COMMAND glib-compile-resources --generate-source linux64-resource.xml
	)
	 
	EXECUTE_PROCESS(
		WORKING_DIRECTORY source		
		COMMAND glib-compile-resources --generate-source linux32-resource.xml
	)
    
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")



#Indichiamo dove sara' messo l'eseguibile
ADD_EXECUTABLE(pobvnc source/linux64-resource.c source/callbacks.c source/utils.c source/main.c)

#Se si ha bisogno di eventuali dipendenze fornite da noi stessi
#ADD_DEPENDENCIES (pobvnc source/linux64-resource.c)
#ADD_DEPENDENCIES (pobvnc source/callbacks.c )
#ADD_DEPENDENCIES (pobvnc source/utils.c )

#Variabili per le directory di installazione
#ADD_DEFINITIONS (-DVERSION=\\\"${VERSION}\\\")
#ADD_DEFINITIONS (-DDATADIR=\\\"${CMAKE_INSTALL_PREFIX}/share\\\")


#Copia file per l'installazione
#ADD_SUBDIRECTORY (src)

# for uninstall use: 'xargs rm < install_manifest.txt'
SET(CMAKE_INSTALL_PREFIX /usr)
INSTALL (TARGETS pobvnc DESTINATION bin)
INSTALL (FILES source/pobvnc.desktop DESTINATION share/applications/)
INSTALL (FILES source/lifesaver.svg DESTINATION share/icons/)

#Binary installer (run 'cpack --config CPackConfig.cmake' if you want binary pack)
#INCLUDE(CPack)
