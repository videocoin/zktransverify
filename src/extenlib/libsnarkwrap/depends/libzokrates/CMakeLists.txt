include_directories(.)

add_library(
  zokrates
  STATIC
  
  pghr13.cpp
  gm17.cpp
  util.cpp
)

target_include_directories(
  zokrates

  PUBLIC
  ${DEPENDS_DIR}/libsnark
  ${DEPENDS_DIR}/libsnark/depends/libfqfft
  ${DEPENDS_DIR}/libsnark/depends/libff
)
