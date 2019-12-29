# Install script for directory: /Users/ja/onemoretime/assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES
    "/Users/ja/onemoretime/assimp/lib/libassimp.5.0.0.dylib"
    "/Users/ja/onemoretime/assimp/lib/libassimp.5.dylib"
    )
  foreach(file
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.5.0.0.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.5.dylib"
      )
    if(EXISTS "${file}" AND
       NOT IS_SYMLINK "${file}")
      execute_process(COMMAND "/usr/bin/install_name_tool"
        -id "/usr/local/lib/libassimp.5.dylib"
        "${file}")
      execute_process(COMMAND /usr/bin/install_name_tool
        -delete_rpath "/Users/ja/onemoretime/assimp/lib"
        "${file}")
      if(CMAKE_INSTALL_DO_STRIP)
        execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" -x "${file}")
      endif()
    endif()
  endforeach()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/ja/onemoretime/assimp/lib/libassimp.dylib")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.dylib" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.dylib")
    execute_process(COMMAND "/usr/bin/install_name_tool"
      -id "/usr/local/lib/libassimp.5.dylib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.dylib")
    execute_process(COMMAND /usr/bin/install_name_tool
      -delete_rpath "/Users/ja/onemoretime/assimp/lib"
      "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.dylib")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/strip" -x "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.dylib")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "/Users/ja/onemoretime/assimp/code/../include/assimp/anim.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/aabb.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/ai_assert.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/camera.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/color4.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/color4.inl"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/config.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/defs.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/Defines.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/cfileio.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/light.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/material.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/material.inl"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/matrix3x3.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/matrix3x3.inl"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/matrix4x4.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/matrix4x4.inl"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/mesh.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/pbrmaterial.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/postprocess.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/quaternion.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/quaternion.inl"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/scene.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/metadata.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/texture.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/types.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/vector2.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/vector2.inl"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/vector3.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/vector3.inl"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/version.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/cimport.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/importerdesc.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/Importer.hpp"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/DefaultLogger.hpp"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/ProgressHandler.hpp"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/IOStream.hpp"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/IOSystem.hpp"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/Logger.hpp"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/LogStream.hpp"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/NullLogger.hpp"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/cexport.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/Exporter.hpp"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/DefaultIOStream.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/DefaultIOSystem.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/ZipArchiveIOSystem.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/SceneCombiner.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/fast_atof.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/qnan.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/BaseImporter.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/Hash.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/MemoryIOWrapper.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/ParsingUtils.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/StreamReader.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/StreamWriter.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/StringComparison.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/StringUtils.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/SGSpatialSort.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/GenericProperty.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/SpatialSort.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/SkeletonMeshBuilder.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/SmoothingGroups.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/SmoothingGroups.inl"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/StandardShapes.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/RemoveComments.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/Subdivision.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/Vertex.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/LineSplitter.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/TinyFormatter.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/Profiler.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/LogAux.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/Bitmap.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/XMLTools.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/IOStreamBuffer.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/CreateAnimMesh.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/irrXMLWrapper.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/BlobIOSystem.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/MathFunctions.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/Macros.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/Exceptional.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/ByteSwapper.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "/Users/ja/onemoretime/assimp/code/../include/assimp/Compiler/pushpack1.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/Compiler/poppack1.h"
    "/Users/ja/onemoretime/assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

