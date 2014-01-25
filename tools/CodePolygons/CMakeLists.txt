PROJECT(Abetare CXX C)

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
IF(COMMAND cmake_policy)
	CMAKE_POLICY(SET CMP0003 OLD)
	CMAKE_POLICY(SET CMP0005 OLD)
ENDIF(COMMAND cmake_policy)

SET(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

#############################################################################
#Compiler flags for normal (debug) and optimized (release) builds
#
#
#  SET(CMAKE_C_FLAGS, "-std=c++0x")
#  SET(CMAKE_CXX_FLAGS, "-std=c++0x")

IF(NOT MSVC_IDE)

  SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0" CACHE STRING  
      "Flags used by the C++ compiler during debug builds." FORCE)
  SET(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG" 
      CACHE STRING "Flags used by the C++ compiler during release builds." FORCE) 
  SET(CMAKE_CXX_FLAGS_RELWITHDEBINFO "-O3 -g -std=c++0x" CACHE STRING
      "Flags used by the C++ compiler during release builds with debug information." FORCE) 

  SET(CMAKE_C_FLAGS_DEBUG "-g -O0" CACHE STRING
      "Flags used by the C compiler during debug builds." FORCE)
  SET(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG" CACHE STRING
      "Flags used by the C compiler during release builds." FORCE)
  SET(CMAKE_C_FLAGS_RELWITHDEBINFO "-O3 -g" CACHE STRING
      "Flags used by the C compiler during release builds with debug information." FORCE)
ENDIF(NOT MSVC_IDE)

IF(APPLE)
	LIST(APPEND CMAKE_SHARED_LINKER_FLAGS "-single_module")
ENDIF(APPLE)

#ADD_DEFINITIONS(-DDEBUG)

ADD_DEFINITIONS(-D_USE_MATH_DEFINES)

#############################################################################
#Find OpenGL and GLUT
#
INCLUDE(FindOpenGL)
IF(NOT OPENGL_INCLUDE_DIR)
  SET(OPENGL_INCLUDE_DIR GL CACHE STRING "OpenGL directory for header file" FORCE)
ENDIF(NOT OPENGL_INCLUDE_DIR)

INCLUDE(FindGLUT)
IF(NOT GLUT_INCLUDE_DIR)
  SET(GLUT_INCLUDE_DIR GL CACHE STRING "Glut header file" FORCE)
ENDIF(NOT GLUT_INCLUDE_DIR)
IF (NOT GLUT_glut_LIBRARY)
  IF (WIN32 AND NOT CYGWIN)
    SET(GLUT_glut_LIBRARY glut32 CACHE STRING "Glut library for win32" FORCE)
  ENDIF(WIN32 AND NOT CYGWIN)
ENDIF (NOT GLUT_glut_LIBRARY)

INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR} ${GLUT_INCLUDE_DIR})


IF(GLUT_glut_LIBRARY)
  SET(INTERACTIVE_LIBS ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY} ${GLUT_glut_LIBRARY})
ELSE (GLUT_glut_LIBRARY)
  SET(INTERACTIVE_LIBS ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY})
ENDIF(GLUT_glut_LIBRARY)


MESSAGE("
*****************************************************************************
* EXTERNAL LIBRARIES
*   GL     : ${OPENGL_gl_LIBRARY}
*   GLU    : ${OPENGL_glu_LIBRARY}
*   GLUT   : ${GLUT_glut_LIBRARY}
*****************************************************************************
")


#############################################################################
INCLUDE_DIRECTORIES(src)
AUX_SOURCE_DIRECTORY(src/Utils SRC_UTILS)
AUX_SOURCE_DIRECTORY(src/External SRC_EXTERNAL)
AUX_SOURCE_DIRECTORY(src/Programs   SRC_PROGRAMS)


ADD_EXECUTABLE(Abetare
		${SRC_UTILS}
		${SRC_EXTERNAL}
		${SRC_PROGRAMS}		
	      )

TARGET_LINK_LIBRARIES(Abetare ${INTERACTIVE_LIBS})

FIND_LIBRARY(DL_LIB dl /usr/lib/ /usr/local/lib/ ./lib/ ${LIBRARY_OUTPUT_PATH}) 
IF(DL_LIB)
   TARGET_LINK_LIBRARIES(Abetare ${DL_LIB})
ENDIF(DL_LIB)  
