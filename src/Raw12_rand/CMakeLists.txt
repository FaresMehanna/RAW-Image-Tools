file( GLOB raw12rand_source_files *.cpp )
add_executable( raw12_rand ${raw12rand_source_files} )
target_link_libraries( raw12_rand ${Libraries} )
