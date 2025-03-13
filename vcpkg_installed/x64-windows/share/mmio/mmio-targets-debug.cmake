#----------------------------------------------------------------
# Generated CMake target import file for configuration "Debug".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "mmio::mmio" for configuration "Debug"
set_property(TARGET mmio::mmio APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)
set_target_properties(mmio::mmio PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_DEBUG "CXX"
  IMPORTED_LOCATION_DEBUG "${_IMPORT_PREFIX}/debug/lib/mmiod.lib"
  )

list(APPEND _cmake_import_check_targets mmio::mmio )
list(APPEND _cmake_import_check_files_for_mmio::mmio "${_IMPORT_PREFIX}/debug/lib/mmiod.lib" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
