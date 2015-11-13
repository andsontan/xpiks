**How to build:**

***Prepare source tree***

- git clone Xpiks repo
- git submodule init & git submodule update - update tinyAES repo
- rename tinyAES/aes.c to tinyAES/aes.cpp

***Generic build***

- install Qt 5.4 from official website
- open zlib project in Qt Creator, select Release, Run qmake, Build
- copy zlib library to the `libs/` directory
- open main project in Qt Creator, select Release, Run qmake, Build
- you can open, build and run tests project additionally

***Debian package build***

- Install build dependecies from repository (QT)
- cd to _xpiks-qt_ subdirectory
- `debuild -i -us -uc -b`

***More***

More instructions can be found at this page http://ribtoks.github.io/xpiks/blog/2014/building-xpiks-alone/
