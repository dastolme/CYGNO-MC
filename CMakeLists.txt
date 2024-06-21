# $Id: CMakeLists.txt 68058 2013-03-13 14:47:43Z gcosmo $

#----------------------------------------------------------------------------
# Setup the project
cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(CYGNO)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# You can set WITH_GEANT4_UIVIS to OFF via the command line or ccmake/cmake-gui
# to build a batch mode only executable
#
option(WITH_GEANT4_UIVIS "Build example with Geant4 UI and Vis drivers" ON)
if(WITH_GEANT4_UIVIS)
  find_package(Geant4 REQUIRED ui_all vis_all)
else()
  find_package(Geant4 REQUIRED)
endif()

#----------------------------------------------------------------------------
# Setup Geant4 include directories and compile definitions
# Setup include directory for this project
#
include(${Geant4_USE_FILE})
include_directories(${PROJECT_SOURCE_DIR}/include)

# Include CADMesh in your project.
find_package(cadmesh)
include_directories(${CADMESH_INCLUDE_DIRS})

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Include root
find_package(ROOT)
if(NOT ROOT_FOUND)
       message(WARNING "ROOT not found")
else()
	message(STATUS "ROOT version:" ${ROOT_VERSION})
	if(ROOT_VERSION VERSION_LESS "6.5")
		include_directories(${ROOT_INCLUDE_DIR})
		message(STATUS "ROOT include dirs:" ${ROOT_INCLUDE_DIR})
	else()
		include_directories(${ROOT_INCLUDE_DIRS})
		message(STATUS "ROOT include dirs:" ${ROOT_INCLUDE_DIRS})
	endif()
endif()

#----------------------------------------------------------------------------
# Add the executable, and link it to the Geant4 libraries
#
add_executable(CYGNO CYGNO.cc ${sources} ${headers})
target_link_libraries(CYGNO ${Geant4_LIBRARIES})
if(ROOT_FOUND)
	target_link_libraries(CYGNO ${ROOT_LIBRARIES})
endif()
target_link_libraries(CYGNO ${cadmesh_LIBRARIES})
#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build B1. This is so that we can run the executable directly because it
# relies on these scripts being in the current working directory.
#
set(CYGNO_SCRIPTS
vis.mac
 )
#
foreach(_script ${CYGNO_SCRIPTS})
  configure_file(
    ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()

## debug
set(CMAKE_BUILD_TYPE Debug)

#----------------------------------------------------------------------------
# For internal Geant4 use - but has no effect if you build this
# example standalone
#
#add_custom_target(CYGNO DEPENDS CYGNO)

#----------------------------------------------------------------------------
# Install the executable to 'bin' directory under CMAKE_INSTALL_PREFIX
#
install(TARGETS CYGNO DESTINATION bin)


