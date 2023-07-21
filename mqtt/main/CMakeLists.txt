set(component_srcs "main.c" "mqtt_pub.c" "mqtt_sub.c")

idf_component_register(SRCS "${component_srcs}"
                       INCLUDE_DIRS ".")
