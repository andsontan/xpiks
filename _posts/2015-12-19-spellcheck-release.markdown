---
layout: post
title: "Second stable release of Xpiks is out!"
date: 2015-12-19T20:55:24+02:00
---

Today I announce release of Xpiks 1.1! Main two features of this release are spell checking and speed improvements. So now you can check spelling in description, title and keywords using en_US dictionary available from LibreOffice suite. Also Xpiks will suggest you corrections for words with bad spelling. What about speed is that I optimized visual and loading parts so now Xpiks should work faster with large collections of artworks! Also I've implemented a lot of small requests from actual users.

As usual, small changelog:

**Major features** of this release:

- Automatic spell checking and suggestion for all metadata #148
- Automatic checking for updates #78
- Added plain text mode for keywords editing #176
- Speed improvements with large collections #172 #186 #175 #174
- Editing of separate keywords
- Improvements for UI of some popups on large monitors
- Improvements on keywords pasting #179
- Exact search terms parsing (with exclamation mark before word like "!man")

Also, a lot of small UI issues and glitches were fixed!

Known issues:

- Xpiks proposes spell suggestions for all words with errors, what creates duplicates if one word has been used several times in metadata (keywords, title, description)
