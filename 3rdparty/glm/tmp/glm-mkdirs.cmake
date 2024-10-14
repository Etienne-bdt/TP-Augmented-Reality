# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/home/etienne/tpAR-v2023.1.0-rc4/3rdparty/glm/src/glm")
  file(MAKE_DIRECTORY "/home/etienne/tpAR-v2023.1.0-rc4/3rdparty/glm/src/glm")
endif()
file(MAKE_DIRECTORY
  "/home/etienne/tpAR-v2023.1.0-rc4/3rdparty/glm/src/glm-build"
  "/home/etienne/tpAR-v2023.1.0-rc4/3rdparty/glm/build"
  "/home/etienne/tpAR-v2023.1.0-rc4/3rdparty/glm/tmp"
  "/home/etienne/tpAR-v2023.1.0-rc4/3rdparty/glm/src/glm-stamp"
  "/home/etienne/tpAR-v2023.1.0-rc4/3rdparty/glm/src"
  "/home/etienne/tpAR-v2023.1.0-rc4/3rdparty/glm/src/glm-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/etienne/tpAR-v2023.1.0-rc4/3rdparty/glm/src/glm-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/etienne/tpAR-v2023.1.0-rc4/3rdparty/glm/src/glm-stamp${cfgdir}") # cfgdir has leading slash
endif()
