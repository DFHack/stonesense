# main project file. use it from a build sub-folder
PROJECT (stonesense) 
cmake_minimum_required(VERSION 2.6)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

# disable warning, autosearch
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

if("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")
   message(SEND_ERROR "In-source builds are not allowed.")
endif("${PROJECT_SOURCE_DIR}" STREQUAL "${PROJECT_BINARY_DIR}")

IF(NOT DEFINED CMAKE_BUILD_TYPE)
  SET(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel.")
ENDIF(NOT DEFINED CMAKE_BUILD_TYPE)

SET( EXECUTABLE_OUTPUT_PATH ${stonesense_SOURCE_DIR} CACHE PATH "Output directory for stonesense, default is root" )

include_directories (
    ${CMAKE_SOURCE_DIR}/dfhack/include/
    ${CMAKE_SOURCE_DIR}/dfhack/depends/md5/
    ${CMAKE_SOURCE_DIR}/dfhack/depends/tinyxml/
    ${CMAKE_SOURCE_DIR}/allegro/lib/
    ${CMAKE_SOURCE_DIR}
)

SET(PROJECT_SRCS
    dfhack/depends/tinyxml/tinystr.cpp
    dfhack/depends/tinyxml/tinyxml.cpp
    dfhack/depends/tinyxml/tinyxmlerror.cpp
    dfhack/depends/tinyxml/tinyxmlparser.cpp
    
    Block.cpp
    BlockCondition.cpp
    BlockFactory.cpp
    BuildingConfiguration.cpp
    ColorConfiguration.cpp
    ConditionalSprite.cpp
    Config.cpp
    Constructions.cpp
    ContentBuildingReader
    ContentLoader.cpp
    CreatureConfiguration.cpp
    Creatures.cpp
    GUI.cpp
    GameBuildings.cpp
    GroundMaterialConfiguration.cpp
    MapLoading.cpp
	SpriteColors.cpp
    SpriteMaps.cpp
    UserInput.cpp
    WorldSegment.cpp
    VegetationConfiguration.cpp
    main.cpp

)

# Under Windows, we also include a resource file to the build
if(WIN32)
    # Make sure that the resource file is seen as an RC file to be compiled with a resource compiler, not a C++ compiler
    set_source_files_properties(${CMAKE_SOURCE_DIR}/res.rc LANGUAGE RC)
    # Add the resource file to the list of sources
    list(APPEND PROJECT_SRCS ${CMAKE_SOURCE_DIR}/res.rc)
    # For MinGW, we have to change the compile flags
    if(MINGW)
        # resource compilation for mingw
        set(CMAKE_RC_COMPILER_INIT windres)
        ENABLE_LANGUAGE(RC)
        SET(CMAKE_RC_COMPILE_OBJECT
        "<CMAKE_RC_COMPILER> <FLAGS> <DEFINES> -o <OBJECT> <SOURCE>")
        set(RC_CFLAGS "-DMINGW -Ocoff")
        # If any sort of debugging is being enabled, add a _DEBUG define to the flags for the resource compiler
        if(CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")
           set(RC_CFLAGS "${RC_CFLAGS} -D_DEBUG")
        endif(CMAKE_BUILD_TYPE STREQUAL "DEBUG" OR CMAKE_BUILD_TYPE STREQUAL "RELWITHDEBINFO")
        set_source_files_properties(${CMAKE_SOURCE_DIR}/res.rc COMPILE_FLAGS "${RC_CFLAGS}")
        # For anything else, assumingly Visual Studio at this point, use a different set of compile flags
    endif(MINGW)
endif(WIN32)

#linux
IF(UNIX)
    
    add_definitions(-DLINUX_BUILD)
    SET (PLATFORM_SRC
    )
    
#    FIND_PACKAGE(Allegro5 REQUIRED)
    LINK_DIRECTORIES(${LINK_DIRECTORIES})
    
#    INCLUDE_DIRECTORIES(${ALLEGRO_INCLUDE_DIR})
 
#    SET(PROJECT_LIBS ${ALLEGRO5_LIBRARIES} ${PROJECT_LIBS})
    SET(PROJECT_LIBS allegro allegro_primitives allegro_color allegro_dialog allegro_font allegro_image allegro_main allegro_ttf dfhack-debug ${PROJECT_LIBS})
    
    ADD_EXECUTABLE(stonesense ${PROJECT_SRCS} ${PLATFORM_SRC})
    TARGET_LINK_LIBRARIES(stonesense ${PROJECT_LIBS})
    
# windows
ELSE(UNIX)
    add_definitions(-DBUILD_DFHACK_LIB)
    SET (PLATFORM_SRC
	)
    # use local allegro
    # MinGW is broken. doesn't link properly for some reason.
    
    IF(MINGW)
        add_definitions()
        SET(PROJECT_LIBS liballegro.a liballegro_primitives.a liballegro_image.a liballegro_font.a liballegro_dialog.a liballegro_ttf.dll.a libpng.dll.a libglu32.a libglaux.a dfhack.a psapi mingw32)
        include_directories (
		    ${include_directories}
		    ${CMAKE_SOURCE_DIR}/allegro
	    )
	    LINK_DIRECTORIES(
		    ${LINK_DIRECTORIES}
		    ${CMAKE_SOURCE_DIR}/allegro/lib
	    )
	     
    # MSVC
    ELSE(MINGW)
        add_definitions()
        SET(PROJECT_LIBS kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib allegro.lib allegro_primitives.lib allegro_font.lib allegro_ttf.lib allegro_image.lib allegro_dialog.lib libpng.lib zdll.lib winmm.lib psapi.lib opengl32.lib glu32.lib dfhack/build/dfhack.lib $(NOINHERIT))
        include_directories (
		    ${include_directories}
		    ${CMAKE_SOURCE_DIR}/allegro
	    )
	    LINK_DIRECTORIES(
		    ${LINK_DIRECTORIES}
		    ${CMAKE_SOURCE_DIR}/allegro/lib
	    )
    ENDIF(MINGW)
    ADD_EXECUTABLE(stonesense WIN32 ${PROJECT_SRCS} ${PLATFORM_SRC})
    TARGET_LINK_LIBRARIES(stonesense ${PROJECT_LIBS})
    
ENDIF(UNIX)