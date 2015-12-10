---
layout: post
title: "How to build xpiks for your platform"
date: 2014-12-28T09:54:01+02:00
---

***Build instructions for your desktop platform:***

- download Qt 5.2 or higher for your platform from <a href="http://qt.io/">official website</a>
- while installing, add Qt bin directory to your PATH (Environmental variables)
- download source code in <a href="{{ site.url }}/downloads">Downloads</a> section or at the <a href="https://github.com/Ribtoks/xpiks">GitHub</a>
- download source code or initialize the repository of tiny-AES (`git submodule init` and `git submodule update`)
- rename `tiny-AES/aes.c` to `tiny-AES/aes.cpp` for correct build
- <span class="gray">[Windows]</span> if you're building for Windows, build Zlib first (<a href="#zlib">see instructions below</a>)
- build <a href="#hunspell">Hunspell</a> and <a href="#quazip">Quazip</a> (instructions below)
- open xpiks-qt.pro file with Qt Creator installed with Qt
- select build target Release, press "Run qmake" and then "Build"
- now you can use xpiks-qt on your computer

***<span id="zlib">To build zlib under Windows for Xpiks, do the following:</span>***

- download zlib (1.2.8) source code and extract to `scr/zlib-1.2.8`
- open `src\zlib-project\zlib.pro` in Qt Creator, select `Release`, execute `Run qmake`, execute `Build`
- copy builded library `.lib` (and z.pdb if you builded for Debug) to `src/libs` directory

***<span id="hunspell">To build Hunspell do the following:</span>***

- download **Hunspell 1.3.3** from http://hunspell.sourceforge.net/ and extract it in the `src/` directory
- open project `src/hunspell/hunspell.pro` in Qt Creator, select `Release`, execute `Run qmake`, execute `Build`
- copy builded library (e.g. `libhunspell.a`, `hunspell.lib`) from the build directory to the `src/libs` directory

***<span id="quazip">To build Quazip do the following:</span>***

- open project `src/quazip/quazip.pro` in Qt Creator, select `Release`, execute `Run qmake`, execute `Build`
- copy builded library (e.g. `libquazip.a`, `quazip.lib`) from the build directory to the `src/libs` directory


If you builded for Release and you wish to redistribute it for similar desktop platforms to yours, follow next steps:

- after build is successful, navigate to build directory, open it in terminal
- find macdeployqt, windeployqt or any similar tool in Qt bin directory
- read help of *deployqt tool and execute it with necessary parameters to include qt frameworks which are needed for build as well as QML directories

Sample macdeployqt command for OS X can be found below: (depends on your build-* directory path)

`macdeployqt xpiks-qt.app -verbose=2 -dmg -executable=xpiks-qt.app/Contents/MacOS/xpiks-qt -qmldir=../ -qmldir=../Components/ -qmldir=../Constants/ -qmldir=../Dialogs/ -qmldir=../StyledControls/`

And for Windows as well:

`windeployqt.exe --qmldir=../../xpiks-qt/Components/ --qmldir=../../xpiks-qt/Constants/ --qmldir=../../xpiks-qt/Dialogs/ --qmldir=../../xpiks-qt/StyledControls/ xpiks-qt.exe`

The RELEASE procedure for OS X, Windows:

- build zlib for Release and put it to `libs/` directory (as specified above)
- build Xpiks for Release
- use *deployqt tool (described above) to make Xpiks standalone
- copy the exiftool to the directory with xpiks binaries
- copy the curl (including additional libs) to the directory with xpiks binaries
- create the archive and that's it
