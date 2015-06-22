cmake_minimum_required(VERSION 2.8)
project( ballgame )
find_package( OpenCV REQUIRED )

FIND_PACKAGE(OpenMP)
IF (OPENMP_FOUND)
  message ("Found ${OpenMP_C_FLAGS}")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
ELSE (OPENMP_FOUND)
  add_definitions(-DNOOPENMP)
  message("OpenMP not found, deactivating OpenMP code with NOOPENMP")
ENDIF (OPENMP_FOUND)

# SSE support
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -msse3")
endif()

# Check existing files to see which version we need to compile
if (EXISTS "trackgui.h")
  add_definitions(-DMAIN_NR=5)
elseif (EXISTS "calgui.h")
  add_definitions(-DMAIN_NR=4)
elseif (EXISTS "hlgui.h")
  add_definitions(-DMAIN_NR=3)
elseif (EXISTS "hcgui.h")
  add_definitions(-DMAIN_NR=2)
elseif (EXISTS "sobelgui.h")
  add_definitions(-DMAIN_NR=1)
endif (EXISTS "trackgui.h")

file(GLOB ballgame_src *.cc *.h)
add_executable( ballgame ${ballgame_src}	 )
target_link_libraries( ballgame ${OpenCV_LIBS} )
add_custom_command(TARGET ballgame POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/images
$<TARGET_FILE_DIR:ballgame>/images)
