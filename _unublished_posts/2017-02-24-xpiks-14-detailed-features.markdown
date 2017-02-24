---
layout: post
title: "Xpiks 1.4 detailed overview"
date: 2017-02-24T15:15:52+01:00
---

Xpiks 1.4 release development took as much time as usual 3 releases combined so it's worth spending some time diving into the features it brings to the users.

First easily notable thing is the user interface change. Not only colors are more contrast in both themes, text is better visible, but also the whole layout is different now. You can play with the panel on the left which is available in all editing modes and shares an additional buffer for metadata and translator. You can collapse it if it bothers you or expand it back if you feel like using it's features. The UI strives to be more adaptive and useful now on.

<img alt="Main window" src="{{site.url}}/images/screenshots/xpiks-qt-dark.jpg" class="small-12 large-8" />

Also one file and multiple editing UI has changed significantly. For one file editing you can switch between available artworks on the bottom without going back and forth to main view. Also now you can zoom into the image right from the editing view.

// img one item editing scrot

You can reveal the file on disk using a right click menu either from the main window or from one item editing view.

// img right click menu

As for the editing of artworks, the spellchecking feature has been enchanced. Now you can add different words to user dictionary so the spellchecking will not mark them as wrong. Also you can manage this list if you feel like removing some accidentally added words via [Tools -> Advanced -> Manager user dictionary]

// img right click menu 

One of the key features in this release are the keywords presets. As a user you can create your own predefined sets of keywords which you can easily insert into any keyword editing field. Just go to [Edit -> Presets] and create some presets.

// img presets

To use the presets you've just created you have several options. First of all, you can right click any keywords editing field and use "Insert presets" menu.

// insert presets submenu

Also if you have created, let's say, presets with the names "old woman" and "young woman" and you have a "woman" keyword, you are able to right click it and select a preset with "woman" in it:

// expand as preset scrot

And the last but not the least way to do it is to use autocompletion feature and while typing a keyword to insert a preset with Ctrl+Enter if you see a small "p" letter right next to the keyword:

// autocomplete "p" scrot

Another big feature provided in this release is the keywords suggestion enchancements. You can search either for "All images" or for "Photos", or "Vector" or "Illustrations" now.