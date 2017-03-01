---
layout: post
title: "Xpiks 1.4 detailed overview"
date: 2017-02-24T15:15:52+01:00
---

Xpiks 1.4 release development took as much time as usual 3 releases combined so it's worth spending some time diving into the features it brings to the users.

# **General**

First easily notable thing is the user interface change. Not only colors are more contrast in both themes, text is better visible, but also the whole layout is different now. You can play with the panel on the left which is available in all editing modes and shares an additional buffer for metadata and translator. You can collapse it if it bothers you or expand it back if you feel like using it's features. The UI strives to be more adaptive and useful now on.

<img alt="Main window" src="{{site.url}}/images/posts/xpiks-14-demo/main-interface.gif" class="small-12 large-8" />

Also one file and multiple editing UI has changed significantly. For one file editing you can switch between available artworks on the bottom without going back and forth to main view. Also now you can zoom into the image right from the editing view.

<img alt="One item editing" src="{{site.url}}/images/posts/xpiks-14-demo/one-item-editing.gif" class="small-12 large-8" />

<br />

# **Tabs**

### **Quick Buffer**

Panel on the right contains two more new featues: Quick Buffer and Translator. Quick Buffer is an additional buffer of metadata (title, description and keywords) which can carry over all editing modes and fill from currently edited artwork or flush the data to the artwork with "Apply" button.

<img alt="Quick Buffer" src="{{site.url}}/images/posts/xpiks-14-demo/quick-buffer-basic.gif" class="small-12 large-8" />


### **Translator**

Xpiks 1.4 comes with StarDict dictionaries support built in and available through the Translator tab. Because of the legal reasons Xpiks doesn't come with any dictionaries preinstalled, but you can easily find dictionaries online or use your own purchased dictionaries. Just press "Add dictionary" in the dropdown. Afterwards to translate text just enter the word into the active input. Translation will appear immediately. In case the translation doesn't fit into the available space, you can always press "Show more" in the bottom and look through the full translation in the additional popup.

<img alt="Translator" src="{{site.url}}/images/posts/xpiks-14-demo/translator-basic.gif" class="small-12 large-8" />

<br />

# **Context menus**

Images everywhere contain useful actions in the context menu. For example, you can reveal the file on disk using a right click menu either from the main window or from one item editing view.

<img alt="Show in folder" src="{{site.url}}/images/posts/xpiks-14-demo/xpiks-qt-show-in-folder.jpg" class="small-8 large-6" />

In the Keywords Suggestion window now you are able to open image from the results in the browser though the contex menu. This will allow you e.g. check other metadata of the selected image like Description or Title.

<img alt="Open in browser" src="{{site.url}}/images/posts/xpiks-14-demo/SuggestionContextMenu.jpg" class="small-8 large-6" />

<br />

# **User dictionary**

As for the editing of artworks, the spellchecking feature has been enchanced. Now you can add different words to user dictionary so the spellchecking will not mark them as wrong. Also you can manage this list if you feel like removing some accidentally added words via [Tools -> Advanced -> Manager user dictionary]

<img alt="Add to dictionary" src="{{site.url}}/images/posts/xpiks-14-demo/xpiks-qt-add-to-dictionary.jpg" class="small-8 large-6" />

<br />

# **Presets**

One of the key features in this release are the keywords presets. As a user you can create your own predefined sets of keywords which you can easily insert into any keyword editing field. Just go to [Edit -> Presets] and create some presets.

<img alt="Presets" src="{{site.url}}/images/posts/xpiks-14-demo/xpiks-qt-presets.jpg" class="small-8 large-6" />

To use the presets you've just created you have several options. First of all, you can right click any keywords editing field and use "Insert presets" menu.

<img alt="Insert Preset" src="{{site.url}}/images/posts/xpiks-14-demo/xpiks-qt-insert-preset.jpg" class="small-8 large-6" />

Also if you have created, let's say, presets with the names "old woman" and "young woman" and you have a "woman" keyword, you are able to right click it and select a preset with "woman" in it:

<img alt="Expand Preset" src="{{site.url}}/images/posts/xpiks-14-demo/xpiks-qt-expand-preset.jpg" class="small-8 large-6" />

And the last but not the least way to do it is to use autocompletion feature and while typing a keyword to insert a preset with Ctrl+Enter if you see a small "p" letter right next to the keyword:

<img alt="Autocomplete Preset" src="{{site.url}}/images/posts/xpiks-14-demo/xpiks-qt-autocomplete-preset.jpg" class="small-8 large-6" />

<br />

# **Keywords Suggestion Enchancements**

Another big feature provided in this release is the keywords suggestion enchancements. You can search either for "All images" or for "Photos", or "Vector" or "Illustrations" now. This can filter out the results which you don't need.

<img alt="Suggestion options" src="{{site.url}}/images/posts/xpiks-14-demo/xpiks-qt-suggestion-filter.jpg" class="small-8 large-6" />

<br />

# **Find and Replace**

Starting with Xpiks 1.4 a new powerfull feature is being presented: Find and Replace tool. You can not only do sophisticated search, but also to remove keywords with it (though a separate tool for convenient removing keywords exists). To launch Find and Replace tool just hit "Ctrl+Shift+F" (or "Cmd+Shift+F") or go to [Edit -> Find and Replace].

<img alt="Find and Replace" src="{{site.url}}/images/posts/xpiks-14-demo/FindAndReplace-Search.jpg" class="small-8 large-6" />

When you're satisfied with the search, hit "Find and Replace" button and you will see the preview of whan actually will be replaced.

<img alt="Find and Replace Preview" src="{{site.url}}/images/posts/xpiks-14-demo/FindAndReplace-Preview.jpg" class="small-12 large-8" />

<br />

# **Batch fix spelling**

If you made same spelling error multiple times one of the options for you is to use "Fix spelling" tool but applied to all artworks. With Xpiks 1.4 this became possible:

<img alt="Fix spelling in selected" src="{{site.url}}/images/posts/xpiks-14-demo/fix-spelling-demo.gif" class="small-12 large-8" />

<br />

# **Batch remove keywords**

Find and Replace can definitely be used to remove keywords from artwords (when replacing those to nothing) but what comes really handy in this case is the new Delete Keywords tool. It allows you to choose conveniently kewords you want to remove.

<img alt="Delete keywords from selected" src="{{site.url}}/images/posts/xpiks-14-demo/delete-keywords-demo.gif" class="small-12 large-8" />

<br />

Enjoy!

<div class="download-link"><a href="{{ site.url }}/downloads">Download</a></div>