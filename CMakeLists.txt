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
    ${include_directories}
    ${CMAKE_SOURCE_DIR}/dfhack/library/
    ${CMAKE_SOURCE_DIR}/dfhack/library/md5/
    ${CMAKE_SOURCE_DIR}/dfhack/library/tinyxml
    ${CMAKE_SOURCE_DIR}/loadpng
    ${CMAKE_SOURCE_DIR}
)

SET(PROJECT_SRCS
    dfhack/library/tinyxml/tinystr.cpp
    dfhack/library/tinyxml/tinyxml.cpp
    dfhack/library/tinyxml/tinyxmlerror.cpp
    dfhack/library/tinyxml/tinyxmlparser.cpp

    dfhack/library/DFDataModel.cpp
    dfhack/library/DFHackAPI.cpp
    dfhack/library/DFMemInfo.cpp
    dfhack/library/DFMemInfoManager.cpp
    dfhack/library/DFTileTypes.cpp
    dfhack/library/md5/md5.cpp
    dfhack/library/md5/md5wrapper.cpp
    loadpng/loadpng.c
    loadpng/regpng.c
    loadpng/savepng.c

    Block.cpp
    BlockCondition.cpp
	BlockFactory.cpp
    BuildingConfiguration.cpp
    ConditionalSprite.cpp
    Config.cpp
    Constructions.cpp
    Contributions.txt
    CreatureConfiguration.cpp
    Creatures.cpp
    GUI.cpp
    GameBuildings.cpp
    GroundMaterialConfiguration.cpp
    MapLoading.cpp
    SpriteMaps.cpp
    UserInput.cpp
    WorldSegment.cpp
    main.cpp
    xmlBuildingReader.cpp
)
#linux
IF(UNIX)
    
    add_definitions(-DLINUX_BUILD)
    add_definitions(-DBUILD_DFHACK_LIB)
    SET (PLATFORM_SRC
    dfhack/library/DFProcess-linux.cpp
    dfhack/library/DFProcessEnumerator-linux.cpp
    )
    
    FIND_PACKAGE(PNG REQUIRED)
    FIND_PACKAGE(Allegro REQUIRED)
    LINK_DIRECTORIES(${LINK_DIRECTORIES})
    
    INCLUDE_DIRECTORIES(${PNG_INCLUDE_DIR} ${ALLEGRO_INCLUDE_DIR})
 
    SET(PROJECT_LIBS ${ALLEGRO_LIBRARIES} ${PNG_LIBRARIES} ${PROJECT_LIBS})
    ADD_EXECUTABLE(stonesense ${PROJECT_SRCS} ${PLATFORM_SRC})
    TARGET_LINK_LIBRARIES(stonesense ${PROJECT_LIBS})
# windows
ELSE(UNIX)
    add_definitions(-DBUILD_DFHACK_LIB)
    SET (PLATFORM_SRC
	dfhack/library/DFProcess-windows.cpp
	dfhack/library/DFProcessEnumerator-windows.cpp
	)
    # use local allegro
    # MinGW is broken. doesn't link properly for some reason.
    IF(MINGW)
        add_definitions(-DALLEGRO_MINGW32)
        SET(PROJECT_LIBS alleg42 psapi mingw32 libpng)
        include_directories (
		    ${include_directories}
		    ${CMAKE_SOURCE_DIR}/allegro-mingw-4.2.2/include
	    )
	    LINK_DIRECTORIES(
		    ${LINK_DIRECTORIES}
		    ${CMAKE_SOURCE_DIR}/allegro-mingw-4.2.2/lib
		    ${CMAKE_SOURCE_DIR}/allegro-mingw-4.2.2/bin
		    ${CMAKE_SOURCE_DIR}/loadpng/libpng/lib/
	    )
    # MSVC
    ELSE(MINGW)
        add_definitions(-DALLEGRO_MSVC)
        SET(PROJECT_LIBS alleg psapi libpng)
        include_directories (
		    ${include_directories}
		    ${CMAKE_SOURCE_DIR}/allegro
	    )
	    LINK_DIRECTORIES(
		    ${LINK_DIRECTORIES}
		    ${CMAKE_SOURCE_DIR}/allegro/lib_msvs8
		    ${CMAKE_SOURCE_DIR}/loadpng/libpng/lib/
	    )
    ENDIF(MINGW)
    ADD_EXECUTABLE(stonesense WIN32 ${PROJECT_SRCS} ${PLATFORM_SRC})
    TARGET_LINK_LIBRARIES(stonesense ${PROJECT_LIBS})
    
ENDIF(UNIX)