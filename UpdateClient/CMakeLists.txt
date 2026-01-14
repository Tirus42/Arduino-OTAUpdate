cmake_minimum_required (VERSION 3.13)

project (OTAUpdateClient)

# Build the whole project with C++17
set(CMAKE_CXX_STANDARD 17)

find_package(tiruslib CONFIG REQUIRED)

include_directories(${TIRUSLIB_INCLUDE_PATH})
link_directories(${TIRUSLIB_LIB_PATH})

# set global compiler options
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	# Warnings
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Weffc++")

	# Errors
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror=switch -Werror=format -Werror=return-type -Werror=init-self")
endif()

add_executable(OTAUpdateClient
	UpdateClient.cpp
)

target_link_libraries(OTAUpdateClient tiruslib)

if (WIN32)
	target_link_libraries(OTAUpdateClient ws2_32 winmm)
endif ()

install(TARGETS OTAUpdateClient DESTINATION bin)
