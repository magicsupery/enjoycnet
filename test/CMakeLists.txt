project(test)

aux_source_directory(${PROJECT_SOURCE_DIR} SRC_LIST)

set(LINK_ARGS enjoycnet enjoycco ev glog ${Boost_LIBRARIES})
foreach(var ${SRC_LIST})
	string(REGEX REPLACE ".*/" "" var ${var})
	string(REGEX REPLACE ".cpp" "" tgt ${var})

	set(target ${tgt}.t)
	add_executable(${target} ${var})
	target_link_libraries(${target} ${LINK_ARGS})
endforeach(var)
