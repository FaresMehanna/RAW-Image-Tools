file( GLOB lj92eval_source_files *.cpp )
add_executable( lj92_eval ${lj92eval_source_files} )
target_link_libraries( lj92_eval ${Libraries} )
