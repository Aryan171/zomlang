llvm_map_components_to_libnames(llvm_libs core orcjit native)

add_executable(zomlang
    Lexer.cpp
    Parser.cpp
    CodeGen.cpp
    main.cpp
    CompilerState.cpp
    Functions.cpp
)

target_link_libraries(zomlang PRIVATE ${llvm_libs})

target_link_options(zomlang PRIVATE
    -rdynamic
)