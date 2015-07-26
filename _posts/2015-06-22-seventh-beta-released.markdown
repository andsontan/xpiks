---
layout: post
title: "Beta version 0.7 of Xpiks released!"
date: 2015-06-22T23:39:01+03:00
---

I'm proud to announce seventh release of Cross-platform image keywording software v1.0-beta7! This time some minor bugs fixed and tons of new features are introduced! First of all I implemented Undo-Redo functionality which now allows to skip confirmation dialogs for some actions. Then I set lots of animation for adding and removing items (Transitions) for ListView items for main window and combined edit. Another requested feature was zipping via 1-click, so I used QuaZip (Qt zip library) and integrated it source code to Xpiks. Last but not least was that I refactored tons of code and introduced quite normal dependency injection in C++ classes.

List of all changes:

- Added ability to Undo major actions (removing/adding artworks, editing in combined edit)
- Made smooth transitions in UI for adding/removing artworks
- Added ability to cancel upload to stocks #81
- Added ability to zip selected illustrations with EPS files (useful for some stocks) #79
- Fixed issue with newline-separated keywords #82
- Fix for Save and Append button #62
- Refactored code and introduced single responsibility entity (useful for undo/redo and tests)
- Added ignore autosave checkbox for import #77
- Added warranty disclaimer message
- Added several integration tests

<div class="download-link"><a href="{{ site.url }}/downloads">Download</a></div>
