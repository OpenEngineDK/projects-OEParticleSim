# Project name. Should be the same as the project directory name and
# will become the name of the binary target.
SET( PROJECT_NAME "OEParticleSim")

# Project source code list
SET( PROJECT_SOURCES
  # Add all the cpp source files here
    main.cpp
    GameFactory.cpp
)

# Project executable
ADD_EXECUTABLE(${PROJECT_NAME}
  ${PROJECT_SOURCES}
)

# Include needed to use SDL under Mac OS X
IF(APPLE)
  SET(PROJECT_SOURCES ${PROJECT_SOURCES} ../../conf/mac/SDLMain.m)
ENDIF(APPLE)

# Project dependencies
TARGET_LINK_LIBRARIES(OEParticleSim
  OpenEngine_Core
  OpenEngine_Logging
  OpenEngine_Display
  OpenEngine_Devices
  OpenEngine_Renderers_OpenGL
  OpenEngine_Scene
  # Extensions
  Extensions_ParticleSystem
  Extensions_GenericHandlers
)
