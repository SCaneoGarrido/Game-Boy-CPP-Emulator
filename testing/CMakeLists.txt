# Arnes compartido por todas las suites
add_library(test_harness STATIC common/TestHarness.cpp)
target_link_libraries(test_harness PUBLIC dmg_core)
target_include_directories(test_harness PUBLIC common)

# Una suite = testing/<suite>/test_<suite>.cpp + checklist_<suite>.txt
set(DMG_TEST_SUITES
  boot
  ld ld16 inc dec add sub and add16
  logic ld16imm ldsp stack ldh mem
)

foreach(suite IN LISTS DMG_TEST_SUITES)
  add_executable(test_${suite} ${suite}/test_${suite}.cpp)
  target_link_libraries(test_${suite} PRIVATE test_harness)
  add_test(NAME ${suite} COMMAND test_${suite})
endforeach()
