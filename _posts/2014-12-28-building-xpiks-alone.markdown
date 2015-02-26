---
layout: post
title: "How to build xpiks for your platform"
date: 2014-12-28T09:54:01+02:00
---

Build instructions for your desktop platform:

- download latest Qt (5.4) for your platform
- while installing, add Qt bin directory to your PATH
- download source code in Downloads section of this release
- download source code or initialize the repository of tiny-AES
- rename tiny-AES/aes.c to tiny-AES/aes.cpp for correct build
- open xpiks-qt.pro file with Qt Creator installed with Qt
- select build target Release, press "Run qmake" and then "Build"
- now you can use xpiks-qt on your computer

If you wish to redistribute it for similar desktop platforms to yours, follow next steps:

- after build is successful, navigate to build directory, open it in terminar
- find macdeployqt, windeployqt or any similar tool in Qt bin directory
- read help of ..deployqt tool and execute it with necessary parameters to include qt frameworks which are needed for build as well as QML directories

Sample macdeployqt command for OS X can be found lower:

`macdeployqt xpiks-qt.app -verbose=2 -dmg -executable=xpiks-qt.app/Contents/MacOS/xpiks-qt -qmldir=../xpiks-qt/ -qmldir=../xpiks-qt/Components/ -qmldir=../xpiks-qt/Constants/ -qmldir=../xpiks-qt/Dialogs/ -qmldir=../xpiks-qt/StyledControls/`

And for Windows here:

`windeployqt.exe --qmldir=../../xpiks-qt/Components/ --qmldir=../../xpiks-qt/Constants/ --qmldir=../../xpiks-qt/Dialogs/ --qmldir=../../xpiks-qt/StyledControls/ xpiks-qt.exe`
