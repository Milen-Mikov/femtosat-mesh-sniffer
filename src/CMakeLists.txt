add_executable(krups-mesh-sniffer main.cpp )

# Add the standard include files to the build
target_include_directories(krups-mesh-sniffer PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}
)

# Add any user requested libraries
target_link_libraries(krups-mesh-sniffer
    pico_stdlib
    hardware_spi
    hardware_dma
    hardware_pio
    hardware_interp
    hardware_timer
    hardware_watchdog
    hardware_clocks
    hardware_pwm
    pico_multicore
    
    RadioLib
    )

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(krups-mesh-sniffer
        pico_cyw43_arch_none 
    )
endif()

pico_add_extra_outputs(krups-mesh-sniffer)

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(krups-mesh-sniffer 0)
pico_enable_stdio_usb(krups-mesh-sniffer 1)