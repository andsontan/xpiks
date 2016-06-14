# Basic install #

**OS X installation**

To install Xpiks in OS X do the following:

- unpack Xpiks from dmg and drag'n'drop it to the Applications

Optionally you can download and install Exiftool (http://www.sno.phy.queensu.ca/~phil/exiftool/) and enable it in Xpiks Settings in the External tab.

**Windows intallation**

- extract Xpiks archive wherever you want. Launch xpiks-qt.exe

Also you can use the distribution of Xpiks as portable installation.

**Debian installation**

Install the deb file from latest release with all dependencies.

# Special instructions #
**Linux**

Install all recommended packages:
- libhunspell
- hunspell-en-us

**Ubuntu**
**Mint 17.3 (Rose)**

List of QML plugins to be installed manually (from standard repository):

- qtdeclarative5-dialogs-plugin
- qtdeclarative5-privatewidgets-plugin
- qtdeclarative5-qtquick2-plugin
- qtdeclarative5-quicklayouts-plugin
- qtdeclarative5-window-plugin
- qtdeclarative5-controls-plugin
- qml-module-qtgraphicaleffects

Also there is an issue with application menu in Qt for Ubuntu which requires a workaround:

`sudo apt-get remove --purge appmenu-qt5`

**OpenSuse installation**

Additional package have to be installed manually (from standard repository):

- libqt5-qtquickcontrols
