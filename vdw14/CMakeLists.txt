add_executable(vdw14-tick-counter main.cpp)
target_link_libraries(vdw14-tick-counter PRIVATE cdboost)

# Tests
add_executable(vdw14-tests test_models.cpp)
target_link_libraries(vdw14-tests PRIVATE cdboost Catch2::Catch2WithMain)
add_test(NAME vdw14-tests COMMAND vdw14-tests)
