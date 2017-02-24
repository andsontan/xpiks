#!/bin/bash

cd build-Release/

~/Qt5.6.2/5.6/clang_64/bin/macdeployqt xpiks-qt.app -verbose=2 -dmg -executable=xpiks-qt.app/Contents/MacOS/xpiks-qt -qmldir=../ -qmldir=../Components/ -qmldir=../Constants/ -qmldir=../Dialogs/ -qmldir=../StyledControls/ -qmldir=../StackViews/ -qmldir=../CollapserTabs/

cd -

echo "Copying libraries..."

cp ../libs/libssdll.1.0.0.dylib build-Release/xpiks-qt.app/Contents/Frameworks/
cp ../libs/libface.1.0.0.dylib build-Release/xpiks-qt.app/Contents/Frameworks/
cp ../libs/libquazip.1.0.0.dylib build-Release/xpiks-qt.app/Contents/Frameworks/

echo "Changing dependency path..."

install_name_tool -change libssdll.1.dylib @executable_path/../Frameworks/libssdll.1.dylib "build-Release/xpiks-qt.app/Contents/MacOS/xpiks-qt"
install_name_tool -change libface.1.dylib @executable_path/../Frameworks/libface.1.dylib "build-Release/xpiks-qt.app/Contents/MacOS/xpiks-qt"
install_name_tool -change libquazip.1.dylib @executable_path/../Frameworks/libquazip.1.dylib "build-Release/xpiks-qt.app/Contents/MacOS/xpiks-qt"

echo "Linking dynamic libraries"

cd build-Release/xpiks-qt.app/Contents/Frameworks/
ln -s libssdll.1.0.0.dylib libssdll.1.dylib
ln -s libquazip.1.0.0.dylib libquazip.1.dylib
ln -s libface.1.0.0.dylib libface.1.dylib
cd -

echo "Done."
