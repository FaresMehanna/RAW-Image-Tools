file( GLOB dng_core_source_files "*.c" )
ADD_LIBRARY ( DngCore SHARED
	${dng_core_source_files}
)

file( GLOB dng_source_files "*.cpp" )
ADD_LIBRARY ( Dng
	${dng_source_files}
)