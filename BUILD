To build this project, you should:

- download and install Qt version 5.4 (or probably higher)
- pull tiny-AES submodule and rename aes.c to aes.cpp
- [Windows] if you're building for Windows, open zlib-project and build it for Debug/Release
- open main xpiks-qt project and build it for Debug/Release

Command to create standalone executable under OS X and Windows can be found here:

macdeployqt xpiks-qt.app -verbose=2 -dmg -executable=xpiks-qt.app/Contents/MacOS/xpiks-qt -qmldir=../xpiks-qt/ -qmldir=../xpiks-qt/Components/ -qmldir=../xpiks-qt/Constants/ -qmldir=../xpiks-qt/Dialogs/ -qmldir=../xpiks-qt/StyledControls/

windeployqt --qmldir=..\xpiks-qt\ --qmldir=..\xpiks-qt\Components\ --qmldir=..\xpiks-qt\Constants\ --qmldir=..\xpiks-qt\Dialogs\ --qmldir=..\xpiks-qt\StyledControls\ xpiks-qt.exe
