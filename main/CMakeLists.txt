idf_component_register(
    SRCS
        "app_main.c"
        "src/led_button.c"
        "src/led.c"
        "src/obstacle_sensor.c"
        "src/ultrasonic_sensor.c"
        "src/PWM.c"
        "src/analog_read.c"
        "src/dht11.c"
        "src/dc_motor.c"
        "src/servo_drive.c"
        "src/ULN2003.c"

    INCLUDE_DIRS
        "."
        "inc"
)