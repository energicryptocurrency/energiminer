find_package(CUDA REQUIRED)

list(APPEND CUDA_NVCC_FLAGS "--ptxas-options=-v,-warn-lmem-usage")
list(APPEND CUDA_NVCC_FLAGS "--generate-line-info")
list(APPEND CUDA_NVCC_FLAGS "--use_fast_math")
list(APPEND CUDA_NVCC_FLAGS "--default-stream=per-thread")

if (NOT MSVC)
	list(APPEND CUDA_NVCC_FLAGS "-Xcompiler=-Wno-pedantic")
endif()

list(APPEND CUDA_NVCC_FLAGS_RELEASE -O3)
list(APPEND CUDA_NVCC_FLAGS_DEBUG -G)

if(COMPUTE AND (COMPUTE GREATER 0))
	list(APPEND CUDA_NVCC_FLAGS "-gencode arch=compute_${COMPUTE},code=sm_${COMPUTE}")
else()
	set(
		CUDA_NVCC_FLAGS
		${CUDA_NVCC_FLAGS}
		"-gencode arch=compute_30,code=sm_30"
		"-gencode arch=compute_35,code=sm_35"
		"-gencode arch=compute_50,code=sm_50"
		"-gencode arch=compute_52,code=sm_52"
		"-gencode arch=compute_53,code=sm_53"
		"-gencode arch=compute_60,code=sm_60"
		"-gencode arch=compute_61,code=sm_61"
		"-gencode arch=compute_62,code=sm_62"
		"-gencode arch=compute_70,code=sm_70"
	)

	if(NOT CUDA_VERSION VERSION_LESS 10)
        set(
            CUDA_NVCC_FLAGS
            ${CUDA_NVCC_FLAGS}
            "-gencode arch=compute_75,code=sm_75"
        )
	endif()
endif()

file(GLOB sources "*.cpp" "*.cu")
file(GLOB headers "*.h" "*.cuh")

cuda_add_library(nrghash-cuda STATIC ${sources} ${headers})
target_include_directories(nrghash-cuda PUBLIC ${CUDA_INCLUDE_DIRS})
target_include_directories(nrghash-cuda PRIVATE .. ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(nrghash-cuda Boost::system Boost::filesystem)
