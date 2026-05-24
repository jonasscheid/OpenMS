# Tiny portable assertion helper for CTest tests that need to count regex
# matches in a generated file (where a byte-DIFF is unsuitable, e.g., output
# containing non-deterministic identifiers).
#
# Usage:
#   add_test("<name>" ${CMAKE_COMMAND}
#       -DFILE=<path>
#       -DPATTERN=<regex>
#       -DEXPECTED=<int>
#       -P <OpenMS_root>/cmake/CountLinesMatching.cmake)
#
# Fails with a FATAL_ERROR (-> non-zero exit -> CTest fail) if the count of
# lines matching PATTERN in FILE does not equal EXPECTED.

if(NOT DEFINED FILE OR NOT DEFINED PATTERN OR NOT DEFINED EXPECTED)
  message(FATAL_ERROR "CountLinesMatching: FILE, PATTERN and EXPECTED must be set")
endif()

if(NOT EXISTS "${FILE}")
  message(FATAL_ERROR "CountLinesMatching: file not found: ${FILE}")
endif()

file(STRINGS "${FILE}" matches REGEX "${PATTERN}")
list(LENGTH matches actual)

if(NOT actual EQUAL ${EXPECTED})
  message(FATAL_ERROR "CountLinesMatching: expected ${EXPECTED} lines matching '${PATTERN}' in ${FILE}, got ${actual}")
endif()
