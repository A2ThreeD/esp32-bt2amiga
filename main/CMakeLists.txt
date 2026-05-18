idf_component_register(SRCS "amiga-db9-mouse.cpp" "amiga-db9-joystick.cpp" "main.cpp" "${app_sources}"
                       REQUIRES bt esp_hid nvs_flash driver esp_driver_gpio bluepad32 btstack)

                       component_compile_options(-Wno-error=format= -Wno-format)
