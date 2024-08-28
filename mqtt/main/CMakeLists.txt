set(srcs "main.c")

if(CONFIG_SENDER)
	list(APPEND srcs "mqtt_sub.c")
elseif(CONFIG_RECEIVER)
	list(APPEND srcs "mqtt_pub.c")
endif()

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS ".")
