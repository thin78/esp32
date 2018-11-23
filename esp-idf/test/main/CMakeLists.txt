set(COMPONENT_SRCS "mg.c"
                   "ws.c"
                   "io.c"
                   "mg.c"
                   "http.c
                   "main.c")
set(COMPONENT_ADD_INCLUDEDIRS ".")

ADD_DEFINITIONS(-DESP_PLATFORM)

register_component()