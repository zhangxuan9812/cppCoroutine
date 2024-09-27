file(REMOVE_RECURSE
  "../lib/libcppCoroutine.a"
  "../lib/libcppCoroutine.pdb"
)

# Per-language clean rules from dependency scanning.
foreach(lang CXX)
  include(CMakeFiles/cppCoroutine.dir/cmake_clean_${lang}.cmake OPTIONAL)
endforeach()
