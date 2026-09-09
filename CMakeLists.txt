cmake_minimum_required(VERSION 3.20)

find_program(CLANG_EXE clang REQUIRED)
find_program(CLANGXX_EXE clang++ REQUIRED)
set(CMAKE_C_COMPILER ${CLANG_EXE})
set(CMAKE_CXX_COMPILER ${CLANGXX_EXE})

project(MyLLVMProject LANGUAGES CXX)

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

add_library(pass MODULE
    src/Hello.cpp
)

set_target_properties(pass PROPERTIES PREFIX "")
target_compile_features(pass PRIVATE cxx_std_17)
target_include_directories(pass PRIVATE ${LLVM_INCLUDE_DIR})
separate_arguments(LLVM_CXX_FLAGS_LIST UNIX_COMMAND "${LLVM_CXX_FLAGS}")
target_compile_options(pass PRIVATE ${LLVM_CXX_FLAGS_LIST})
target_link_options(pass PRIVATE ${LLVM_LD_FLAGS})
target_link_libraries(pass PRIVATE ${LLVM_SYSTEM_LIBS})
