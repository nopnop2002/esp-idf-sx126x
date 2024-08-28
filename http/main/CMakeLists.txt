set(srcs "main.c")

if(CONFIG_SENDER)
	list(APPEND srcs "http_server.c")
elseif(CONFIG_RECEIVER)
	list(APPEND srcs "http_client.c")
endif()

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS ".")
