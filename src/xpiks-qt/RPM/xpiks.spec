Summary: Xpiks
Name: xpiks
Version: 1.4
Release: 1
License: GPLv3
Group: Applications/Internet
Source: xpiks-qt.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot
BuildRequires: libqt5-qtdeclarative-devel libqt5-qtsvg-devel libqt5-qttools-devel libqt5-linguist-devel libquazip-qt5-devel  quazip-devel hunspell-devel curl-devel libexiv2-devel
Requires: libqt5-qtgraphicaleffects libqt5-qtquickcontrols

%description
Cross-platform (X) Photo Keywording Software
 Xpiks is a free and open source keywording and uploading tool for
 microstock photographers and illustrators with no hidden fees.
 If you're tired with copy and pasting keywords from one stock to another,
 Xpiks would be a great time saver for you!

%prep
%setup -q -n xpiks-qt

%build
cd ../cpp-libface
make install-lib
cd -
cd ../ssdll/src/ssdll
qmake -r QMAKE_CXXFLAGS+=-std=gnu++11 -spec linux-g++-64
make install
cd -

cd deps/translations
make
cd -
qmake-qt5 -r -spec linux-g++-64

%install
rm -rf %{buildroot}
make install INSTALL_ROOT="%buildroot";
mkdir -p %{buildroot}%{_datadir}/applications/
install -D %{_builddir}/xpiks-qt/debian/xpiks.desktop %{buildroot}%{_datadir}/applications/
mkdir -p %{buildroot}%{_datadir}/icons/
install -D %{_builddir}/xpiks-qt/debian/xpiks.png %{buildroot}%{_datadir}/icons/
mkdir -p %{buildroot}%{_datadir}/Xpiks/Xpiks/
install -D %{_builddir}/xpiks-qt/deps/whatsnew.txt %{buildroot}%{_datadir}/Xpiks/Xpiks/
install -D %{_builddir}/xpiks-qt/deps/terms_and_conditions.txt %{buildroot}%{_datadir}/Xpiks/Xpiks/
mkdir -p %{buildroot}%{_datadir}/Xpiks/Xpiks/translations
install -D %{_builddir}/xpiks-qt/deps/translations/*.qm %{buildroot}%{_datadir}/Xpiks/Xpiks/translations

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%{_bindir}/*
%{_datadir}/*


%changelog
* Wed Mar 2 2016 Artiom Molchanov <a.mv@gmx.fr> 
- RPM build on OpenSuse
