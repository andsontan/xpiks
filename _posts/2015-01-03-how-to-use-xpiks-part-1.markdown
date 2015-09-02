---
layout: post
title: "How to use Xpiks. Part 1: adding and keywording artworks"
date: 2015-01-03T22:17:01+02:00
---

This serie of posts will describe typical use case of Xpiks. Xpiks was created for microstockers by microstocker. It should help you to minimize time you spend keywording and uploading images or illustrations to stock agencies.

You will learn how:

- add artworks to flow
- keyword artworks
- use keywods suggestion
- <span class="gray">save XMP/IPTC metadata</span>
- <span class="gray">upload artworks to microstocks</span>

In this post you will learn how to add images for keywording or upload and to add keywords and other metadata to existing images.

First of all, open Xpiks. You'll see main window with no added images:

<img alt="Main window" src="{{site.url}}/images/howto/xpiks-qt-justopened.jpg" class="small-12 large-8" />

Then you can add files via "Add files" or "Add directory" button. Both can add only JPG files. If you are an illustrator, add previews instead of vectors. Let's add some files. When you select files or directories and open them, an import dialog launches. It helps to import existing IPTC metadata into Xpiks. Also you can drag'n'drop the directories and filed directly onto Xpiks.

<img alt="Adding images" src="{{site.url}}/images/howto/xpiks-qt-startimport.jpg" class="small-12 large-8 " />

Press "Start import" to actually start importing metadata or just "Close" to skip this step. <span class="gray">When Xpiks is working, it creates temporary backups of your keywords (even if you didn't saved them yet) so you can restore them in case of you closed Xpiks without saving your work. When opening the artworks again you have option to ignore that backups.</span>

In current version of Xpiks you won't be able to import metadata later. If import finishes with success, progress bar will remain blue and in case of errors it becomes red.

Now you can proceed to editing metadata.

<img alt="Import finished" src="{{site.url}}/images/howto/xpiks-qt-imported.jpg" class="small-12 large-8 " />

If you have images which would have many similar keywords, you can process simultaneously (just like all popular stocks allow). To do that, just select similar images and press "Edit" button in top right control box.

The following popup will appear:

<img alt="Combined edit" src="{{site.url}}/images/howto/xpiks-qt-combinededit.jpg" class="small-12 large-8" />

Now you can open keywords suggestion service in Shutterstock, in Yuri Arcurs blog or any other you prefer, get some keywords for your images and type them in appropriate fields.

Also you can use keywords suggestion in Xpiks which works using the Shutterstock's images database. To use it, just press the "Suggest keywords" button. <span class="gray">You can use keywords suggestion for separate item also on the main window below the keywords.</span>

<img alt="Keywords suggestion" src="{{site.url}}/images/howto/xpiks-qt-keywords-suggestion.jpg" class="small-12 large-8" />

After pressing the "Add suggested keywords" button only keywords in the area marked "Suggested keywords" will be added to the selected image(s). Keywords in the area "Othe keywords" won't be added.

Now you have choise to append written keywords to images you edit or to replace keywords with those you entered. Press appropriate button and you'll see keywords in main window. When an item is modified in any way, you'll see yellow sign near it until you'll save it. Also, you can track modified artworks count in status bar in the bottom line of Xpiks.

<img alt="Applied changes in images" src="{{site.url}}/images/howto/xpiks-qt-combinededit-applied.jpg" class="small-12 large-8" />

Now you can fine-tune images edited in combined mode and to add some specific keywords for each image.

Now this part is over. You can read about how to save metadata and upload artworks to microstock agencies in <a href="{{site.url}}/blog/2015/how-to-use-xpiks-part-2">next post</a>.