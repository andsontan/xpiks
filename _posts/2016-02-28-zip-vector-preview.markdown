---
layout: post
title: "How to zip vectors and previews"
date: 2016-02-28T21:31:48+02:00
---

<img alt="Generating archives" src="{{site.url}}/images/posts/GenerateArchives.jpg" class="small-12 large-8" />

Artists who draw illustrations and sell them at several microstocks sometimes need to create archives with vectors and previews. For example, Fotolia, Depositphotos and CanStockPhoto require vector and preview to be in .zip archive.

When Xpiks was only in deep alpha I was asked to help such artists to create .zip archives automatically. After a while I create a shell script for OS X and Linux <a href="https://raw.githubusercontent.com/Ribtoks/heap/master/stock_archiver/generate_archives.sh" target="_blank">generate_archives.sh</a>. You can put it to any directory, make it executable and in the terminal you can execute it like this <code class="highlighter-rouge">./generate_archives.sh /path/to/my/directory/with/vectors</code>. This code will process all files in the directory passed as first parameter and create a directory with archives.

Of course, nowadays there's no need to run scrips. You can just download and open cross-platform Xpiks application, add files to the workflow and select Tools -> Zip selected artworks. Of course, even this is not needed, becasue if you upload files with Xpiks, you can just mark "Zip vector and preview" option on particular stock when uploading. Xpiks will create archives and upload them automatically.