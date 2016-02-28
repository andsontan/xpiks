---
layout: post
title: "Xpiks 1.2 is out!"
date: 2016-02-27T14:08:11+02:00
---

Today I announce release of Xpiks 1.2! Actually this release was supposed to be later, but because of tons of users experiencing "cannot find Exiftool on Mac" problem, I decided that some features will slip into next Xpiks 1.3 release and this one will be publicly available ASAP. This release is special because work model with vectors was completely renovated: now you can drag'n'drop vectors and they will be attached to specific images. There's no more "Include vector" checkbox in the upload dialog as well.

Also "description" fields are now bigger on all of the popups.

Small changelog:

**Major features** of this release:

- switching between list and grid view options
- ability to explicitly attach vectors to previews
- automatic exiftool detection for OS X users
- upload progress reporting in taskbar on Windows
- ability to correctly upload vectors to Dreamtimes (to the 'additional' directory)
- logs are now highlighted

Minor stuff:

- logs are now splitted to file-per-session
- window size is saved and restored

Also Xpiks has Plugin API for building extensions now available. So separate developers can build their own extensions.

<div class="download-link"><a href="{{ site.url }}/downloads">Download</a></div>