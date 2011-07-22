PROJECT (stonesense)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

if(WIN32)
include_directories (
    ${CMAKE_SOURCE_DIR}/dfhack/include/
    ${CMAKE_SOURCE_DIR}/dfhack/depends/md5/
    ${CMAKE_SOURCE_DIR}/dfhack/depends/tinyxml/
    ${CMAKE_SOURCE_DIR}/dfhack/
    ${CMAKE_SOURCE_DIR}/allegro/lib/
    ${CMAKE_SOURCE_DIR}
)
else(WIN32)
include_directories (
    ${CMAKE_SOURCE_DIR}/dfhack/depends/md5/
    ${CMAKE_SOURCE_DIR}/dfhack/depends/tinyxml/
    ${CMAKE_SOURCE_DIR}
)
endif(WIN32)

SET(PROJECT_SRCS
    dfhack/depends/tinyxml/tinystr.cpp
    dfhack/depends/tinyxml/tinyxml.cpp
    dfhack/depends/tinyxml/tinyxmlerror.cpp
    dfhack/depends/tinyxml/tinyxmlparser.cpp

    Block.cpp
    BlockCondition.cpp
    BlockFactory.cpp
    BlockTree.cpp
    BuildingConfiguration.cpp
    ColorConfiguration.cpp
    ConditionalSprite.cpp
    Config.cpp
    Constructions.cpp
    ContentBuildingReader.cpp
    ContentLoader.cpp
    CreatureConfiguration.cpp
    Creatures.cpp
    FluidConfiguration.cpp
    GUI.cpp
    GameBuildings.cpp
    GroundMaterialConfiguration.cpp
    MapLoading.cpp
    SpriteColors.cpp
    SpriteMaps.cpp
    SpriteObjects.cpp
    UserInput.cpp
    WorldSegment.cpp
    VegetationConfiguration.cpp
    main.cpp
)

#linux
IF(UNIX)
    SET(PROJECT_LIBS allegro allegro_primitives allegro_color allegro_dialog
                     allegro_font allegro_image allegro_main allegro_ttf
                     ${PROJECT_LIBS})
    DFHACK_PLUGIN(stonesense ${PROJECT_SRCS} LINK_LIBRARIES ${PROJECT_LIBS})
# windows
ELSE(UNIX)
    # THIS IS TOTAL BULL****
    add_definitions(-DBUILD_DFHACK_LIB)
    SET (PLATFORM_SRC
    )
    SET(PROJECT_LIBS kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib allegro.lib allegro_primitives.lib allegro_font.lib allegro_ttf.lib allegro_image.lib allegro_dialog.lib libpng.lib zdll.lib winmm.lib psapi.lib opengl32.lib glu32.lib dfhack/build/dfhack.lib $(NOINHERIT))
    include_directories (
        ${include_directories}
        ${CMAKE_SOURCE_DIR}/allegro
    )
    LINK_DIRECTORIES(
        ${LINK_DIRECTORIES}
        ${CMAKE_SOURCE_DIR}/allegro/lib
    )
    ADD_EXECUTABLE(stonesense WIN32 ${PROJECT_SRCS} ${PLATFORM_SRC})
    TARGET_LINK_LIBRARIES(stonesense ${PROJECT_LIBS})
ENDIF(UNIX)
