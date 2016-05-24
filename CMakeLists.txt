project(ObjectRenderer)
cmake_minimum_required(VERSION 3.2)

set(CMAKE_MODULE_PATH
	"${ObjectRenderer_SOURCE_DIR}"
	"${ObjectRenderer_SOURCE_DIR}/cmake"
	"${ObjectRenderer_SOURCE_DIR}/cmake/Utils"
	"${ObjectRenderer_SOURCE_DIR}/cmake/Packages"
	"${ObjectRenderer_SOURCE_DIR}/cmake/SpecializedConfig"
)

INCLUDE(PlatformSpecific)
INCLUDE(Configuration)
SET(ObjectRenderer_INCLUDE_DIR "${ObjectRenderer_SOURCE_DIR}/include")
include_directories("${ObjectRenderer_INCLUDE_DIR}")

# Find dependencies
include(Dependencies)

set(OBJECTRENDERER_SRCS
	"main.cpp"
	"Renderer.cpp"
	"logger.cpp"
	)
find_package(OpenCL REQUIRED)


if (NOT Boost_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required Boost files - Please check ${BOOST_SEARCH_PATH}")
endif()
MESSAGE(${Boost_LIBRARIES})

if (NOT OPENIMAGEIO_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required OpenImageIO files - Please check ${OPENIMAGEIO_SEARCH_PATH}")
endif()

if (NOT OPENCL_FOUND AND NOT LUXRAYS_DISABLE_OPENCL)
	MESSAGE(ERROR "--> Could not locate required OpenCL files, disabling OpenCL support - Please check ${OPENCL_SEARCH_PATH}")
	SET(LUXRAYS_DISABLE_OPENCL 1)
endif()

if (NOT OPENGL_FOUND AND NOT LUXRAYS_DISABLE_OPENCL)
	MESSAGE(ERROR "--> Could not locate required OpenGL files, disabling OpenCL support, disabling samples build")
	SET(LUXRAYS_DISABLE_OPENCL 1)
endif()

if (NOT GLEW_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required GLEW files, disabling samples build - Please check ${GLEW_SEARCH_PATH}")
endif()

if (NOT GLUT_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required GLUT files, disabling samples build - Please check ${GLUT_SEARCH_PATH}")
endif()

if (NOT EMBREE_FOUND)
	MESSAGE(FATAL_ERROR "--> Could not locate required Intel Embree files - Please check ${EMBREE_SEARCH_PATH}")
endif()

if(NOT WIN32 AND NOT APPLE)
	ADD_DEFINITIONS(-msse2 -mfpmath=sse -ftree-vectorize -funroll-loops -Wall -fPIC -DHAVE_PTHREAD_H)
endif()

if (OPENCL_FOUND)
	include_directories(BEFORE SYSTEM ${OPENCL_INCLUDE_DIR} ${OPENCL_C_INCLUDE_DIR})
endif()


link_directories("${ObjectRenderer_SOURCE_DIR}/lib")
include_directories("include")

add_executable(objectrenderer ${OBJECTRENDERER_SRCS})
add_definitions(${VISIBILITY_FLAGS})

TARGET_LINK_LIBRARIES(
	objectrenderer 
	luxcore 
	smallluxgpu 
	luxrays
	${OPENEXR_LIBRARIES} 
	${EMBREE_LIBRARY} 
	${Boost_LIBRARIES} 
	${OPENGL_gl_LIBRARY} 
	${OPENIMAGEIO_LIBRARIES} 
	${OPENCL_LIBRARIES}  
	)