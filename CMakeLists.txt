PROJECT (stonesense)

SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/plugins/stonesense/configs/
        DESTINATION "${DFHACK_DATA_DESTINATION}/data/dfhack-config-defaults/stonesense")

SET(PROJECT_SRCS
    TrackingModes.cpp
    Tile.cpp
    TileCondition.cpp
    TileTree.cpp
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
    MaterialMatcher.cpp
    main.cpp
)

#proto files to include.
SET(PROJECT_HDRS
    BuildingConfiguration.h
    ColorConfiguration.h
    common.h
    commonTypes.h
    ConditionalSprite.h
    Config.h
    Constructions.h
    ContentBuildingReader.h
    ContentLoader.h
    CreatureConfiguration.h
    Creatures.h
    DumpInfo.h
    FluidConfiguration.h
    GameBuildings.h
    GameConfiguration.h
    GameState.h
    GroundMaterialConfiguration.h
    GUI.h
    ItemConfiguration.h
    MapLoading.h
    MaterialMatcher.h
    OcclusionTest.h
    SegmentProcessing.h
    SpriteColors.h
    SpriteMaps.h
    SpriteObjects.h
    StonesenseState.h
    Tile.h
    TileCondition.h
    TileDirection.h
    TileTree.h
    TrackingModes.h
    TreeGrowthConfiguration.h
    UserInput.h
    VegetationConfiguration.h
    WorldSegment.h
)

SET_SOURCE_FILES_PROPERTIES(${PROJECT_HDRS} PROPERTIES HEADER_FILE_ONLY TRUE)

# mash them together (headers are marked as headers and nothing will try to compile them)
LIST(APPEND PROJECT_SRCS ${PROJECT_HDRS})

SET(ALLEGRO_LIBS
    allegro
    allegro_primitives
    allegro_font
    allegro_color
    allegro_image
    allegro_ttf
)

IF(UNIX)
    IF(APPLE)
    # Mac OS X
        OPTION(STONESENSE_INTERNAL_DYLIB "Link with prebuilt internal allegro libs and headers." ON)
        IF(STONESENSE_INTERNAL_DYLIB)
            set(ALLEGRO_DOWNLOAD_DIR ${stonesense_SOURCE_DIR}/allegro/osx${DFHACK_BUILD_ARCH})
            MESSAGE(FATAL_ERROR "OSX build not supported for stonesense")
            if("${DFHACK_BUILD_ARCH}" STREQUAL "64")
                set(ALLEGRO_DOWNLOAD_FILE "osx64-allegro-5.2.2.tar.bz2")
                download_file("https://github.com/DFHack/dfhack-bin/releases/download/0.44.09/osx64-allegro-5.2.2.tar.bz2"
                    "${ALLEGRO_DOWNLOAD_DIR}/${ALLEGRO_DOWNLOAD_FILE}"
                    "306e8981bf67cf8b3f9ac0b26003869e")
            else()
                set(ALLEGRO_DOWNLOAD_FILE "osx32-allegro-5.0.11.tar.bz2")
                download_file("https://github.com/DFHack/dfhack-bin/releases/download/0.44.09/osx32-allegro-5.0.11.tar.bz2"
                    "${ALLEGRO_DOWNLOAD_DIR}/${ALLEGRO_DOWNLOAD_FILE}"
                    "c531eb16f2530e1168116fcbce7644df")
            endif()
            execute_process(COMMAND tar xjf "${ALLEGRO_DOWNLOAD_DIR}/${ALLEGRO_DOWNLOAD_FILE}" --strip-components=1
                            WORKING_DIRECTORY ${ALLEGRO_DOWNLOAD_DIR})

            # Download freetype only on x64
            if("${DFHACK_BUILD_ARCH}" STREQUAL "64")
                download_file_unzip("https://github.com/DFHack/dfhack-bin/releases/download/0.44.09/osx64-libfreetype.6.dylib.gz"
                    "gz"
                    ${ALLEGRO_DOWNLOAD_DIR}/lib/libfreetype.6.dylib.gz
                    "f4c4005a53a353802492ab1a76f188b6"
                    ${ALLEGRO_DOWNLOAD_DIR}/lib/libfreetype.6.dylib
                    "f593477141d6f10c510dca7dbc10dab8")
            endif()

            foreach(ALLEGRO_LIB ${ALLEGRO_LIBS})
                list(APPEND PROJECT_LIBS ${ALLEGRO_DOWNLOAD_DIR}/lib/lib${ALLEGRO_LIB}.dylib)
            endforeach()

            SET(stonesense_INCLUDES ${ALLEGRO_DOWNLOAD_DIR}/include ${ALLEGRO_DOWNLOAD_DIR}/lib/Headers)
            if(BUILD_PLUGINS)
                install(DIRECTORY ${ALLEGRO_DOWNLOAD_DIR}/lib/
                        DESTINATION ${DFHACK_LIBRARY_DESTINATION})
            endif()
        ELSE()
            foreach(ALLEGRO_LIB ${ALLEGRO_LIBS})
                list(APPEND PROJECT_LIBS ${ALLEGRO_LIB})
            endforeach()
        ENDIF()
    ELSE(APPLE)
    #linux
        OPTION(STONESENSE_INTERNAL_SO "Link with prebuilt internal allegro libs and headers." ON)
        IF(STONESENSE_INTERNAL_SO)
            set(ALLEGRO_DOWNLOAD_DIR ${stonesense_SOURCE_DIR}/allegro/linux${DFHACK_BUILD_ARCH})
            if("${DFHACK_BUILD_ARCH}" STREQUAL "64")
                set(ALLEGRO_DOWNLOAD_FILE "linux64-allegro5210.tbz2")
                download_file("https://github.com/DFHack/dfhack-bin/releases/download/50.15/linux64-allegro5210.tbz2"
                    "${ALLEGRO_DOWNLOAD_DIR}/${ALLEGRO_DOWNLOAD_FILE}"
                    "14c4237fbe6f2abf26edc0ba4d6c4424")
            else()
                MESSAGE(FATAL_ERROR "32 bit build not supported for stonesense")
            endif()
            execute_process(COMMAND tar xf "${ALLEGRO_DOWNLOAD_DIR}/${ALLEGRO_DOWNLOAD_FILE}" --strip-components=1
                            WORKING_DIRECTORY ${ALLEGRO_DOWNLOAD_DIR})
            foreach(ALLEGRO_LIB ${ALLEGRO_LIBS})
                list(APPEND PROJECT_LIBS ${ALLEGRO_DOWNLOAD_DIR}/lib/lib${ALLEGRO_LIB}.so.5.2)
            endforeach()
            SET(stonesense_INCLUDES ${ALLEGRO_DOWNLOAD_DIR}/include)
            if(BUILD_PLUGINS)
                install(DIRECTORY ${ALLEGRO_DOWNLOAD_DIR}/lib/
                        DESTINATION ${DFHACK_LIBRARY_DESTINATION})
            endif()
        ELSE()
            foreach(ALLEGRO_LIB ${ALLEGRO_LIBS})
                list(APPEND PROJECT_LIBS ${ALLEGRO_LIB})
            endforeach()
        ENDIF()
    ENDIF()
