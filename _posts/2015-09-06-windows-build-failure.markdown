---
layout: post
title: "All Windows builds broken upto 0.8"
date: 2015-09-06T15:15:01+02:00
---

I've found issue with all archives with Windows builds of Xpiks: they were completely useless if you don't have Qt installed on your system. So I suppose it affected almost all users (no many users have Qt installed). Anyway, since noone reported it to me I guess I don't have that much Windows users now.

The bug was that I didn't include QtQuick dlls with other binaries and main windows of Xpiks would never appear. I copied needed directories and libraries and it seems to work now.

Anyway, 0.8 build is fixed now and you can grab it from <a href="{{ site.url }}/downloads">Downloads</a> page.

Enjoy this release!