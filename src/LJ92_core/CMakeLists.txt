file( GLOB lj92core_source_files *.cpp )
add_executable( lj92_core ${lj92core_source_files} )
target_link_libraries( lj92_core ${Libraries} )
