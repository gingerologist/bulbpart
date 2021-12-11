# Edit following two lines to set component requirements (see docs)
set(COMPONENT_REQUIRES )
set(COMPONENT_PRIV_REQUIRES )

set(COMPONENT_SRCS "main.c" EMBED_FILES "partition_table.bin")
set(COMPONENT_ADD_INCLUDEDIRS "")

register_component()