# windows
ELSE(UNIX)
    SET(ALLEGRO_DOWNLOAD_DIR ${stonesense_SOURCE_DIR}/win${DFHACK_BUILD_ARCH})
    IF(DFHACK_BUILD_64)
        download_file("https://github.com/DFHack/dfhack-bin/releases/download/50.15/allegro-msvc-x64-5.2.10.7z"
                      ${ALLEGRO_DOWNLOAD_DIR}/allegro-msvc-x64-5.2.10.7z
                      "ff9eb09893ce7ff560de618817db043a")
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${ALLEGRO_DOWNLOAD_DIR}/allegro-msvc-x64-5.2.10.7z
                        WORKING_DIRECTORY ${ALLEGRO_DOWNLOAD_DIR})
        download_file("https://github.com/DFHack/dfhack-bin/releases/download/50.15/allegro_deps-msvc-x64-1.15.0.7z"
                      ${ALLEGRO_DOWNLOAD_DIR}/allegro_deps-msvc-x64-1.15.0.7z
                      "6ae351c319bdbad7dc6792a07428e71f")
        execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${ALLEGRO_DOWNLOAD_DIR}/allegro_deps-msvc-x64-1.15.0.7z
                        WORKING_DIRECTORY ${ALLEGRO_DOWNLOAD_DIR})
    ELSE()
        MESSAGE(FATAL_ERROR "32 bit build not supported for stonesense")
    ENDIF()
    foreach(ALLEGRO_LIB ${ALLEGRO_LIBS})
        list(APPEND PROJECT_LIBS ${ALLEGRO_LIB}.lib)
        if(BUILD_PLUGINS)
            install(FILES ${stonesense_SOURCE_DIR}/win${DFHACK_BUILD_ARCH}/Allegro/bin/${ALLEGRO_LIB}-5.2.dll
                DESTINATION .)
        endif()
    endforeach()
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
        ${PROJECT_LIBS}
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
       )
    SET(stonesense_INCLUDES
        ${stonesense_SOURCE_DIR}/win${DFHACK_BUILD_ARCH}/Allegro/include
        ${stonesense_SOURCE_DIR}/win${DFHACK_BUILD_ARCH}/Allegro_Deps/include)
    LINK_DIRECTORIES(
        ${LINK_DIRECTORIES}
        ${stonesense_SOURCE_DIR}/win${DFHACK_BUILD_ARCH}/Allegro/lib
        ${stonesense_SOURCE_DIR}/win${DFHACK_BUILD_ARCH}/Allegro_Deps/lib
    )
ENDIF(UNIX)

set(STONESENSE_GCC_FLAGS "")
if(UNIX AND "${CMAKE_CXX_COMPILER_VERSION}" VERSION_GREATER 6.0)
    set(STONESENSE_GCC_FLAGS "${STONESENSE_GCC_FLAGS} -Wno-misleading-indentation")
endif()

DFHACK_PLUGIN(stonesense ${PROJECT_SRCS} LINK_LIBRARIES ${DFHACK_TINYXML} ${PROJECT_LIBS} COMPILE_FLAGS_MSVC "/FI\"Export.h\" /wd4312 /wd4996 /wd4251" COMPILE_FLAGS_GCC "-include Export.h ${STONESENSE_GCC_FLAGS}")
if(BUILD_PLUGINS)
    target_include_directories(stonesense PRIVATE ${stonesense_INCLUDES})
endif()

IF(MSVC)
    # Ignore unhelpful warnings about debug symbols being missing.
    SET_TARGET_PROPERTIES(stonesense PROPERTIES LINK_FLAGS "/ignore:4098 /ignore:4099")
ENDIF(MSVC)

IF(APPLE)
    add_custom_command(TARGET stonesense POST_BUILD COMMAND ${stonesense_SOURCE_DIR}/fix-libs-stonesense.sh WORKING_DIRECTORY . COMMENT "Fixing Stonesense library dependencies...")
ENDIF(APPLE)

if(INSTALL_DATA_FILES)
    #install resources and readme into DF/stonesense
    install(DIRECTORY resources/ DESTINATION stonesense)
    install(FILES README.md DESTINATION stonesense)
endif()
