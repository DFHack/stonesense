PROJECT (stonesense)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

include_directories (${CMAKE_SOURCE_DIR})


SET(PROJECT_SRCS
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
	ItemConfiguration.cpp
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

#linux
IF(UNIX)
    OPTION(STONESENSE_INTERNAL_SO "Link with prebuilt internal allegro libs and headers." ON)
    IF(STONESENSE_INTERNAL_SO)
        FILE(DOWNLOAD http://cloud.github.com/downloads/peterix/stonesense/allegro.tar.bz2
                      ${stonesense_SOURCE_DIR}/allegro.tar.bz2
                      EXPECTED_MD5 11ce22e71db4a0886eac3af220eef032)
        execute_process(COMMAND tar xf ${stonesense_SOURCE_DIR}/allegro.tar.bz2
                        WORKING_DIRECTORY ${stonesense_SOURCE_DIR})
        SET(PROJECT_LIBS
            ${stonesense_SOURCE_DIR}/allegro/lib/liballegro.so.5.0
            ${stonesense_SOURCE_DIR}/allegro/lib/liballegro_primitives.so.5.0
            ${stonesense_SOURCE_DIR}/allegro/lib/liballegro_font.so.5.0
            ${stonesense_SOURCE_DIR}/allegro/lib/liballegro_image.so.5.0
            ${stonesense_SOURCE_DIR}/allegro/lib/liballegro_ttf.so.5.0
            ${PROJECT_LIBS}
        )
        include_directories (
            ${include_directories}
            ${stonesense_SOURCE_DIR}/allegro/include
        )
        install(DIRECTORY allegro/lib/
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
# windows
ELSE(UNIX)
    FILE(DOWNLOAD http://cloud.github.com/downloads/peterix/stonesense/allegrow.tar.gz
                  ${stonesense_SOURCE_DIR}/allegrow.tar.gz
                  EXPECTED_MD5 5bf41454531ee9d847b6ddac122bbc51)
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${stonesense_SOURCE_DIR}/allegrow.tar.gz
                    WORKING_DIRECTORY ${stonesense_SOURCE_DIR})
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
    include_directories (
        ${include_directories}
        ${stonesense_SOURCE_DIR}/allegrow/include
    )
    LINK_DIRECTORIES(
        ${LINK_DIRECTORIES}
        ${stonesense_SOURCE_DIR}/allegrow/lib
    )
ENDIF(UNIX)
DFHACK_PLUGIN(stonesense ${PROJECT_SRCS} LINK_LIBRARIES dfhack-tinyxml ${PROJECT_LIBS})
#install resources into DF/stonesense
install(DIRECTORY resources/
        DESTINATION "stonesense"
        PATTERN "CVS" EXCLUDE
        PATTERN ".svn" EXCLUDE)
# install readme
install(FILES README.txt DESTINATION stonesense)
