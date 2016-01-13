---
layout: post
title: "In the pursuit of sharpness: downscaling images (tip: don't)"
date: 2016-01-14T00:10:38+02:00
---

<img alt="Pursuit of sharpness" src="{{site.url}}/images/sharpness_struggle.jpg" class="small-12 large-8" />

There's a common misconception when you're trying to archieve a perfect sharp image: you downscale the original. In some cases it can help you, but there're <strong>a lot</strong> of situations when it will make your image look less sharp.

You can notice in the header image how the downscaled image on the right is less sharp in the black lines than it's original. The problem is that there're no perfect algorithms of downscaling images. Most of them will struggle with something. Text, Moire effect, edges, textures. You will have to pick up different scaling algorithms depending on a situation. Sometimes it would be Cubic, sometimes one of the sinc family.

There's very interesting <a href="http://www.cambridgeincolour.com/tutorials/image-resize-for-web.htm" target="_blanc">article at Cambridge in Color about downscaling</a>.

So my advice to you would be to examine your images very well when you downscale them. Maybe the original was way more sharper.