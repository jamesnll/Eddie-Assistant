# main/CMakeLists.txt
idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "../components/secrets" "../components/wifi" "../components/http" "../components/queue_wrapper")