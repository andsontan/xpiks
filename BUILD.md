**The most detailed instructions** can be found at this page: **http://ribtoks.github.io/xpiks/blog/2014/building-xpiks-alone**

*

This space was intentionally left blank.

*

**Simplified instructions:**

***Prepare the source tree***

- git clone Xpiks repo
- `git submodule init` & `git submodule update --recursive`
- rename `tinyAES/aes.c` to `tinyAES/aes.cpp`
- ensure you have `git` available in your PATH

***Mac OS X / Windows : build third-parties***

Zlib:

- [windows] you will also need to download **zlib 1.2.11** and extract it in the `src/` directory
- [windows] open project `src/zlib-project/zlib.pro` in Qt Creator, execute `Run qmake`, execute `Build`
- [windows] copy built library (e.g. `z.lib`) from the build directory to the `src/libs` directory

Hunspell:

- download **Hunspell 1.6.0** from http://hunspell.sourceforge.net/ and extract it in the `src/` directory
- open project `src/hunspell/hunspell.pro` in Qt Creator, execute `Run qmake`, execute `Build`
- copy built library (e.g. `libhunspell.a`) from the build directory to the `src/libs` directory

cpp-libface:

- follow instructions in `src/cpp-libface/README.md` to build libface
- copy built library to `src/libs` directory

ssdll:

- follow instructions in `src/ssdll/README.md` to build ssdll
- copy built library to `src/libs` directory

Quazip:

- open project `src/quazip/quazip/quazip.pro` in Qt Creator, execute `Run qmake`, execute `Build`
- copy built library (e.g. `quazip.lib` and `quazip.dll`) from the build directory to the `src/libs` directory

LibCurl:

- [windows] do `git clone https://github.com/blackrosezy/build-libcurl-windows.git` and execute `build.bat`
- [windows] copy contests of `lib/dll-debug-x64` contents to the `src/libs/` directory
- [windows] build exiv2 library (see steps below)

***OS X build of exiv2 library***

- open Terminal in the directory of exiv2 sources and do the following
- `mkdir build-Debug && cd build-Debug`
- `/Applications/CMake.app/Contents/bin/cmake .. -DEXIV2_ENABLE_SHARED=off -DCMAKE_BUILD_TYPE=Debug -DCMAKE_OSX_DEPLOYMENT_TARGET=10.7 -DCMAKE_CXX_STANDARD=11`
- `make`

***Visual Studio 2015 build of exiv2 library***

- download **exiv2 v0.25** library from http://www.exiv2.org/ and extract it somewhere
- download **expat-2.1.1** library from https://sourceforge.net/projects/expat/ and extract it on same level as exiv2
- rename directory `expat-2.1.1/` to `expat\`
- open `msvc2005\exiv2.sln` and upgrade all projects
- from each project remove file `localtime.c`
- open file `include\exiv2\exv_msvs.h` and enable unicode support, disable png, curl and ssh (alternatively, you can overwrite this file with the one in the _Xpiks_ repository via path `src/exiv2-0.25/include/exiv2/exv_msvs.h`)
- in Visual Studio select `"Build" -> "Batch build"` and mark all _DebugDLL_ and _ReleaseDLL_ builds for your platform (x64 or x86)
- press "Build"
- copy build artifacts (`libexiv2.dll`, `libexiv2.lib`, `libexpat.dll`, `libexpat.lib`) to `src/libs` directory in _Xpiks_ repository

***Manual build***
- install Qt 5.4 or later from official website (or 5.2 or later from repository in case of Linux)
- [linux] install development packages for `libcurl`, `hunspell`, `quazip`, `zlib`, `libexiv`
- [linux] install `exiftool` (this **is not** necessary since Xpiks 1.3.1) and `expat` (this **is** necessary since Xpiks 1.3.1)
- open main project in Qt Creator, execute `Run qmake`, execute `Build`
- you can open, build and run tests project (`src/xpiks-tests/xpiks-tests.pro`) additionally

If you want to create Xpiks executable for everyday usage, before each `Run qmake` step described below, select `Release` in Qt Creator. Select `Debug` (the default) in all other cases.

**Linux package build**

This chapter assumes that you have installed generic development packages for your OS.

***Additional requirements***

In some cases you neet to install `libcurl-devel`, `hunspell-devel`, `quazip-devel`, `libexiv-devel` packages (they can have slightly different names in your distribution) if it is not installed as a build requirement.

***Debian package build***

- Install build dependecies from repository (QT)
- cd to _xpiks-qt_ subdirectory
- `debuild -i -us -uc -b`

***OpenSUSE RPM build***

- Install rpmbuild.
- `mkdir -p ~/rpmbuild/{RPMS,SRPMS,SPECS,BUILD,BUILDROOT,SOURCES}`
- `tar -czf xpiks-qt.tar.gz xpiks-qt/ tiny-aes/`
- Drop this source tarball in `~/rpmbuild/SOURCES`
- Drop the the spec file found in `xpiks-qt/RPM` folder to `~/rpmbuild/SPECS`
- `rpmbuild -ba ~/rpmbuild/SPECS/xpiks.spec`

***More***

Xpiks uses English dictionaries from http://cgit.freedesktop.org/libreoffice/dictionaries/tree/. In Windows they are stored in the `dict/` subdirectory, in OS X they are in the `Resources/` directory of a bundle.
