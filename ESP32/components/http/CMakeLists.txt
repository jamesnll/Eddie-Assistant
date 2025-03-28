idf_component_register(SRCS "http.c"
                       INCLUDE_DIRS "." "../queue_wrapper"
                       PRIV_REQUIRES esp_http_client esp-tls)
