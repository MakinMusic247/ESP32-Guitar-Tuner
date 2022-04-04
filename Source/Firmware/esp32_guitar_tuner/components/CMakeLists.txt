set(COMPONENT_SRCDIRS ". DFT") # space separated list of folders
set(COMPONENT_ADD_INCLUDEDIRS ".")
set(COMPONENT_REQUIRES nvs_flash) # Add external esp-idf components required by custom components in child directories esp_vfs_fat
register_component()
