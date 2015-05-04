cmake_minimum_required(VERSION 2.8)
project( cutlery )
find_package( OpenCV REQUIRED )
add_executable( cutlery cutlery.cc )
target_link_libraries( cutlery ${OpenCV_LIBS} )
add_custom_command(TARGET cutlery POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/images
$<TARGET_FILE_DIR:cutlery>/images)
