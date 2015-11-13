---
layout: post
title: "What are the xpks files?"
date: 2015-11-14T00:01:01+02:00
---

You may have noticed that after you edit your images in Xpiks, it creates files with extension "xpks" in the directory with images, which are named exactly like the image files.

The point is to make your editing process bullet-proof safe. Xpks files are automatic backup of what you are writing to the Description/Title/Keywords fields. The backup is updated once you're editing the artwork and the metadata gets written to the image itself only when you press "Save".

In case of any emergency (blackout, application crash, OS crash, misclick, exiftool path error) you will have latest backup always with you saved in the xpks file. If you want to skip reading metadata from backup on import you can check appropriate checkbox in the Import dialog window ("Skip backups").

So even if you don't have the exiftool installed (e.g. network unreachable) you can fill metadata of your files which will be backed up in the xpks files. After you will get the exiftool you can resave all your files to get metadata written to files.

If you don't like to have those backups you can easily turn them off in Settings in the Behavior tab, but my recomendation is to always have them available. They saved my butt much more than once.