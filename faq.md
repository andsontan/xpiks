---
layout: default
title: FAQ
permalink: /faq/
---

<article class="row">
<section class="small-12 large-8 columns page-content">

<div>
  <p>
    FAQ section contains solutions to the most typical Xpiks user problems so far.
  </p>
</div>

<div>
  <ul>
    <li><strong>I downloaded <i>Xpiks-qt-v1.1.4.zip</i> and what the hell I have to do next?</strong></li>
    <li>Xpiks will have an installer for Windows soon. Meanwhile extract zip archive to any place you want to (e.g. <code class="highlighter-rouge">C:\</code>), go inside and double-click the file <strong>xpiks-qt.exe</strong></li>
  </ul>
</div>

<div>
  <ul>
    <li><strong>"Xpiks" can't be opened because it is from an unidentified developer</strong></li>
    <li>Cmd-click the Xpiks.app and select Open from context menu. Read more at the <a href="https://support.apple.com/kb/PH18657" target="_blank">decicated official Apple webpage</a>.</li>
  </ul>
</div>

<div>
  <ul>
    <li><strong>"Import/Export finished with errors. See logs for details."</strong></li>
    <li>Usually this error appears if Xpiks fails to find Exiftool. If you didn't install it, please visit <a href="http://www.sno.phy.queensu.ca/~phil/exiftool/" target="_blank">Official Exiftool Website</a> and install it. If you have Exiftool installed and still you see this error, then please go in Xpiks to Settings -> External and select correct path to `exiftool` executable (for example, if it's <code class="highlighter-rouge">/usr/bin/exiftool</code> try to change it to <code class="highlighter-rouge">/usr/local/bin/exiftool</code> and vice versa)</li>
  </ul>
</div>

<div>
  <ul>
    <li><strong>Replace misspelled keywords does nothing</strong></li>
    <li>If you happen to have two keywords in your list, say, <i>"colour"</i> and <i>"color"</i> and then you hit "Fix spelling", select <i>"colour"</i> -> <i>"colour"</i> and hit "Replace". When Xpiks tries to replace <i>"colour"</i> to <i>"colour"</i> (as you chose) it sees that you already have <i>"color"</i> and replacement will create two <i>"colour"</i> keywords. There is an ambiguity in this situation since it's not clear if you were to remove the misspelled word or the original duplicate of it so Xpiks leaves everything as is.</li>
  </ul>
</div>

<div>
  <ul>
    <li><strong>Xpiks closes shortly after start in Windows</strong></li>
    <li>It's possible that your antivirus (Avast?) detected Xpiks as malware and quarantined it. Xpiks is not a malware and it's source code is <a href="https://github.com/ribtoks/xpiks" target="_blank">available at GitHub</a>. You can check either by yourself or by third-parties that Xpiks does not contain viruses.</li>
  </ul>
</div>

<div>
  <ul>
    <li><strong>Text looks ugly small at my beautiful Surface Pro 3 with Windows 8.1</strong></li>
    <li>1. Press "windows" button, search for "environmental variables", select "Edit system environmental variables". 2. In the upper section "Add new variable". For a variable name use <code class="highlighter-rouge">QT_DEVICE_PIXEL_RATIO</code> and for it's value use "2" (without quotes as well). Press "Ok", "Apply" or what ever you need to save and quit. 3. Open Xpiks</li>
  </ul>
</div>

<div>
  <ul>
    <li><strong>I can't upload via FTP to my favorite stock XYZ</strong></li>
    <li>First of all, check if you didn't make a typo in a Host, Username and Password field. Use "Show Password" checkbox to see if you typed it correctly. Try to use "Test connection" button before upload and see if it works. Try to login with and without Passive Mode (in the Advanced tab). Read help in your stock XYZ about how to upload artworks.</li>
  </ul>
</div>

</section>
</article>
