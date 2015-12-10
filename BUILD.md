**How to build:**

***Prepare source tree***

- git clone Xpiks repo
- git submodule init & git submodule update - update tinyAES repo
- rename tinyAES/aes.c to tinyAES/aes.cpp

If you want to create Xpiks executable for everyday usage, before each `Run qmake` step described below, select `Release` in Qt Creator. Select `Debug` (the default) in all other cases.

***Obtain and build third-parties***
- download **Hunspell 1.3.3** from http://hunspell.sourceforge.net/ and extract it in the `src/` directory
- open project `src/hunspell/hunspell.pro` in Qt Creator, execute `Run qmake`, execute `Build`
- copy builded library (e.g. `libhunspell.a`) from the build directory to the `src/libs` directory
- open project `src/quazip/quazip/quazip.pro` in Qt Creator, execute `Run qmake`, execute `Build`
- copy builded library (e.g. `libquazip.a`) from the build directory to the `src/libs` directory
- [windows] you will also need to download **zlib 1.2.8** and extract it in the `src/` directory
- [windows] open project `src/zlib-project/zlib.pro` in Qt Creator, execute `Run qmake`, execute `Build`
- [windows] copy builded library (e.g. `z.lib`) from the build directory to the `src/libs` directory

***Generic build***

- install Qt 5.4 or later from official website
- open main project in Qt Creator, execute `Run qmake`, execute `Build`
- you can open, build and run tests project (`src/xpiks-tests/xpiks-tests.pro`) additionally

***Debian package build***

- Install build dependecies from repository (QT)
- cd to _xpiks-qt_ subdirectory
- `debuild -i -us -uc -b`

***More***

More instructions can be found at this page http://ribtoks.github.io/xpiks/blog/2014/building-xpiks-alone/
