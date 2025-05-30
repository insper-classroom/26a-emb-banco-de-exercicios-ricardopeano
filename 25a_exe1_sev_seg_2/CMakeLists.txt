add_executable(exe1-2
        main.c
)

target_link_libraries(exe1-2 pico_stdlib hardware_adc freertos)
pico_add_extra_outputs(exe1-2)
