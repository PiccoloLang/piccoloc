
project(piccoloc)

set(LLVM_DIR "/usr/local/Cellar/llvm/13.0.0_2/lib/cmake/llvm")
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(piccoloc src/compiler/main.c src/package.c src/compiler/scanner.c src/compiler/parser.c src/debug.c src/compiler/rtlib.c src/compiler/compiler.c src/compiler/builders/common.c src/compiler/builders/literal.c src/compiler/builders/unary.c src/compiler/builders/binary.c)
llvm_map_components_to_libnames(llvm_libs support core codegen)
target_link_libraries(piccoloc ${llvm_libs})
install(TARGETS piccoloc DESTINATION bin)

project(piccrtlib)
add_library(piccrtlib src/rtlib/rtlib.c src/package.c src/value.c)
install(TARGETS piccrtlib DESTINATION ~/Library/Application\ Support/piccolo/rtlib)