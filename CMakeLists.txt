
project(piccoloc)

set(LLVM_DIR `llvm-config --cmakedir`)
find_package(LLVM REQUIRED CONFIG)
include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_compile_options(-Wall -Wextra -pedantic -Werror)

add_custom_command(OUTPUT src/ast.h
                   COMMAND python ${CMAKE_SOURCE_DIR}/src/gen/generate_ast_boilerplate.py
                   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

add_custom_command(OUTPUT src/obj.h
                   COMMAND python ${CMAKE_SOURCE_DIR}/src/gen/generate_obj_boilerplate.py
                   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})  

add_executable(piccoloc src/ast_util.c src/compiler/main.c src/package.c src/compiler/scanner.c src/compiler/parser.c src/debug.c src/compiler/rtlib.c src/compiler/types.c src/compiler/compiler.c src/compiler/analysis_pass.c src/compiler/builders/common.c src/compiler/builders/obj_builders.c src/compiler/builders/literal.c src/compiler/builders/var.c src/compiler/builders/unary.c src/compiler/builders/binary.c src/compiler/builders/block.c src/compiler/builders/call.c src/compiler/builders/quote.c src/compiler/builders/eval.c src/compiler/builders/var_decl.c src/compiler/builders/fn.c  src/ast.h src/obj.h src/compiler/var_find.c src/compiler/chrono.c)

llvm_map_components_to_libnames(llvm_libs `llvm-config --components all`)
target_link_libraries(piccoloc ${llvm_libs})
message(STATUS ${llvm_libs})
install(TARGETS piccoloc DESTINATION bin)

project(piccrtlib)
add_library(piccrtlib src/eval.c src/rtlib/rtlib.c src/package.c src/value.c src/quote.c src/obj.c src/rtlib/eval.c src/eval/literal.c src/eval/var.c src/eval/unary.c src/eval/binary.c src/eval/block.c src/eval/call.c src/eval/quote.c src/eval/eval.c src/eval/var_decl.c src/eval/fn.c src/ast.h src/ast_util.c)
install(TARGETS piccrtlib DESTINATION ~/Library/Application\ Support/piccolo/rtlib)
