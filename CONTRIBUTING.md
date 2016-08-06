Project guidelines:

- After you create a pull request ensure continuous integration bindings are green (build is compilable and tests are passing across all platforms)

Coding guidelines:

- curly bracket opens on the same line as previous statement
- always put a class into a namespace
- asterisk or ampersand should be aligned with variable/method name
- class fields should go last
- class fields variables should be private except very special cases when they can be protected
- class fields should be named using the `m_CamelCase` style (starting with "m_" and then the name starting with the capital letter)
- use STL containers for custom classes and Qt containers for Qt classes
- try to move as much includes from the header to implementation as possible
- methods should be separated by 1 empty line as well as all files should have 1 last empty line
- always put space around assignments, after comma, before curly bracket etc.
- in the cpp file first include should be appropriate header, then system Qt headers, then system standard C++ headers and only then local headers followed by third-party libs headers
- you can use Uncrustify withe the config in `src/xpiks-qt/` directory for code formatting to ensure you comply with project rules
- log using INFO when you have useful parameters and using DEBUG when just marking execution place

Xpiks architecture:

- take a look at the [Xpiks Wiki pages](https://github.com/Ribtoks/xpiks/wiki). They contain a lot of topics of how to develop this or that feature as well as architecture overview.

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
