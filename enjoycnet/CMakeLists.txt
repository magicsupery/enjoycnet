project(enjoycnet)


macro(add_src_dir name)
	file(GLOB SRC_LIST ${name}/*.h ${name}/*.cpp)
		if(WIN32)
			list(FILTER SRC_LIST EXCLUDE REGEX _linux.cpp)
		else()
			list(FILTER SRC_LIST EXCLUDE REGEX _win.cpp)
		endif()

endmacro()
add_src_dir(core)

set(TARGET ${projectName})
add_library(${TARGET} ${SRC_LIST} )
install(TARGETS ${TARGET} LIBRARY DESTINATION "lib" ARCHIVE DESTINATION "lib")
install(DIRECTORY ${PROJECT_SOURCE_DIR} DESTINATION "include" FILES_MATCHING PATTERN "*.h")



