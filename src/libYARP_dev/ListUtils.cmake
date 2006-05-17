### STRING UTILITIES, from CMAKE public wiki

# lisp CAR
MACRO(CAR var)
  SET(${var} ${ARGV1})
ENDMACRO(CAR)

# lisp CDR
MACRO(CDR var junk)
  SET(${var} ${ARGN})
ENDMACRO(CDR)

# get the length of a list
MACRO(LIST_LENGTH var)
  SET(entries)
  FOREACH(e ${ARGN})
    SET(entries "${entries}.")
  ENDFOREACH(e)
  STRING(LENGTH ${entries} ${var})
ENDMACRO(LIST_LENGTH)

# get the n-th element of a list
MACRO(LIST_INDEX var index)
  SET(list . ${ARGN})
  FOREACH(i RANGE 1 ${index})
    CDR(list ${list})
  ENDFOREACH(i)
  CAR(${var} ${list})
ENDMACRO(LIST_INDEX)

# check if something is in a list
MACRO(LIST_CONTAINS var value)
  SET(${var})
  FOREACH (value2 ${ARGN})
    IF (${value} STREQUAL ${value2})
      SET(${var} TRUE)
    ENDIF (${value} STREQUAL ${value2})
  ENDFOREACH (value2)
ENDMACRO(LIST_CONTAINS)

