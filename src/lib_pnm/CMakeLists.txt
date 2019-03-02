file( GLOB pnm_source_files "*.cpp" )
file( GLOB core_source_files "../core/*.cpp" )
ADD_LIBRARY ( Pnm SHARED
	${pnm_source_files} ${core_source_files}
)