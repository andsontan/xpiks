---
layout: post
title: "Security of uploading artworks to microstocks"
date: 2016-01-13T22:26:12+02:00
---

<img alt="FTP security?" src="{{site.url}}/images/globe2.jpg" class="small-12 large-8" />

When you use software to upload files to microstocks FTP servers you must enter your username and password into software you use. You can easily end up with a question if you can trust this software. It's even more naturally to ask this kindof questions if this software is not open source. You don't know what's going on inside with your valuable data.

But even if you trust your software, there's one huge security hole behind everything. And this hole is ... FTP.
FTP (File Transfer Protocol) is not secure protocol by definition. It sends your credentials in plain text over the internet to the remote server. Don't belive me? You can install <a href="https://www.wireshark.org/">Wireshark</a> and investigate yourself. Or you can read a number of blogposts about this. <a href="http://engineering.deccanhosts.com/2013/02/why-is-ftp-insecure.html">Why is FTP insecure</a>, <a href="http://fixmyitsystem.com/2010/09/grab-ftp-username-and-password-using.html">Grab FTP username and password using Wireshark</a> or you even can see a <a href="https://www.youtube.com/watch?v=qdo6XtFrEvo">video of how to do it</a> on the Youtube.

There are several ways to mitigate it.
One of the most simple one is that all microstocks will switch to <a href="https://en.wikipedia.org/wiki/SSH_File_Transfer_Protocol">SFTP</a> which is Secure File Transfer Protocol (basically it's encrypted FTP). Then your credentials will be transfered over the network in secure manner.
Another options is do what Fotolia does: they supply each contributor with <strong>separate</strong> username and password for FTP upload so even if someone will intercept your credentials everything he would be able to do is ... to upload some pictures for you?

Talking about Xpiks, it always uses <a href="https://en.wikipedia.org/wiki/Advanced_Encryption_Standard">AES encryption</a> when storing user's FTP passwords. Also you can set your own passphrase for encryption. So it is almost next to possible for other programs to retrieve them. But remember: because of the FTP it won't save you.
