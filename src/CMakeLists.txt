SET(SOURCES
    BitVector.cc
    Buffer.cc
    Exception.cc
    FileFormat.cc
    GetOpt.cc
    OutputFile.cc
    Pulses.cc
    TI99TapeDecoder.cc
    TI99TapeEncoder.cc
    TZX.cc
    Wav.cc
    System.cc
    main.cc
    utility.cc
)

ADD_EXECUTABLE(ti99tape ${SOURCES})
INSTALL(TARGETS ti99tape RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
