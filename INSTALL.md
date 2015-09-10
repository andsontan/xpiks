**OS X installation**

To install Xpiks in OS X do the following:

- download and install Exiftool
- unpack Xpiks from dmg and drag'n'drop it to the Applications

**Windows intallation**

Exiftool and curl are packed inside the zip archive. Download and extract Xpiks wherever you want. Launch xpiks-qt.exe.

**Debian installation**

Install the deb file from latest release with all dependencies.

**Ubuntu installation**

Currently two dependencies have to be installed manually:

- qtdeclarative5-controls-plugin
- qtdeclarative5-quicklayouts-plugin

Also there is an issue with application menu in Qt for Ubuntu which requires a workaround:

`sudo apt-get remove --purge appmenu-qt5`
