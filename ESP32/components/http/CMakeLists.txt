idf_component_register(SRCS "http.c"
                       INCLUDE_DIRS "." "../stream_buf"
                       PRIV_REQUIRES esp_http_client esp-tls)
