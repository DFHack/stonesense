PROJECT (stonesense)
# prevent CMake warnings about INTERFACE_LINK_LIBRARIES vs LINK_INTERFACE_LIBRARIES
IF(CMAKE_VERSION VERSION_GREATER "2.8.12")
    CMAKE_POLICY(SET CMP0022 OLD)
ENDIF()

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

include_directories (${CMAKE_SOURCE_DIR})


SET(PROJECT_SRCS
    TrackingModes.cpp
    Overlay.cpp
    Tile.cpp
    TileCondition.cpp
    TileTree.cpp
    BuildingConfiguration.cpp
    ColorConfiguration.cpp
    ConditionalSprite.cpp
    Config.cpp
    Constructions.cpp
    ContentBuildingReader.cpp
    ContentLoader.cpp
    CreatureConfiguration.cpp
    Creatures.cpp
    DumpInfo.cpp
    FluidConfiguration.cpp
    GUI.cpp
    GameBuildings.cpp
    GroundMaterialConfiguration.cpp
    ItemConfiguration.cpp
    MapLoading.cpp
    SegmentProcessing.cpp
    OcclusionTest.cpp
    SpriteColors.cpp
    SpriteMaps.cpp
    SpriteObjects.cpp
    TreeGrowthConfiguration.cpp
    UserInput.cpp
    WorldSegment.cpp
    VegetationConfiguration.cpp
    Keybinds.cpp
    EnumToString.cpp
    MaterialMatcher.cpp
    TiletypeMatcher.cpp
    ConnectionState.cpp
    main.cpp
)

#proto files to include.
SET(PROJECT_PROTO
	../../proto/RemoteFortressReader
)

IF(UNIX)
    IF(APPLE)
    # Mac OS X
        OPTION(STONESENSE_INTERNAL_DYLIB "Link with prebuilt internal allegro libs and headers." ON)
        IF(STONESENSE_INTERNAL_DYLIB)
            FILE(DOWNLOAD http://topazgryphon.org/~tcollett/df/allegro-5.0.11-osx.tar.bz2
                          ${stonesense_SOURCE_DIR}/allegro-5.0.11-osx.tar.bz2
                          EXPECTED_MD5 c531eb16f2530e1168116fcbce7644df)
            execute_process(COMMAND tar xjf ${stonesense_SOURCE_DIR}/allegro-5.0.11-osx.tar.bz2
                            WORKING_DIRECTORY ${stonesense_SOURCE_DIR})
            SET(PROJECT_LIBS
                ${stonesense_SOURCE_DIR}/allegro-osx/lib/liballegro.5.0.dylib
                ${stonesense_SOURCE_DIR}/allegro-osx/lib/liballegro_primitives.5.0.dylib
                ${stonesense_SOURCE_DIR}/allegro-osx/lib/liballegro_font.5.0.dylib
                ${stonesense_SOURCE_DIR}/allegro-osx/lib/liballegro_color.5.0.dylib
                ${stonesense_SOURCE_DIR}/allegro-osx/lib/liballegro_image.5.0.dylib
                ${stonesense_SOURCE_DIR}/allegro-osx/lib/liballegro_ttf.5.0.dylib
                ${PROJECT_LIBS}
            )
            include_directories (
                ${include_directories}
                ${stonesense_SOURCE_DIR}/allegro-osx/include
                ${stonesense_SOURCE_DIR}/allegro-osx/lib/Headers
            )
            install(DIRECTORY allegro-osx/lib/
                    DESTINATION "hack/libs"
                    PATTERN "CVS" EXCLUDE
                    PATTERN ".svn" EXCLUDE)
        ELSE()
            SET(PROJECT_LIBS
                allegro
                allegro_primitives
                allegro_font
                allegro_color
                allegro_image
                allegro_ttf
                ${PROJECT_LIBS}
            )
        ENDIF()
    ELSE(APPLE)
    #linux
        OPTION(STONESENSE_INTERNAL_SO "Link with prebuilt internal allegro libs and headers." ON)
        IF(STONESENSE_INTERNAL_SO)
            FILE(DOWNLOAD http://dethware.org/dfhack/download/allegro509b.tar.bz2
                          ${stonesense_SOURCE_DIR}/allegro509b.tar.bz2
                          EXPECTED_MD5 74ead5d95a83a03a301cdc76fa0a3df8)
            execute_process(COMMAND tar xf ${stonesense_SOURCE_DIR}/allegro509b.tar.bz2
                            WORKING_DIRECTORY ${stonesense_SOURCE_DIR})
            SET(PROJECT_LIBS
                ${stonesense_SOURCE_DIR}/allegro/lib/liballegro.so.5.0
                ${stonesense_SOURCE_DIR}/allegro/lib/liballegro_primitives.so.5.0
                ${stonesense_SOURCE_DIR}/allegro/lib/liballegro_font.so.5.0
                ${stonesense_SOURCE_DIR}/allegro/lib/liballegro_image.so.5.0
                ${stonesense_SOURCE_DIR}/allegro/lib/liballegro_ttf.so.5.0
                ${stonesense_SOURCE_DIR}/allegro/lib/liballegro_color.so.5.0
                ${PROJECT_LIBS}
            )
            include_directories (
                ${include_directories}
                ${stonesense_SOURCE_DIR}/allegro/include
            )
            install(DIRECTORY allegro/lib/
                    DESTINATION "hack/libs"
                    PATTERN "CVS" EXCLUDE
                    PATTERN ".svn" EXCLUDE)
        ELSE()
            SET(PROJECT_LIBS
                allegro
                allegro_primitives
                allegro_font
                allegro_image
                allegro_ttf
                allegro_color
                ${PROJECT_LIBS}
            )
        ENDIF()
    ENDIF()
