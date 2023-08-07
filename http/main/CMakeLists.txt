set(component_srcs "main.c" "http_client.c" "http_server.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS ".")
