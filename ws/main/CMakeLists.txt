set(component_srcs "main.c" "ws_client.c" "ws_server.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS ".")
