To build this project, you should download and install Qt version 5.4
(or probably higher), open project file and build for Release on your platform.

Also you would have to pull tiny-AES submodule and rename aes.c to aes.cpp in order 
to be built correctly.

Command to create standalone executable under OS X and Windows can be found here:

macdeployqt xpiks-qt.app -verbose=2 -dmg -executable=xpiks-qt.app/Contents/MacOS/xpiks-qt -qmldir=../xpiks-qt/ -qmldir=../xpiks-qt/Components/ -qmldir=../xpiks-qt/Constants/ -qmldir=../xpiks-qt/Dialogs/ -qmldir=../xpiks-qt/StyledControls/

windeployqt --qmldir=..\xpiks-qt\ --qmldir=..\xpiks-qt\Components\ --qmldir=..\xpiks-qt\Constants\ --qmldir=..\xpiks-qt\Dialogs\ --qmldir=..\xpiks-qt\StyledControls\ xpiks-qt.exe
