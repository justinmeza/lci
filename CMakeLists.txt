cmake_minimum_required(VERSION 2.8)
project(lci)

ENABLE_TESTING()
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

SET(PERFORM_MEM_TESTS FALSE CACHE BOOL "Whether or not to enable memory testing")
MARK_AS_ADVANCED(PERFORM_MEM_TESTS)

IF(${PERFORM_MEM_TESTS})
  find_program(VALGRIND valgrind)
  IF(NOT VALGRIND)
    message(FATAL_ERROR "Error: You've enabled memory testing but we can't find valgrind")
    message(FATAL_ERROR "Try one of the following: ")
    message(FATAL_ERROR "1: Make sure valgrind is in your PATH")
    message(FATAL_ERROR "2: Install valgrind if you already haven't")
    message(FATAL_ERROR "3: Disable memory testing")
  ENDIF(NOT VALGRIND)
ENDIF(${PERFORM_MEM_TESTS})

SET(HDRS 
  interpreter.h
  lexer.h
  parser.h
  tokenizer.h
  unicode.h
)

SET(SRCS
  interpreter.c
  lexer.c
  main.c
  parser.c
  tokenizer.c
  unicode.c
)
  
add_executable(lci ${SRCS} ${HDRS})
target_link_libraries(lci m)
add_subdirectory(test)
