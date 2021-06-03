SET(SOURCES
    BitVector.cc
    Buffer.cc
    Exception.cc
    OutputFile.cc
    Pulses.cc
    TI99TapeEncoder.cc
    TZX.cc
    Wav.cc
    main.cc
    utility.cc
)

ADD_EXECUTABLE(ti99tape ${SOURCES})
INSTALL(TARGETS ti99tape RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
