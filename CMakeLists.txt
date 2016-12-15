CMAKE_MINIMUM_REQUIRED(VERSION 3.5)

# Add executable targets
########################
ADD_EXECUTABLE(weatherStation dumpReading.c)

# Add library targets
#####################
FIND_LIBRARY(LIB_AWA libawa.so ${STAGING_DIR}/usr/lib)
FIND_PACKAGE(LetMeCreate CONFIG REQUIRED)
TARGET_LINK_LIBRARIES(weatherStation letmecreate_core letmecreate_click ${LIB_AWA})

# Add install targets
######################
INSTALL(TARGETS weatherStation RUNTIME DESTINATION bin)
