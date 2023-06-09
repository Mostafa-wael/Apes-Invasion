# - Try to find the Bullet physics engine
#
#  This module defines the following variables
#
#  BULLET_FOUND - Was bullet found
#  BULLET_INCLUDE_DIRS - the Bullet include directories
#  BULLET_LIBRARIES - Link to this, by default it includes bullet components
#
#  This module accepts the following variables
#
#  BULLET_ROOT - Can be set to bullet install path or Windows build path
#

#=============================================================================
# Copyright 2009 Kitware, Inc.
# Copyright 2009 Philip Lowman <philip@yhbt.com>
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

macro(_FIND_BULLET_LIBRARY _var)
  find_library(${_var}
     NAMES 
        ${ARGN}
     HINTS
        ${BULLET_ROOT}
        ${BULLET_ROOT}/out/release8/libs
        ${BULLET_ROOT}/out/debug8/libs
        ${BULLET_ROOT}/lib
     PATH_SUFFIXES lib
  )
  mark_as_advanced(${_var})
endmacro()

macro(_BULLET_APPEND_LIBRARIES _list _release)
   set(_debug ${_release}_DEBUG)
   if(${_debug})
      set(${_list} ${${_list}} optimized ${${_release}} debug ${${_debug}})
   else()
      set(${_list} ${${_list}} ${${_release}})
   endif()
endmacro()

find_path(BULLET_INCLUDE_DIR NAMES btBulletCollisionCommon.h
  HINTS
    ${BULLET_ROOT}/include
    ${BULLET_ROOT}/src
  PATH_SUFFIXES bullet
)

# Find the libraries

_FIND_BULLET_LIBRARY(BULLET_DYNAMICS_LIBRARY            BulletDynamics)
_FIND_BULLET_LIBRARY(BULLET_DYNAMICS_LIBRARY_DEBUG      BulletDynamics_Debug BulletDynamics_d)
_FIND_BULLET_LIBRARY(BULLET_COLLISION_LIBRARY           BulletCollision)
_FIND_BULLET_LIBRARY(BULLET_COLLISION_LIBRARY_DEBUG     BulletCollision_Debug BulletCollision_d)
_FIND_BULLET_LIBRARY(BULLET_MATH_LIBRARY                BulletMath LinearMath)
_FIND_BULLET_LIBRARY(BULLET_MATH_LIBRARY_DEBUG          BulletMath_Debug BulletMath_d LinearMath_d)
_FIND_BULLET_LIBRARY(BULLET_FILELOADER_LIBRARY          BulletFileLoader)
_FIND_BULLET_LIBRARY(BULLET_FILELOADER_LIBRARY_DEBUG    BulletFileLoader_Debug BulletFileLoader_d)
_FIND_BULLET_LIBRARY(BULLET_WORLDIMPORTER_LIBRARY       BulletWorldImporter)
_FIND_BULLET_LIBRARY(BULLET_WORLDIMPORTER_LIBRARY_DEBUG BulletWorldImporter_Debug BulletWorldImporter_d)

# handle the QUIETLY and REQUIRED arguments and set BULLET_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Bullet DEFAULT_MSG
    BULLET_DYNAMICS_LIBRARY BULLET_COLLISION_LIBRARY BULLET_MATH_LIBRARY
    BULLET_FILELOADER_LIBRARY BULLET_WORLDIMPORTER_LIBRARY BULLET_INCLUDE_DIR)

set(BULLET_INCLUDE_DIRS ${BULLET_INCLUDE_DIR})

# we only need dynamics, collision, math and worldimporter

if(BULLET_FOUND)
   _BULLET_APPEND_LIBRARIES(BULLET_LIBRARIES BULLET_FILELOADER_LIBRARY)
   _BULLET_APPEND_LIBRARIES(BULLET_LIBRARIES BULLET_WORLDIMPORTER_LIBRARY)
   _BULLET_APPEND_LIBRARIES(BULLET_LIBRARIES BULLET_DYNAMICS_LIBRARY)
   _BULLET_APPEND_LIBRARIES(BULLET_LIBRARIES BULLET_COLLISION_LIBRARY)
   _BULLET_APPEND_LIBRARIES(BULLET_LIBRARIES BULLET_MATH_LIBRARY)
endif()