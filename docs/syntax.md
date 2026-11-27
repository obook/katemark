# Supported syntax

| Family | Syntax | Also understood by |
|---|---|---|
| GitHub | Tables, task lists (`- [ ]`), alerts (`> [!NOTE]`), `~~strikethrough~~`, bare links, highlighted code blocks, YAML front matter | Alerts: Obsidian, [VitePress](https://vitepress.dev/guide/markdown), GitLab |
| [markdown-it](https://github.com/markdown-it/markdown-it) plugins | `==mark==` ([mark](https://github.com/markdown-it/markdown-it-mark)), `++ins++` ([ins](https://github.com/markdown-it/markdown-it-ins)), `H~2~O` ([sub](https://github.com/markdown-it/markdown-it-sub)), `19^th^` ([sup](https://github.com/markdown-it/markdown-it-sup)), footnotes ([footnote](https://github.com/markdown-it/markdown-it-footnote)), emoji by name such as `:warning:` ([emoji](https://github.com/markdown-it/markdown-it-emoji)) | |
| [CodiMD](https://github.com/hackmdio/codimd) | `:::success`, `:::info`, `:::warning` and `:::danger` alert areas, also under the Docusaurus names `note`, `tip` and `caution`; `:::spoiler`; `[TOC]` | Alert areas: HackMD, [Docusaurus](https://docusaurus.io/docs/markdown-features/admonitions) (`:::note[Title]`), [VitePress](https://vitepress.dev/guide/markdown) (`::: info`) |
| Obsidian | Callouts (`> [!faq]- Title`) with every type and alias, custom titles and folding; wiki links (`[[Note]]`, `[[Note\|label]]`, `[[Note#Heading]]`); image embeds with a size (`![[image.png\|300]]`); `%%comments%%`; block ids (`^id`) and links to them (`[[Note#^id]]`); highlight colors (`==🔴text==`) | |
| MkDocs | `!!! note "Title"` admonitions with an indented body, `??? note` for a folded one, `???+ note` for one that starts open | Python-Markdown's `admonition` extension, Material for MkDocs; `???` comes from `pymdownx.details` |
| Teaching | Callouts `definition`, `theorem`, `lemma`, `proposition`, `corollary`, `proof`, `exercise`, `solution`, `remark`... and their short names (`def`, `thm`, `pf`...), plus `method`; also under their French names, with or without accents (`théorème`, `propriété`, `démonstration`, `correction`...); a picture's size, `![](image.png){width=50%}` | Callouts: the Obsidian plugin [Academic Notes](https://github.com/Kristal-Boltinn/academic-notes/wiki/Mathematical-environments); sizes: Pandoc |
| Math | `$...$`, `$$...$$`, `\(...\)` and `\[...\]`, rendered by [KaTeX](https://github.com/KaTeX/KaTeX) | |
| Diagrams | [Mermaid](https://github.com/mermaid-js/mermaid) in ` ```mermaid ` blocks, and Mermaid files (`.mmd`, `.mermaid`) | |

- Obsidian links resolve against the folder of the document, not across a
  vault.
- With **GitHub only** checked, the preview reads the GitHub row alone, plus
  what GitHub renders too: footnotes, emoji, math and Mermaid. It also
  strikes `~text~` between single tildes, as GitHub does. The result is
  close to GitHub's without being identical: the code highlighter and the
  math engine differ.
- [markdown-it-container](https://github.com/markdown-it/markdown-it-container)
  parses the `:::` blocks.
- Math accepts what MathJax does where KaTeX is stricter: `\require{...}`,
  inline math over several lines, `\newcommand` on a command that exists.
- A macro defined in a formula serves the formulas after it, and `$$...$$
  (1)` numbers an equation.
