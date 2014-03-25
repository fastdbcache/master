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
        OUTPUT  ${PROJECT_SOURCE_DIR}/${_basename}.c
                ${PROJECT_SOURCE_DIR}/${_basename}.h               
        COMMAND ${YACC_EXE} -d
                ARGS --output=${PROJECT_SOURCE_DIR}/${_basename}.c 
                --defines=${PROJECT_SOURCE_DIR}/${_basename}.h
                ${CMAKE_CURRENT_SOURCE_DIR}/${_filename}
        DEPENDS ${_filename}
        COMMENT "me BIOS Generating ${PROJECT_SOURCE_DIR}/${_basename}.c"
    )
ENDMACRO(YACC_FILE)

# reuseable cmake macro for lex
MACRO(LEX_FILE _filename)
    GET_FILENAME_COMPONENT(_basename ${_filename} NAME_WE)
    ADD_CUSTOM_COMMAND(
        OUTPUT  ${PROJECT_SOURCE_DIR}/${_basename}.c
        OUTPUT  ${PROJECT_SOURCE_DIR}/${_basename}.h
        COMMAND ${LEX_EXE}
               ARGS  --outfile=${PROJECT_SOURCE_DIR}/${_basename}.c 
               --header-file=${PROJECT_SOURCE_DIR}/${_basename}.h
                ${CMAKE_CURRENT_SOURCE_DIR}/${_filename}
        DEPENDS ${_filename}
        COMMENT "me FLEX Generating ${CMAKE_CURRENT_SOURCE_DIR}/${_basename}.c" )
ENDMACRO(LEX_FILE)

YACC_FILE(Parser.y)
LEX_FILE(Lexer.l)

#add_executable(parser
#    Expression.c
#    Parser.c
#    Lexer.c
#)
#INCLUDE_DIRECTORIES("${PROJECT_SOURCE_DIR}")

ADD_LIBRARY(parser Expression.c Parser.c Lexer.c)

