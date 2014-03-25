project(parsing_foo)

find_program(LEX_EXE
    flex
)
if(LEX_EXE STREQUAL "LEX_EXE-NOTFOUND")
    message(FATAL_ERROR "dear user, plase install flex!")
endif(LEX_EXE STREQUAL "LEX_EXE-NOTFOUND")

find_program(YACC_EXE
    bison
)
if(YACC_EXE STREQUAL "YACC_EXE-NOTFOUND")
    message(FATAL_ERROR "dear user, plase install bison!")
endif(YACC_EXE STREQUAL "YACC_EXE-NOTFOUND")

# reuseable cmake macro for yacc
MACRO(YACC_FILE _filename)
    GET_FILENAME_COMPONENT(_basename ${_filename} NAME_WE)
    ADD_CUSTOM_COMMAND(
        OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.c
                ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.h
        COMMAND ${YACC_EXE} -d
                -o ${_basename}.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/${_filename}
        DEPENDS ${_filename}
    )
ENDMACRO(YACC_FILE)

# reuseable cmake macro for lex
MACRO(LEX_FILE _filename)
    GET_FILENAME_COMPONENT(_basename ${_filename} NAME_WE)
    ADD_CUSTOM_COMMAND(
        OUTPUT  ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.c
        COMMAND ${LEX_EXE}
                -o  ${_basename}.c 
                ${CMAKE_CURRENT_SOURCE_DIR}/${_filename}
        DEPENDS ${_filename} )
ENDMACRO(LEX_FILE)

YACC_FILE(Parser.y)
LEX_FILE(Lexer.l)

#add_executable(parser
#    Expression.c
#    Parser.c
#    Lexer.c
#)

ADD_LIBRARY(parser Expression.c  Parser.c Lexer.c)

