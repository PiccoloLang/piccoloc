
project(piccoloc)

set(LLVM_DIR "/usr/local/Cellar/llvm/13.0.0_2/lib/cmake/llvm")
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_compile_options(-Wall -Wextra -pedantic -Werror)

add_custom_command(OUTPUT src/ast.h
                   COMMAND python ${CMAKE_SOURCE_DIR}/src/gen/generate_ast_boilerplate.py
                   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_executable(piccoloc src/compiler/main.c src/package.c src/compiler/scanner.c src/compiler/parser.c src/debug.c src/compiler/rtlib.c src/compiler/compiler.c src/compiler/builders/common.c src/compiler/builders/literal.c src/compiler/builders/var.c src/compiler/builders/unary.c src/compiler/builders/binary.c src/compiler/builders/block.c src/compiler/builders/quote.c src/compiler/builders/eval.c src/compiler/builders/var_decl.c  src/ast.h src/compiler/var_find.c)
llvm_map_components_to_libnames(llvm_libs support core codegen)
target_link_libraries(piccoloc ${llvm_libs})
install(TARGETS piccoloc DESTINATION bin)

project(piccrtlib)
add_library(piccrtlib src/rtlib/rtlib.c src/package.c src/value.c src/quote.c src/obj.c src/rtlib/eval.c src/eval.c src/eval/literal.c src/eval/unary.c src/eval/binary.c src/eval/block.c src/eval/quote.c src/eval/eval.c src/ast.h)
install(TARGETS piccrtlib DESTINATION ~/Library/Application\ Support/piccolo/rtlib)

project(test)
add_executable(test src/test/test.c src/obj.c src/quote.c src/rtlib/rtlib.c src/value.c)