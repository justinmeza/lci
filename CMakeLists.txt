cmake_minimum_required(VERSION 2.8)
project(lci)

ENABLE_TESTING()
SET(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake )

SET(PERFORM_MEM_TESTS FALSE CACHE BOOL "Whether or not to enable memory testing")
MARK_AS_ADVANCED(PERFORM_MEM_TESTS)

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
add_subdirectory(test)
