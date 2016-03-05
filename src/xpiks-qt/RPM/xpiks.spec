Summary: Xpiks
Name: xpiks
Version: 1.2
Release: 1
License: GPLv3
Group: Applications/Internet
Source: xpiks-qt.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot
BuildRequires: libqt5-qtdeclarative-devel libqt5-qtsvg-devel libqt5-qttools-devel libquazip1 libqt5-qtquick1-devel quazip-devel hunspell-devel curl-devel

%description
Cross-platform (X) Photo Keywording Software
 Xpiks is a free and open source keywording and uploading tool for
 microstock photographers and illustrators with no hidden fees.
 If you're tired with copy and pasting keywords from one stock to another,
 Xpiks would be a great time saver for you!

%prep
%setup -q -n xpiks-qt

%build
qmake-qt5 -r -spec linux-g++-64

%install
rm -rf %{buildroot}
make install INSTALL_ROOT="%buildroot";
mkdir -p %{buildroot}%{_datadir}/applications/
install -D %{_builddir}/xpiks-qt/debian/xpiks.desktop %{buildroot}%{_datadir}/applications/
mkdir -p %{buildroot}%{_datadir}/icons/
install -D %{_builddir}/xpiks-qt/debian/xpiks.png %{buildroot}%{_datadir}/icons/
mkdir -p %{buildroot}%{_datadir}/Xpiks/Xpiks/
install -D %{_builddir}/xpiks-qt/whatsnew.txt %{buildroot}%{_datadir}/Xpiks/Xpiks/
install -D %{_builddir}/xpiks-qt/terms_and_conditions.txt %{buildroot}%{_datadir}/Xpiks/Xpiks/

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%{_bindir}/*
%{_datadir}/*


%changelog
* Wed Mar 2 2016 Artiom Molchanov <a.mv@gmx.fr> 
- RPM build on OpenSuse
