# Third-party notices

This plugin bundles the following libraries in `data/`. Their licenses are reproduced below.

| Library | Version | License | URL |
|---|---|---|---|
| markdown-it | 14.3.3 | MIT | https://github.com/markdown-it/markdown-it |
| markdown-it-container | 4.0.0 | MIT | https://github.com/markdown-it/markdown-it-container |
| markdown-it-mark | 4.0.0 | MIT | https://github.com/markdown-it/markdown-it-mark |
| markdown-it-ins | 4.0.0 | MIT | https://github.com/markdown-it/markdown-it-ins |
| markdown-it-sub | 2.0.0 | MIT | https://github.com/markdown-it/markdown-it-sub |
| markdown-it-sup | 2.0.0 | MIT | https://github.com/markdown-it/markdown-it-sup |
| markdown-it-footnote | 4.0.0 | MIT | https://github.com/markdown-it/markdown-it-footnote |
| markdown-it-emoji | 3.1.0 | MIT | https://github.com/markdown-it/markdown-it-emoji |
| markdown-it-texmath | 1.0.0 | MIT | https://github.com/goessner/markdown-it-texmath |
| KaTeX | 0.19.1 | MIT (fonts: SIL OFL 1.1) | https://github.com/KaTeX/KaTeX |
| Mermaid | 12.1.0 | MIT | https://github.com/mermaid-js/mermaid |
| highlight.js | 11.12.0 | BSD-3-Clause | https://github.com/highlightjs/highlight.js |
| js-yaml | 4.1.0 | MIT | https://github.com/nodeca/js-yaml |
| github-markdown-css, by Sindre Sorhus | | MIT | https://github.com/sindresorhus/github-markdown-css |

This fork reimplements the Obsidian syntax and three parts of CodiMD: its
alert areas, its spoiler and its table of contents. No code comes from
either project. The other text extensions come from the markdown-it plugins
listed above. The colors of the CodiMD alert areas are those of [Bootstrap
3](https://github.com/twbs/bootstrap) alerts (MIT).

## markdown-it

`data/js/markdown-it.min.js`, MIT License.

```
Copyright (c) 2014 Vitaly Puzrin, Alex Kocharin.

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
```

## js-yaml

`data/js/js-yaml.min.js`, MIT License.

```
Copyright (C) 2011-2015 by Vitaly Puzrin.

Permission is hereby granted, free of charge, to any person
obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without
restriction, including without limitation the rights to use,
copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following
conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
```

## highlight.js

`data/js/highlight.min.js`, `data/css/hljs-github.min.css`, `data/css/hljs-github-dark.min.css`, BSD 3-Clause License.

```
BSD 3-Clause License

Copyright (c) 2006, Ivan Sagalaev.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
```

## github-markdown-css

`data/css/github-markdown.css` (color variable definitions removed; layout unchanged), MIT License.

```
MIT License

Copyright (c) Sindre Sorhus <sindresorhus@gmail.com> (https://sindresorhus.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
```

## markdown-it plugins

`data/js/markdown-it-container.min.js`, `markdown-it-mark.min.js`, `markdown-it-ins.min.js`,
`markdown-it-sub.min.js`, `markdown-it-sup.min.js`, `markdown-it-footnote.min.js`,
`markdown-it-emoji.min.js`.
MIT License, same text as markdown-it above.

```
Copyright (c) 2014-2015 Vitaly Puzrin, Alex Kocharin.
```

## markdown-it-texmath

`data/js/texmath.js`, MIT License, same text as markdown-it above.

```
Copyright (c) 2013-17 Stefan Goessner
```

## KaTeX

`data/js/katex.min.js` and `data/css/katex.min.css`, MIT License, same text as markdown-it
above. The stylesheet embeds the KaTeX fonts (SIL Open Font License 1.1) as data: URIs; it is
generated by `packaging/inline_katex_fonts.py`.

```
Copyright (c) 2013-2020 Khan Academy and other contributors
```

## Mermaid

`data/js/mermaid.min.js`, MIT License, same text as markdown-it above.

```
Copyright (c) 2014 - 2022 Knut Sveidqvist
```
