#!/bin/bash

# replace url for local development
sed -i.bak "s/^url: \/xpiks/url: ''/" _config.yml

bundle exec jekyll serve --baseurl ''

# revert url for GitHub
sed -i.bak "s/^url: ''/url: \/xpiks/" _config.yml
