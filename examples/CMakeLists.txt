# Examples CMakeLists.txt

# Function to create an example executable
function(add_amp_example EXAMPLE_NAME SOURCE_FILE)
    add_executable(${EXAMPLE_NAME} ${SOURCE_FILE})
    
    target_link_libraries(${EXAMPLE_NAME}
        PRIVATE
            amp-runtime
    )
    
    target_compile_options(${EXAMPLE_NAME} PRIVATE
        -Wconversion
        -Wsign-conversion
    )
    
    # Platform-specific configuration
    if(AMP_PLATFORM STREQUAL "rp2350")
        # Add RP2350-specific linker scripts, startup code, etc.
        # target_link_options(${EXAMPLE_NAME} PRIVATE ...)
    endif()
    
    # Installation
    install(TARGETS ${EXAMPLE_NAME}
        RUNTIME DESTINATION bin/examples
    )
endfunction()

# Add examples
add_amp_example(hello-amp hello-amp/hello_amp.c)
add_amp_example(pingpong pingpong/pingpong.c)
add_amp_example(shared-counter shared-counter/shared_counter.c)
