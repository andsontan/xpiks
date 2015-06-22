To build this project, you should:

- download and install Qt version 5.4 (or probably higher)
- pull tiny-AES submodule and rename aes.c to aes.cpp
- [Windows] if you're building for Windows, build Zlib (see instructions below)
- open main xpiks-qt project and build it for Debug/Release

To build zlib under Windows for Xpiks, do:
  
- download zlib (1.2.8) source code and extract to scr/zlib-1.2.8
- open zlib-project and build it for Debug/Release
- copy z.lib and z.pdb to src/libs directory

Command to create standalone executable under OS X and Windows can be found here:

macdeployqt xpiks-qt.app -verbose=2 -dmg -executable=xpiks-qt.app/Contents/MacOS/xpiks-qt -qmldir=../xpiks-qt/ -qmldir=../xpiks-qt/Components/ -qmldir=../xpiks-qt/Constants/ -qmldir=../xpiks-qt/Dialogs/ -qmldir=../xpiks-qt/StyledControls/

windeployqt --qmldir=..\xpiks-qt\ --qmldir=..\xpiks-qt\Components\ --qmldir=..\xpiks-qt\Constants\ --qmldir=..\xpiks-qt\Dialogs\ --qmldir=..\xpiks-qt\StyledControls\ xpiks-qt.exe
