idf_component_register(SRCS "amiga-db9-mouse.cpp" "main.cpp" "bt_keyboard.cpp" "esp32-ps2dev.cpp" "${app_sources}"
                       REQUIRES bt esp_hid nvs_flash driver esp_driver_gpio)

                       component_compile_options(-Wno-error=format= -Wno-format)
