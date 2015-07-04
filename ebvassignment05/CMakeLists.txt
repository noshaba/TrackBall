cmake_minimum_required(VERSION 2.8)
project( gpuSobel )
find_package( CUDA REQUIRED)
find_package( OpenCV REQUIRED )

if (CUDA_FOUND)
	message("CUDA found!")
	set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} -O3 -lineinfo -gencode arch=compute_20,code=sm_20 -gencode=arch=compute_30,code=sm_30 -gencode=arch=compute_35,code=sm_35 -gencode=arch=compute_35,code=compute_35)

	set(CUDASRCS 
		main.cu
	)

	CUDA_ADD_EXECUTABLE(gpuSobel ${CUDASRCS})

else()
	message("CUDA not found, abort")
endif()

set(CMAKE_CXX_FLAGS "-g -Wall")

# SSE support
if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -msse -msse2 -msse3")
endif()

target_link_libraries( gpuSobel ${OpenCV_LIBS} )


add_custom_command(TARGET gpuSobel POST_BUILD
   COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/images
$<TARGET_FILE_DIR:gpuSobel>/images)