# windows
ELSE(UNIX)
    SET(ALLEGRO_DOWNLOAD_DIR ${stonesense_SOURCE_DIR}/win${DFHACK_BUILD_ARCH})
    IF(DFHACK_BUILD_64)
        download_file("http://download.gna.org/allegro/allegro-unstable-bin/5.1.12/allegro-msvc2015-x64-5.1.12.zip"
                      ${ALLEGRO_DOWNLOAD_DIR}/allegro-msvc2015-x64-5.1.12.zip
                      "3abb0bd88251efde001a8b65fdf17683")
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${ALLEGRO_DOWNLOAD_DIR}/allegro-msvc2015-x64-5.1.12.zip
                        WORKING_DIRECTORY ${ALLEGRO_DOWNLOAD_DIR})
        download_file("http://download.gna.org/allegro/allegro-deps/1.2.0/allegro_deps-msvc2015-x64-1.2.0.zip"
                      ${ALLEGRO_DOWNLOAD_DIR}/allegro_deps-msvc2015-x64-1.2.0.zip
                      "eb8d0f4569b84c36bef3733b40b00c72")
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${ALLEGRO_DOWNLOAD_DIR}/allegro_deps-msvc2015-x64-1.2.0.zip
                        WORKING_DIRECTORY ${ALLEGRO_DOWNLOAD_DIR})
    ELSE()
        download_file("http://download.gna.org/allegro/allegro-unstable-bin/5.1.12/allegro-msvc2015-x86-5.1.12.zip"
                      ${ALLEGRO_DOWNLOAD_DIR}/allegro-msvc2015-x86-5.1.12.zip
                      "86e6aeea828743107dc1f3a3d562e53d")
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${ALLEGRO_DOWNLOAD_DIR}/allegro-msvc2015-x86-5.1.12.zip
                        WORKING_DIRECTORY ${ALLEGRO_DOWNLOAD_DIR})
        download_file("http://download.gna.org/allegro/allegro-deps/1.2.0/allegro_deps-msvc2015-x86-1.2.0.zip"
                      ${ALLEGRO_DOWNLOAD_DIR}/allegro_deps-msvc2015-x86-1.2.0.zip
                      "be096bfef256cb113e25dbb2eb7fd410")
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${ALLEGRO_DOWNLOAD_DIR}/allegro_deps-msvc2015-x86-1.2.0.zip
                        WORKING_DIRECTORY ${ALLEGRO_DOWNLOAD_DIR})
    ENDIF()
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
        allegro_color-static.lib
        freetype.lib
        libpng16.lib
        jpeg.lib
        zlib.lib
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
        ${stonesense_SOURCE_DIR}/win${DFHACK_BUILD_ARCH}/allegro/include
        ${stonesense_SOURCE_DIR}/win${DFHACK_BUILD_ARCH}/allegro_deps/include
    )
    LINK_DIRECTORIES(
        ${LINK_DIRECTORIES}
        ${stonesense_SOURCE_DIR}/win${DFHACK_BUILD_ARCH}/allegro/lib
        ${stonesense_SOURCE_DIR}/win${DFHACK_BUILD_ARCH}/allegro_deps/lib
    )
ENDIF(UNIX)

DFHACK_PLUGIN(stonesense ${PROJECT_SRCS} LINK_LIBRARIES ${DFHACK_TINYXML} ${PROJECT_LIBS} PROTOBUFS ${PROJECT_PROTO})

# Make sure the source is generated before the executable builds.
ADD_DEPENDENCIES(stonesense generate_proto)

TARGET_LINK_LIBRARIES(stonesense protobuf-lite ${PROJECT_LIBS})


IF(APPLE)
    add_custom_command(TARGET stonesense POST_BUILD COMMAND ${stonesense_SOURCE_DIR}/fix-libs-stonesense.sh WORKING_DIRECTORY . COMMENT "Fixing Stonesense library dependencies...")
ENDIF(APPLE)
#install resources into DF/stonesense
install(DIRECTORY resources/
        DESTINATION "stonesense"
        PATTERN "CVS" EXCLUDE
        PATTERN ".svn" EXCLUDE)
# install readme
install(FILES README.md DESTINATION stonesense)
