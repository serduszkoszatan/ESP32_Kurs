idf_component_register(
    SRCS
        "main.c"
        "src/template_file.c"
        "src/comunication_protocols/UART.c"

    INCLUDE_DIRS
        "."
        "inc"
        "inc/comunication_protocols"
)