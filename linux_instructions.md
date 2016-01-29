---
layout: default
title: linux
permalink: /linux-instructions/
---

<article class="row">
<section class="small-12 large-8 columns page-content">

<p>Install the following packages:</p>
<ul>
  <li>hunspell</li>
  <li>hunspell-en-us</li>
</ul>

<p><strong>Ubuntu</strong></p>

<p>You have to manually install <i>qtdeclarative5-controls-plugin</i> and <i>qtdeclarative5-quicklayouts-plugin</i>. Also there is an issue with application menu in Qt for Ubuntu which requires a workaround:</p>

<p><code>sudo apt-get remove --purge appmenu-qt5</code></p>

<p><strong>Mint 17.3 (Rose)</strong></p>

<p>List of QML plugins to be installed manually (from standard repository):</p>

<ul>
  <li>qtdeclarative5-dialogs-plugin</li>
  <li>qtdeclarative5-privatewidgets-plugin</li>
  <li>qtdeclarative5-qtquick2-plugin</li>
  <li>qtdeclarative5-quicklayouts-plugin</li>
  <li>qtdeclarative5-window-plugin</li>
  <li>qtdeclarative5-controls-plugin</li>
</ul>

<p><strong>OpenSuse installation</strong></p>

<p>Additional package have to be installed manually (from standard repository):</p>

<ul>
  <li>libqt5-qtquickcontrols</li>
</ul>
</section>
</article>
