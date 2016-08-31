# Add executable targets
########################
ADD_EXECUTABLE(weatherStation dumpReading.c)

# Add library targets
#####################
FIND_LIBRARY(LIB_AWA libawa.so ${STAGING_DIR}/usr/lib)
FIND_LIBRARY(LIB_LMC_CORE libletmecreate_core.so ${STAGING_DIR}/usr/lib)
FIND_LIBRARY(LIB_LMC_CLICK libletmecreate_click.so ${STAGING_DIR}/usr/lib)
TARGET_LINK_LIBRARIES(weatherStation ${LIB_LMC_CORE} ${LIB_LMC_CLICK} ${LIB_AWA})

# Add install targets
######################
INSTALL(TARGETS weatherStation RUNTIME DESTINATION bin)
