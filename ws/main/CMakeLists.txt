set(srcs "main.c")

if(CONFIG_SENDER)
	list(APPEND srcs "ws_server.c")
elseif(CONFIG_RECEIVER)
	list(APPEND srcs "ws_client.c")
endif()

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS ".")
