PROJECT (stonesense)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

# WTF
if(WIN32)
include_directories (
    ${CMAKE_SOURCE_DIR}/dfhack/include/
    ${CMAKE_SOURCE_DIR}/dfhack/depends/md5/
    ${CMAKE_SOURCE_DIR}/dfhack/depends/tinyxml/
    ${CMAKE_SOURCE_DIR}/dfhack/
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
# windows
ELSE(UNIX)
    SET(PROJECT_LIBS kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib allegro-5.0.3-monolith-md.lib winmm.lib psapi.lib opengl32.lib glu32.lib $(NOINHERIT))
    include_directories (
        ${include_directories}
        ${stonesense_SOURCE_DIR}/allegro5/include
    )
    LINK_DIRECTORIES(
        ${LINK_DIRECTORIES}
        ${stonesense_SOURCE_DIR}/allegro5/lib
    )
ENDIF(UNIX)
DFHACK_PLUGIN(stonesense ${PROJECT_SRCS} LINK_LIBRARIES ${PROJECT_LIBS})