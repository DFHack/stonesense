#!/bin/bash

install_name_tool -change liballegro.5.0.dylib @executable_path/stonesense/deplibs/liballegro.5.0.dylib stonesense.plug.so
install_name_tool -change liballegro_primitives.5.0.dylib @executable_path/stonesense/deplibs/liballegro_primitives.5.0.dylib stonesense.plug.so
install_name_tool -change liballegro_image.5.0.dylib @executable_path/stonesense/deplibs/liballegro_image.5.0.dylib stonesense.plug.so
install_name_tool -change liballegro_font.5.0.dylib @executable_path/stonesense/deplibs/liballegro_font.5.0.dylib stonesense.plug.so
install_name_tool -change liballegro_ttf.5.0.dylib @executable_path/stonesense/deplibs/liballegro_ttf.5.0.dylib stonesense.plug.so

echo "Stonesense libs fixed.\n"