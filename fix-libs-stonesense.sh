#!/bin/bash

install_name_tool -change liballegro.5.0.9.dylib @executable_path/hack/liballegro.5.0.dylib stonesense.plug.dylib
install_name_tool -change liballegro_primitives.5.0.9.dylib @executable_path/hack/liballegro_primitives.5.0.dylib stonesense.plug.dylib
install_name_tool -change liballegro_image.5.0.9.dylib @executable_path/hack/liballegro_image.5.0.dylib stonesense.plug.dylib
install_name_tool -change liballegro_font.5.0.9.dylib @executable_path/hack/liballegro_font.5.0.dylib stonesense.plug.dylib
install_name_tool -change liballegro_ttf.5.0.9.dylib @executable_path/hack/liballegro_ttf.5.0.dylib stonesense.plug.dylib
install_name_tool -change liballegro_color.5.0.9.dylib @executable_path/hack/liballegro_color.5.0.dylib stonesense.plug.dylib

echo "Stonesense libs fixed."
