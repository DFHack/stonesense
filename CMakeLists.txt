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
    Occlusion_Test.cpp
    SpriteColors.cpp
    SpriteMaps.cpp
    SpriteObjects.cpp
    UserInput.cpp
    WorldSegment.cpp
    VegetationConfiguration.cpp
    main.cpp
)

OPTION(STONESENSE_STATIC "Static link allegro to Stonesense." OFF)
#linux
IF(UNIX)
    IF(STONESENSE_STATIC)
        #this is terrible :(
        #lions live here
        #ADD_DEFINITIONS("-DALLEGRO_STATICLINK")
        #SET(PROJECT_LIBS
        #    allegro-static
        #    allegro_primitives-static
        #    allegro_font-static
        #    allegro_image-static
        #    allegro_ttf-static
        #    m
        #    pthread
        #    SM
        #    ICE
        #    X11
        #    Xext
        #    Xcursor
        #    Xxf86vm
        #    Xinerama
        #    Xrandr
        #    Xpm
        #    GL
        #    GLU
        #    jpeg
        #    #png and freetype are already loaded by DF
        #    ${PROJECT_LIBS}
        #   )
    ELSE()
        OPTION(STONESENSE_INTERNAL_SO "Link with prebuilt internal allegro libs and headers." ON)
        IF(STONESENSE_INTERNAL_SO)
            SET(PROJECT_LIBS
                ${stonesense_SOURCE_DIR}/alleg-ssense/bin-linux/liballegro.so.5.0
                ${stonesense_SOURCE_DIR}/alleg-ssense/bin-linux/liballegro_primitives.so.5.0
                ${stonesense_SOURCE_DIR}/alleg-ssense/bin-linux/liballegro_font.so.5.0
                ${stonesense_SOURCE_DIR}/alleg-ssense/bin-linux/liballegro_image.so.5.0
                ${stonesense_SOURCE_DIR}/alleg-ssense/bin-linux/liballegro_ttf.so.5.0
                ${PROJECT_LIBS}
            )
            include_directories (
                ${include_directories}
                ${stonesense_SOURCE_DIR}/alleg-ssense/include-linux
            )
            install(DIRECTORY alleg-ssense/bin-linux/
                    DESTINATION "stonesense/deplibs"
                    PATTERN "CVS" EXCLUDE
                    PATTERN ".svn" EXCLUDE)
        ELSE()
            SET(PROJECT_LIBS
                allegro
                allegro_primitives
                allegro_font
                allegro_image
                allegro_ttf
                ${PROJECT_LIBS}
            )
        ENDIF()
    ENDIF()
# windows
ELSE(UNIX)
    IF(STONESENSE_STATIC)
        ADD_DEFINITIONS("-DALLEGRO_STATICLINK")
        SET(PROJECT_LIBS
            kernel32.lib
            user32.lib
            gdi32.lib
            winspool.lib
            shell32.lib
            ole32.lib
            oleaut32.lib
            uuid.lib
            comdlg32.lib
            advapi32.lib
            allegro-static.lib
            allegro_primitives-static.lib
            allegro_font-static.lib
            allegro_image-static.lib
            allegro_ttf-static.lib
            freetype-static.lib
            winmm.lib
            psapi.lib
            opengl32.lib
            glu32.lib
            gdiplus.lib
            shlwapi.lib
            $(NOINHERIT)
           )
    ELSE()
        SET(PROJECT_LIBS
            kernel32.lib
            user32.lib
            gdi32.lib
            winspool.lib
            shell32.lib
            ole32.lib
            oleaut32.lib
            uuid.lib
            comdlg32.lib
            advapi32.lib
            allegro.lib
            allegro_primitives.lib
            allegro_font.lib
            allegro_image.lib
            allegro_ttf.lib
            winmm.lib
            psapi.lib
            opengl32.lib
            glu32.lib
            $(NOINHERIT)
           )
    ENDIF()
    include_directories (
        ${include_directories}
        ${stonesense_SOURCE_DIR}/alleg-ssense/include
    )
    LINK_DIRECTORIES(
        ${LINK_DIRECTORIES}
        ${stonesense_SOURCE_DIR}/alleg-ssense/lib
    )
ENDIF(UNIX)
DFHACK_PLUGIN(stonesense ${PROJECT_SRCS} LINK_LIBRARIES ${PROJECT_LIBS})
#install resources into DF/stonesense
install(DIRECTORY resources/
        DESTINATION "stonesense"
        PATTERN "CVS" EXCLUDE
        PATTERN ".svn" EXCLUDE)
