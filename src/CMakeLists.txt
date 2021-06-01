SET(SOURCES
    Buffer.cc
    Exception.cc
    Pulses.cc
    Wav.cc
    main.cc
)

ADD_EXECUTABLE(ti99tape ${SOURCES})
INSTALL(TARGETS ti99tape RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
