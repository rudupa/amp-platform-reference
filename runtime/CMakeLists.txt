# Runtime library CMakeLists.txt

# Collect source files
set(RUNTIME_SOURCES
    src/amp_boot.c
    src/amp_config.c
    src/amp_mailbox.c
    src/amp_ringbuf.c
    src/amp_semaphore.c
    src/amp_shmem.c
)

# Create runtime library
add_library(amp-runtime STATIC ${RUNTIME_SOURCES})

# Include directories
target_include_directories(amp-runtime
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

# Compiler options
target_compile_options(amp-runtime PRIVATE
    -Wconversion
    -Wsign-conversion
)

# Platform-specific sources
if(AMP_PLATFORM STREQUAL "rp2350")
    # Add RP2350-specific implementation files here when available
    # target_sources(amp-runtime PRIVATE src/platform/rp2350/...)
endif()

# Installation
install(TARGETS amp-runtime
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)
