---
layout: post
title: "How to use Xpiks. Part 2: saving and uploading artworks"
date: 2015-01-03T23:29:01+02:00
---

This serie of posts will describe typical use case of Xpiks. Xpiks was created for microstockers by microstocker. It should help you to minimize time you spend keywording and uploading images or illustrations to stock agencies.

In this article you will learn how:

- <span class="gray">add artworks to flow</span>
- <span class="gray">keyword artworks</span>
- <span class="gray">use keywods suggestion</span>
- save XMP/IPTC metadata
- check for warnings before upload
- upload artworks to microstocks

In this post you will learn how to save edited artworks and to upload them to microstocks.

As you can recall from <a href="{{site.url}}/blog/2015/how-to-use-xpiks-part-1">previous post</a>, we added and keyworded a bunch of images. It's time to upload images and earn some money. If you'll try to select all of them and press "Upload" button, you'll gen a warning, that there're unsaved images in items you're going to upload.

So let's save them. In order to do that, just select images you want to save and press "Save" button. An export dialog will pop up. You have an option to save a backup for each artwork (keywords are being written to same file you've opened).

<img alt="Saving images" src="{{site.url}}/images/howto/xpiks-qt-saving-images.jpg" class="small-12 large-8" />

After export finished, press "Close" <span class="gray">if the dialog didn't close itself</span> and proceed to upload. Select artworks you wish to upload and press "Upload" button.

<img alt="Saving images" src="{{site.url}}/images/howto/xpiks-qt-upload-warnings.jpg" class="small-12 large-8" />

If you have any errors, you will see orange "Warnings" link where you can check what is wrong with the images. You can see warnings related to dimensions, description and keywords error (empty, too long etc). You can configure warnings limits in the "Warnings" section of Xpiks settings.

<img alt="Warnings check" src="{{site.url}}/images/screenshots/WarningsDialog.jpg" class="small-12 large-8" />

Most microstocks allow FTP uploading of your works. To proceed with upload, add some hosts of agencies. Usually you can find FTP credentials in your stock's upload section. Just look for FTP host, username and password and enter them in appropriate fields in Xpiks hosts edit dialog. All credentials are automatically saved for you. Passwords are encrypted using the strong encryption. <span class="gray">At first they are encrypted using some default password. You can choose your own password (Maste password) in Xpiks settings, section "Security".</span>

When adding a FTP account you can check it's validity using the "Test connection" button. The circle will get green if you entered correct data.

<img alt="Adding upload hosts" src="{{site.url}}/images/howto/xpiks-qt-upload-hosts.jpg" class="small-12 large-8" />

You can add as many hosts as you wish and before each upload you can select stocks where you wish to upload this time. When everything is ready, just press "Start Upload" and wait until "Close" button becomes available again. When uploading, you have an option to upload EPS or AI files along with JPG files (it's useful for illustrators, which have to keyword jpg's but upload both). Also you have option to zip preview and vector file before actual upload (some stocks require you to upload zips) - just check the checkbox. <span class="gray">If talking about Shutterstock, you have to uncheck the Automatic FTP processing option in order to catch the keywords from the previews correctly.</span>

<img alt="Uploading items" src="{{site.url}}/images/screenshots/UploadInProgress.jpg" class="small-12 large-8" />

After upload finished, you can go and check stocks. In this example I've uploaded photos to Shutterstock and Depositphotos. Depositphotos processed FTP items almost immediately while Shutterstock had some delay (30 minutes) so be patient. If any error happens, you'll be notified about this by color of progress bar.

You can note, that no stock has selected categories. That's because all they use different categories and you still have to enter them manually.

<span class="gray">Photos below differ, because they correspond to previous version of this tutorial.</span>

<img alt="Depositphotos" src="{{site.url}}/images/howto/depositphotos-success.jpg" class="small-12 large-8" />

<img alt="Shuterstock" src="{{site.url}}/images/howto/shutterstock-success.jpg" class="small-12 large-8" />
