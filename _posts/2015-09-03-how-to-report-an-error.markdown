---
layout: post
title: "How to report a bug"
date: 2015-09-03T00:45:01+02:00
---

In case you had bad user experience with Xpiks (crash, failure, unexpected behavior) you can report it and it will be fixed as soon as possible taking into account it's severity.

To report an error, first of all write down what have you done before the error. 

Please, **enumerate as many steps you did as you can** (ideally all of them from the application start and to the failure).

Afterwards, **try to find Xpiks logs**:

As of Xpiks 1.1, you can easily find them in Logs View (leftmost link in the bottom of main window) with a magical button **"Reveal logfile"**. If you're on earlier build, you can find them in one of the following:

- on Windows they should be at `C:/Users/USERNAME/AppData/Roaming/Xpiks/` directory
- on OS X they should be at `~/Library/Application Support/Xpiks/` directory
- on Linux they should be at `~/.local/share/xpiks/` directory

Please, paste the contents of the logfile to <a href="http://pastebin.com/">PasteBin</a>.

Now you can navigate to the <a href="{{ site.url }}/support">Support</a> part of this website (Google group) and to create a new topic, where you are kindly asked to include:

- short description of your system (name, version, architecture)
- description of what you done before the crash (see above)
- Xpiks logs as a file
- anything you would be find useful (e.g. screenshots)

Also you are welcome start a new topic if you'd like to propose an improvement or have any useful ideas on Xpiks development and mission.

In case you're a developer and you have technical information about the issue, you're welcome to create a ticket at <a href="https://github.com/Ribtoks/xpiks/issues">Github page</a>.
