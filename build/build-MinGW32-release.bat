mkdir build-real
cd build-real
set path=c:\Program Files\CMake 2.6\bin;c:\MinGW\bin;%path%
cmake ..\.. -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE:string=Release
rem CMake should do this, but wont cooperate
windres -I..\.. -i ..\..\res.rc -o res_rc.obj 
mingw32-make
pause