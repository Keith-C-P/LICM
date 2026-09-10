cmake_minimum_required(VERSION 3.20)

find_program(CLANG_EXE clang REQUIRED)
find_program(CLANGXX_EXE clang++ REQUIRED)
find_program(OPT_EXE opt REQUIRED)
set(CMAKE_C_COMPILER ${CLANG_EXE})
set(CMAKE_CXX_COMPILER ${CLANGXX_EXE})

project(MyLICM LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_program(LLVM_CONFIG_EXE llvm-config REQUIRED)

execute_process(COMMAND ${LLVM_CONFIG_EXE} --includedir
    OUTPUT_VARIABLE LLVM_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG_EXE} --cxxflags
    OUTPUT_VARIABLE LLVM_CXX_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG_EXE} --ldflags
    OUTPUT_VARIABLE LLVM_LD_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG_EXE} --system-libs
    OUTPUT_VARIABLE LLVM_SYSTEM_LIBS OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "LLVM include dir: ${LLVM_INCLUDE_DIR}")

file(GLOB_RECURSE PASSES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

foreach(PASS ${PASSES})
  cmake_path(GET PASS FILENAME PASS_FILE)
  cmake_path(GET PASS_FILE STEM PASS_NAME)

  add_library(${PASS_NAME}_pass MODULE
    ${PASS}
  )

  target_compile_features(${PASS_NAME}_pass PRIVATE cxx_std_17)
  target_include_directories(${PASS_NAME}_pass PRIVATE ${LLVM_INCLUDE_DIR})

  separate_arguments(LLVM_CXX_FLAGS_LIST UNIX_COMMAND "${LLVM_CXX_FLAGS}")

  target_compile_options(${PASS_NAME}_pass PRIVATE ${LLVM_CXX_FLAGS_LIST})
  target_link_options(${PASS_NAME}_pass PRIVATE ${LLVM_LD_FLAGS})
  target_link_libraries(${PASS_NAME}_pass PRIVATE ${LLVM_SYSTEM_LIBS})

endforeach()

file(GLOB_RECURSE C_TESTS
    "${CMAKE_SOURCE_DIR}/tests/C/*.c"
)

file(GLOB_RECURSE IR_TESTS
    "${CMAKE_SOURCE_DIR}/tests/IR/*.ll"
)

file(GLOB_RECURSE PASSES
    "${CMAKE_SOURCE_DIR}/src/*.cpp"
)

add_custom_target(test-c)
add_custom_target(test-ir)

foreach(PASS ${PASSES})
  cmake_path(GET PASS FILENAME PASS_FILE)
  cmake_path(GET PASS_FILE STEM PASS_NAME)
  message(STATUS "PASS NAME: ${PASS_NAME}")

  foreach(C_TEST ${C_TESTS})

    get_filename_component(TEST_NAME
      ${C_TEST}
      NAME_WE
    )

    add_custom_target(
      test-c-${TEST_NAME}-${PASS_NAME}

      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_BINARY_DIR}/test-results/C

      COMMAND ${CLANG_EXE}
      -S
      -emit-llvm
      -O0
      -Xclang -disable-O0-optnone
      -Wno-\#warnings
      ${C_TEST}
      -o ${CMAKE_BINARY_DIR}/test-results/C/${TEST_NAME}.ll

      COMMAND ${OPT_EXE}
      -load-pass-plugin $<TARGET_FILE:${PASS_NAME}_pass>
      -passes="${PASS_NAME}"
      ${CMAKE_BINARY_DIR}/test-results/C/${TEST_NAME}.ll
      -S
      -o ${CMAKE_BINARY_DIR}/test-results/C/${TEST_NAME}.out.ll

      DEPENDS ${PASS_NAME}_pass

      COMMENT "Running LLVM ${PASS_NAME} pass on ${TEST_NAME}.c"
    )

    add_dependencies(test-c test-c-${TEST_NAME}-${PASS_NAME})

  endforeach()


  foreach(IR_TEST ${IR_TESTS})

    get_filename_component(TEST_NAME
      ${IR_TEST}
      NAME_WE
    )

    cmake_path(GET IR_TEST PARENT_PATH TEST_PARENT_DIR)
    cmake_path(GET TEST_PARENT_DIR FILENAME TEST_PARENT_DIR_NAME)

    add_custom_target(
      test-ir-${TEST_PARENT_DIR_NAME}-${TEST_NAME}-${PASS_NAME}

      COMMAND ${CMAKE_COMMAND} -E make_directory
      ${CMAKE_BINARY_DIR}/test-results/IR

      COMMAND ${OPT_EXE}
      -load-pass-plugin $<TARGET_FILE:${PASS_NAME}_pass>
      -passes="${PASS_NAME}"
      ${IR_TEST}
      -S
      -o ${CMAKE_BINARY_DIR}/test-results/IR/${TEST_NAME}.out.ll

      DEPENDS ${PASS_NAME}_pass

      COMMENT "Running ${PASS_NAME} pass on ${TEST_NAME}.ll"
    )

    add_dependencies(test-ir test-ir-${TEST_PARENT_DIR_NAME}-${TEST_NAME}-${PASS_NAME})

  endforeach()
endforeach()

add_custom_target(test)
add_dependencies(test test-c test-ir)
