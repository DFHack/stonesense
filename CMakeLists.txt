PROJECT (stonesense)
SET(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMake/Modules)

include_directories (${CMAKE_SOURCE_DIR})


SET(PROJECT_SRCS
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
    UserInput.cpp
    WorldSegment.cpp
    VegetationConfiguration.cpp
    Keybinds.cpp
    main.cpp
)

IF(UNIX)
	IF(APPLE)
	# Mac OS X
		OPTION(STONESENSE_INTERNAL_DYLIB "Link with prebuilt internal allegro libs and headers." ON)
		IF(STONESENSE_INTERNAL_DYLIB)
			FILE(DOWNLOAD http://topazgryphon.org/~tcollett/df/allegro-osx.tar.bz2
						  ${stonesense_SOURCE_DIR}/allegro-osx.tar.bz2
						  EXPECTED_MD5 5637cc4ab5dc90f29de7788cb07a6e9e)
			execute_process(COMMAND tar xjf ${stonesense_SOURCE_DIR}/allegro-osx.tar.bz2
							WORKING_DIRECTORY ${stonesense_SOURCE_DIR})
			SET(PROJECT_LIBS
				${stonesense_SOURCE_DIR}/allegro-osx/lib/liballegro.5.0.dylib
				${stonesense_SOURCE_DIR}/allegro-osx/lib/liballegro_primitives.5.0.dylib
				${stonesense_SOURCE_DIR}/allegro-osx/lib/liballegro_font.5.0.dylib
				${stonesense_SOURCE_DIR}/allegro-osx/lib/liballegro_image.5.0.dylib
				${stonesense_SOURCE_DIR}/allegro-osx/lib/liballegro_ttf.5.0.dylib
				${PROJECT_LIBS}
			)
			include_directories (
				${include_directories}
				${stonesense_SOURCE_DIR}/allegro-osx/include
			)
			install(DIRECTORY allegro-osx/lib/
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
	ELSE(APPLE)
	#linux
		OPTION(STONESENSE_INTERNAL_SO "Link with prebuilt internal allegro libs and headers." ON)
		IF(STONESENSE_INTERNAL_SO)
			FILE(DOWNLOAD http://dethware.org/dfhack/download/allegro.tar.bz2
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
	ENDIF()
# windows
ELSE(UNIX)
    FILE(DOWNLOAD http://dethware.org/dfhack/download/allegrow.tar.gz
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
add_custom_command(TARGET stonesense POST_BUILD COMMAND ${stonesense_SOURCE_DIR}/fix-libs-stonesense.sh WORKING_DIRECTORY . COMMENT "Fixing Stonesense library dependencies...")
#install resources into DF/stonesense
install(DIRECTORY resources/
        DESTINATION "stonesense"
        PATTERN "CVS" EXCLUDE
        PATTERN ".svn" EXCLUDE)
# install readme
install(FILES README.txt DESTINATION stonesense)
