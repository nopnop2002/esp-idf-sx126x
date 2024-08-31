set(srcs "main.c")

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "." REQUIRED_IDF_TARGETS esp32s2 esp32s3)
