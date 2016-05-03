Create several strings files in the translations directory (run command from xpiks-qt.pro directory)
`lupdate.exe -pro xpiks-qt.pro -ts deps/translations/xpiks_en_US.ts`

To build .qm files out of .ts files, run
`lrelease.exe *.ts`

.qm files should remain in the `src/xpiks-qt/deps/translations` directory. When you will `Run qmake` and `Build` on the _xpiks-qt_ project, they will be automatically added to `translations/` directory next to _xpiks-qt_ executable. 

