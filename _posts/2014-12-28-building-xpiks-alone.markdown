---
layout: post
title: "How to build xpiks for your platform"
date: 2014-12-28T09:54:01+02:00
---

***Build instructions for your desktop platform:***

- download Qt 5.2 or higher for your platform from <a href="http://qt.io/">official website</a>
- while installing, add Qt bin directory to your PATH (Environmental variables)
- make sure you also have Git bin directory in your PATH
- download source code in <a href="{{ site.url }}/downloads">Downloads</a> section or at the <a href="https://github.com/Ribtoks/xpiks">GitHub</a>
- download source code or initialize the repository of tiny-AES (`git submodule init` and `git submodule update`)
- rename `tiny-AES/aes.c` to `tiny-AES/aes.cpp` for correct build
- <span class="gray">[Windows]</span> if you're building for Windows, build Zlib first (<a href="#zlib">see instructions below</a>)
- build <a href="#hunspell">Hunspell</a> and <a href="#quazip">Quazip</a> libraries (instructions below)
- build <a href="#libface">cpp-libface</a> library (instructions below)
- <span class="gray">[Windows]</span> build <a href="#libcurl">libcurl</a> library (instructions below)
- build <a href="#exiv2">Exiv2</a> library (instructions below)
- open `xpiks-qt.pro` file with Qt Creator installed with Qt
- select build target Release, press "Run qmake" and then "Build"
- now you can use xpiks-qt on your computer

***<span id="zlib">To build zlib under Windows for Xpiks, do the following:</span>***

- download zlib (1.2.8) source code and extract to `scr/zlib-1.2.8`
- open `src\zlib-project\zlib.pro` in Qt Creator, select `Release`, execute `Run qmake`, execute `Build`
- copy built library `.lib` (and z.pdb if you builded for Debug) to `src/libs` directory

***<span id="hunspell">To build Hunspell do the following:</span>***

- download **Hunspell 1.3.3** from http://hunspell.sourceforge.net/ and extract it in the `src/` directory
- open project `src/hunspell/hunspell.pro` in Qt Creator, select `Release`, execute `Run qmake`, execute `Build`
- copy built library (e.g. `libhunspell.a`, `hunspell.lib`) from the build directory to the `src/libs` directory

***<span id="quazip">To build Quazip do the following:</span>***

- open project `src/quazip/quazip/quazip.pro` in Qt Creator, select `Release`, execute `Run qmake`, execute `Build`
- copy built library (e.g. `libquazip.a`, `quazip.lib`) from the build directory to the `src/libs` directory

***<span id="libface">To build libface do the following:</span>***

- open file `src/cpp-libface/README.md` and follow instructions for your platform
- copy built library (e.g. `libface.a`, `face.lib`) from the build directory to the `src/libs` directory
- for Windows you will also need mman32 library (`mman.lib`) to be in the `src/libs` directory

***<span id="libcurl">To build Libcurl do the following:</span>***

- open git bash and type `git clone https://github.com/blackrosezy/build-libcurl-windows.git`
- execute `build.bat` and after if finishes, copy contents of `lib/dll-debug-x64` to the `src/libs/` directory

***<span id="exiv2">To build Exiv2 in Visual Studio 2015 do the following:</span>***

- download **exiv2 v0.25** library from <a href="http://www.exiv2.org/" target="_blanc">official website</a> and extract it somewhere
- open `msvc2005\exiv2.sln` and upgrade all projects
- from each project remove file `localtime.c`
- open file `include\exiv2\exv_msvs.h` and enable unicode support, disable png, curl and ssh (alternatively, you can overwrite this file with the one in the _Xpiks_ repository via path `src/exiv2-0.25/include/exiv2/exv_msvs.h`)
- in Visual Studio select `"Build" -> "Batch build"` and mark all _DebugDLL_ and _ReleaseDLL_ builds for your platform (x64 or x86)
- press "Build"
- copy build artifacts (`libexiv2.dll`, `libexiv2.lib`, `libexpat.dll`, `libexpat.lib`) to `src/libs` directory in _Xpiks_ repository

***Redistribution***

If you builded for Release and you wish to redistribute it for similar desktop platforms to yours, follow next steps:

- after build is successful, navigate to build directory, open it in terminal
- find macdeployqt, windeployqt or any similar tool in Qt bin directory
- read help of *deployqt tool and execute it with necessary parameters to include qt frameworks which are needed for build as well as QML directories

Sample macdeployqt command for OS X can be found below: (depends on your build-* directory path)

`macdeployqt xpiks-qt.app -verbose=2 -dmg -executable=xpiks-qt.app/Contents/MacOS/xpiks-qt -qmldir=../ -qmldir=../Components/ -qmldir=../Constants/ -qmldir=../Dialogs/ -qmldir=../StyledControls/`

And for Windows as well:

`windeployqt.exe --qmldir=../../xpiks-qt/Components/ --qmldir=../../xpiks-qt/Constants/ --qmldir=../../xpiks-qt/Dialogs/ --qmldir=../../xpiks-qt/StyledControls/ xpiks-qt.exe`

The RELEASE procedure for OS X, Windows:

- build Xpiks for Release using procedure described above
- use *deployqt tool (described above) to make Xpiks standalone
- [Windows] copy the exiftool to the directory with Xpiks binaries (this is not needed anymore starting from Xpiks 1.3.1)
- create the archive and that's it

Xpiks uses English dictionaries from [Libreoffice collection](http://cgit.freedesktop.org/libreoffice/dictionaries/tree/). In Windows they are stored in the `dict/` subdirectory, in OS X they are in the `Resources/` directory of a bundle. As of Xpiks 1.1 these files are copied automatically.
