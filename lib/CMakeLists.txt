
include_directories("../include")

set(LIB_SRCS ../include/ColumnRead.c ../include/Windows.c)
set(HDRS ../include/ColumnRead.h ../include/Windows.h)

add_library(nid STATIC ${LIB_SRCS} ${HDRS})
target_link_libraries(nid m fftw3)

install(TARGETS nid DESTINATION lib)
install(FILES ${HDRS} DESTINATION include)
