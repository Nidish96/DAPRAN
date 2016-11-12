
include_directories("../include")
link_directories("../lib")

add_executable(convolve convolve.c)
add_executable(nidfft nidfft.c)
add_executable(nidstft nidstft.c)
add_executable(wavegen wavegen.c)
add_executable(nidpoincare nidpoincare.c)

target_link_libraries(convolve m fftw3 nid)
target_link_libraries(nidfft m fftw3 nid)
target_link_libraries(nidstft m fftw3 nid)
target_link_libraries(wavegen m fftw3 nid)
target_link_libraries(nidpoincare m fftw3 nid)

install(TARGETS convolve nidfft nidstft wavegen nidpoincare DESTINATION bin)


