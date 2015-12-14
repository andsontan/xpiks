# Basic install #

**OS X installation**

To install Xpiks in OS X do the following:

- download and install Exiftool
- unpack Xpiks from dmg and drag'n'drop it to the Applications

**Windows intallation**

Exiftool and curl are packed inside the zip archive. Download and extract Xpiks wherever you want. Launch xpiks-qt.exe.

**Debian installation**

Install the deb file from latest release with all dependencies.

# Special instructions #

**Ubuntu**

**Mint 17.3 (Rose)**

List of QML plugins to be installed manually (from standard repository):

- qtdeclarative5-dialogs-plugin
- qtdeclarative5-privatewidgets-plugin
- qtdeclarative5-qtquick2-plugin
- qtdeclarative5-quicklayouts-plugin
- qtdeclarative5-window-plugin
- qtdeclarative5-controls-plugin

Also there is an issue with application menu in Qt for Ubuntu which requires a workaround:

`sudo apt-get remove --purge appmenu-qt5`


**OpenSuse installation**

Additional package have to be installed manually (from standard repository):

- libqt5-qtquickcontrols
