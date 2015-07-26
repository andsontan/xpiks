---
layout: post
title: "Beta version 0.5 of Xpiks released!"
date: 2015-01-25T19:39:01+02:00
---

I'm proud to announce fifth release of Cross-platform image keywording software v1.0-beta5! This release contains a lot of UI fixes. You can find binaries at Downloads page.

In this release I focused on fixing bugs and enchancing internal Qt/C++ stuff. I implemented Qt-way binding of keywords to UI control (and had good fun with C++ memory management vs Qml memory management, when Qt deleted my objects). Also I made all popups movable (just for fun).

List of all changes:

- Fixes for non-updated listview, existance of EPS file on upload and other
- Added checks if file is actually a jpeg image
- Added a very basic versioning and About window with version
- Fixed import issue when importing again all items
- Fixes for loading from tempdb data and modified state
- Ability to clear logs
- Changed the way keywords list is bound to c++ code
- Made all dialogs movable
- and many other minor

<div class="download-link"><a href="{{ site.url }}/downloads">Download</a></div>
