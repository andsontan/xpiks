Create several strings files in the translations directory (run command from xpiks-qt.pro directory)
lupdate.exe -pro xpiks-qt.pro -ts translations/xpiks_en_US.ts

Create .qm files (to be put in the translations/ dir)
lrelease.exe *.ts

Copy resulted qm files to the translations/ dir

mv *.qm ../../build-xpiks-qt-Desktop_Qt_5_5_1_MSVC2013_64bit-Debug/debug/translations/
