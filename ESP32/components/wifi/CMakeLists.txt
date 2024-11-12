idf_component_register(SRCS "wifi.c"
                    INCLUDE_DIRS "." "../secrets"
                    PRIV_REQUIRES esp_wifi nvs_flash)
