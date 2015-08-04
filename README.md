xpiks
=====

Cross-platform (X) Photo Keywording Software

Official website: http://ribtoks.github.io/xpiks/

Xpiks is a free and open source keywording and uploading tool for microstock photographers and illustrators with no hidden fees. If you're tired with copy and pasting keywords from one stock to another, Xpiks would be a great time saver for you!

**Main features:**

- ability to keyword images as well as set additional XMP/IPTC metadata (title, description, author)
- uploading images to Stock Agencies' FTP hosts (saves credentials for each host)
- keywords suggestion based on Shutterstock API
- ability to upload vector eps files with images with same name (useful for illustrators)
- checks for potential problems before upload (insufficient resolution, missing metadata etc.)
- unsaved work is backed up so you can continue work later without actual writing metadata to images
- Ability to Drag'n'Drop images to workflow
- works under Linux, OS X and Windows
- ExifTool and Curl under the hood
- written in C++ with Qt 5.4

**How to build:**
- git clone Xpiks repo
- install Qt 5.4 from official website
- git submodule init & git submodule update - update tinyAES repo
- rename tinyAES/aes.c to tinyAES/aes.cpp
- open main project in Qt Creator, select Release, Run qmake, Build
- you can open, build and run tests project additionally 

More instructions can be found at this page http://ribtoks.github.io/xpiks/blog/2014/building-xpiks-alone/

**How to contribute:**

- open Issues page and select an issue (i'll provide you with all details and explanations as needed)
- send your ideas and proposals in comments, issues or via email
- send your pull requests

C++ knowledge and experience in programming is required. Qt knowledge would be a plus.
