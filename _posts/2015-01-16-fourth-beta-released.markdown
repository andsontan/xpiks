---
layout: post
title: "Beta version 0.4 of Xpiks released!"
date: 2015-01-16T09:13:01+02:00
---

I'm proud to announce fourth release of Cross-platform image keywording software v1.0-beta4! Few important improvements and fixes of old issues. You can find binaries at Downloads page.

In this release I managed to implement Drag&Drop feature, which had interesting issues in OS X, because dropping does not provide direct URL to file, but the OS X specific temp URL. I had to write Objective-Qt-C file with optional compilation which retrieved real URL. Also I added quite a useful feature for stockers: all artworks/photos can now be checked automaticaly for different simple errors: insufficient resolution, wrong number of keywords or too long description.

List of all changes:

- Added workfiles, which backup entered but not saved keywords and other metadata
- Added ability to Drag&Drop images to workflow
- FTP credentials checking on upload (notifies if something is missing e.g. username, password)
- Checks for potential problems on upload (insufficient resolution, missing metadata, too much keywords etc)

<div class="download-link"><a href="{{ site.url }}/downloads">Download</a></div>
