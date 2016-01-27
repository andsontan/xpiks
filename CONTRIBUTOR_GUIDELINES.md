Project guidelines:

- Before you push your commits ensure that project builds without errors and warnings and all tests are passing

Coding guidelines:

- curly bracket opens on the same line as previous statement
- always put a class into a namespace
- class fields variables should be private except special cases when they can be protected
- class fields should be named using the `m_CamelCase` style (starting with "m_" and then the name starting with the capital letter)
- try to move as much includes from the header to implementation as possible

Take a look at the [Xpiks Wiki pages](https://github.com/Ribtoks/xpiks/wiki). They contain a lot of topics of how to develop this or that feature.

How to contribute:
- [Fork](http://help.github.com/forking/) Xpiks repository on GitHub
- Clone your fork locally
- Configure the upstream repo (`git remote add upstream git@github.com:Ribtoks/xpiks.git`)
- Create local branch (`git checkout -b your_feature`)
- Work on your feature
- Build project using BUILD.md instructions
- Build and Run tests (`xpiks-tests.pro` project), fix your feature if needed
- Push the branch to GitHub (`git push origin your_feature`)
- Send a [pull request](https://help.github.com/articles/using-pull-requests) on GitHub
