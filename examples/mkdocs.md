# MkDocs demo

## Admonitions

!!! note
    No title given: the name of the type is the title.

!!! tip "A title of your own"
    The body is every line indented by four spaces.

    It can hold several paragraphs, lists and `code`.

??? question "Folded by default"
    The answer shows on a click.

???+ warning "Open, can be folded"
    A warning.

!!! danger ""
    An empty title, `""`, gives a box without a title.

## Math extras

A macro defined once serves every formula after it:

$\newcommand{\vect}[1]{\overrightarrow{#1}}$

$\vect{AB} + \vect{BC} = \vect{AC}$

LaTeX's own delimiters work too: \(a^2 + b^2 = c^2\), and

\[
\sum_{k=1}^{n} k = \frac{n(n+1)}{2}
\]

An equation with a number on the line of its closing delimiter:

$$ e^{i\pi} + 1 = 0 $$ (1)
