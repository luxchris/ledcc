set(LEDCC_VERSION_MAJOR 0)
set(LEDCC_VERSION_MINOR 1)
#set(LEDCC_VERSION_MICRO 1)
#set(LEDCC_VERSION ${LEDCC_VERSION_MAJOR}.${LEDCC_VERSION_MINOR}.${LEDCC_VERSION_MICRO})
set(LEDCC_VERSION ${LEDCC_VERSION_MAJOR}.${LEDCC_VERSION_MINOR})

message(STATUS "Creating Global.hpp")

configure_file("${CoreDirectory}/Global.hpp.in" "${CoreDirectory}/Global.hpp" @ONLY)
