PROJECT (stonesense)
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

IF(UNIX)
	IF(APPLE)
	# Mac OS X
		OPTION(STONESENSE_INTERNAL_DYLIB "Link with prebuilt internal allegro libs and headers." ON)
		IF(STONESENSE_INTERNAL_DYLIB)
			FILE(DOWNLOAD http://topazgryphon.org/~tcollett/df/allegro-osx.tar.bz2
						  ${stonesense_SOURCE_DIR}/allegro-osx.tar.bz2
						  EXPECTED_MD5 9f41ef3f3be8094100316ecbda1c607c)
			execute_process(COMMAND tar xjf ${stonesense_SOURCE_DIR}/allegro-osx.tar.bz2
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
    FILE(DOWNLOAD http://dethware.org/dfhack/download/allegrow509b.tar.gz
                  ${stonesense_SOURCE_DIR}/allegrow509b.tar.gz
                  EXPECTED_MD5 55b7e37ece699f02192346883ba38677)
    execute_process(COMMAND ${CMAKE_COMMAND} -E tar xzf ${stonesense_SOURCE_DIR}/allegrow509b.tar.gz
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
		allegro_color-static.lib
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
# Tell CMake the source won't be available until build time.
SET_SOURCE_FILES_PROPERTIES(${CMAKE_CURRENT_SOURCE_DIR}/../proto/RemoteFortressReader.pb.cc PROPERTIES GENERATED 1)

SET(PROJECT_SRCS ${PROJECT_SRCS} ${CMAKE_CURRENT_SOURCE_DIR}/../proto/RemoteFortressReader.pb.cc)

DFHACK_PLUGIN(stonesense ${PROJECT_SRCS} LINK_LIBRARIES dfhack-tinyxml ${PROJECT_LIBS})

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
