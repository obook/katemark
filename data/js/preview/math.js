// math.js
// Makes KaTeX accept the formulas of documents written for MathJax (CodiMD), and keeps
// the macros defined along the way.
//
// Part of the preview page: adds its functions to window.katemark (see core.js).
// Author: Olivier Booklage
// Date: October 2026
// License: GPL-3.0-or-later
(function () {
  "use strict";

  var katemark = window.katemark;

  // Macros defined so far in the document being rendered (\newcommand, \def...). With
  // globalGroup, KaTeX writes each definition into this object, so it serves the
  // formulas that follow, as with MathJax.
  var macros = {};
  // \newcommand lines from the plugin's settings, available in every document.
  var globalMacros = "";
  // The one options object handed to KaTeX for every formula.
  var mathOptions = { globalGroup: true, macros: macros, throwOnError: false };

  // SPIKE (spike/formel): formel forgets macros between formulas and refuses to redefine with
  // \newcommand. Each formula gets the definitions met before it, written as \def, which
  // always defines.
  var formelDefs = "";

  // ponytail: an optional argument with a default ("[1][x]") is not converted.
  function toDef(tex) {
    return tex.replace(
      /\\(?:re)?newcommand\s*(?:\{\s*(\\[A-Za-z]+)\s*\}|(\\[A-Za-z]+))\s*(?:\[(\d)\])?/g,
      function (whole, braced, bare, count) {
        var params = "";
        for (var i = 1; i <= (parseInt(count, 10) || 0); i++) {
          params += "#" + i;
        }
        return "\\def" + (braced || bare) + params;
      }
    );
  }

  // The \def statements of tex, bodies included.
  function defsOf(tex) {
    var found = "";
    var start = /\\def\s*\\[A-Za-z]+[^{]*\{/g;
    var match;
    while ((match = start.exec(tex))) {
      var depth = 1;
      var i = start.lastIndex;
      for (; i < tex.length && depth > 0; i++) {
        if (tex[i] === "\\") {
          i++;
        } else if (tex[i] === "{") {
          depth++;
        } else if (tex[i] === "}") {
          depth--;
        }
      }
      found += tex.slice(match.index, i) + " ";
      start.lastIndex = i;
    }
    return found;
  }

  // Returns whether the macros changed.
  function setGlobalMacros(tex) {
    var changed = tex !== globalMacros;
    globalMacros = tex;
    return changed;
  }

  // MathJax lets \newcommand redefine a command that exists. KaTeX refuses, and it
  // already knows \R, \N, \Z... which documents written for CodiMD often define
  // themselves. This stands in for KaTeX's \newcommand and always defines.
  // "context" is KaTeX's macro expander, handed to every macro written as a function;
  // the tokens it returns go back to it unchanged, as its own \newcommand does.
  function lenientNewcommand(context) {
    var name = context.consumeArg().tokens[0].text;
    var body = context.consumeArg().tokens;
    var argumentCount = 0;
    // "\newcommand{\vect}[1]{...}": the number of arguments comes between brackets.
    if (body.length === 1 && body[0].text === "[") {
      var digits = "";
      var token = context.expandNextToken();
      while (token.text !== "]" && token.text !== "EOF") {
        digits += token.text;
        token = context.expandNextToken();
      }
      argumentCount = parseInt(digits, 10) || 0;
      body = context.consumeArg().tokens;
    }
    context.macros.set(name, { tokens: body, numArgs: argumentCount });
    return "";
  }

  // To call before each render: forget the macros of the previous one, then define the
  // global ones again.
  function resetMath() {
    Object.keys(macros).forEach(function (name) {
      delete macros[name];
    });
    macros["\\newcommand"] = lenientNewcommand;
    formelDefs = defsOf(toDef(globalMacros));
    if (globalMacros) {
      // The output is thrown away: only the definitions it leaves in "macros" matter.
      try {
        window.katex.renderToString(globalMacros, mathOptions);
      } catch (e) {
        // A line KaTeX cannot read ("\\newcommand{}{x}") leaves its macro undefined. It
        // must not keep every document from rendering.
      }
    }
  }

  // CodiMD renders math with MathJax, which is more lenient than KaTeX. This bridges the
  // differences met in real documents and returns the engine to hand to texmath.
  function lenientMath() {
    // texmath finds the formulas with regular expressions; some of them are replaced.
    var dollars = window.texmath.rules.dollars;
    var brackets = window.texmath.rules.brackets;
    // Inline math, "$...$": [\s\S] matches any character including a line break, which
    // the original "." did not, so a formula may run over several lines of a paragraph.
    dollars.inline[1].rex = /\$((?:[^\s\\])|(?:\S[\s\S]*?[^\s\\]))\$/gy;
    // Display math, "$$...$$": (?:[^$]|\$(?!\$)) accepts a "$" as long as no second one
    // follows it, so a lone "$" inside the formula does not end it.
    dollars.inline[0].rex = /\${2}((?:[^$]|\$(?!\$))*?[^\\])\${2}/gy;
    dollars.block[1].rex = /\${2}((?:[^$]|\$(?!\$))*?[^\\])\${2}/gmy;
    // A numbered equation, "$$...$$ (1)" or "\[...\] (1)". The number must sit on the
    // line of the closing delimiter, hence [ \t]* and not \s*: otherwise a line such as
    // "(a) Show that..." written under a formula is taken for its number.
    dollars.block[0].rex = /\${2}((?:[^$]|\$(?!\$))*?[^\\])\${2}[ \t]*\(([^)\s]+?)\)/gmy;
    brackets.block[0].rex = /\\\[(((?!\\\]|\\\[)[\s\S])+?)\\\][ \t]*\(([^)$\r\n]+?)\)/gmy;

    // MathJax shows a "$" met inside a formula as a dollar sign; KaTeX wants "\$". Inside
    // \text{...} the "$" stays: there it opens a formula again, as in LaTeX.
    var TEXT_GROUP = /\\(?:text[a-z]*|mbox)\s*\{/y;

    function escapeDollars(tex) {
      var result = "";
      var textDepth = 0; // braces open inside a \text{...}, 0 outside of one
      for (var i = 0; i < tex.length; i++) {
        var character = tex[i];
        if (character === "\\") {
          TEXT_GROUP.lastIndex = i;
          var opening = textDepth === 0 ? TEXT_GROUP.exec(tex) : null;
          if (opening) {
            result += opening[0];
            i += opening[0].length - 1;
            textDepth = 1;
          } else {
            // A command or an escaped character: "\$" is already what KaTeX wants.
            result += character + (tex[i + 1] || "");
            i++;
          }
          continue;
        }
        if (textDepth > 0) {
          if (character === "{") {
            textDepth++;
          } else if (character === "}") {
            textDepth--;
          }
        } else if (character === "$") {
          result += "\\";
        }
        result += character;
      }
      return result;
    }

    function toKatex(tex) {
      // \require{...} loads a MathJax extension; KaTeX has those built in.
      return escapeDollars(tex.replace(/\\require\s*\{[^}]*\}/g, ""));
    }

    // SPIKE (spike/formel): formel when preview/formel.js has loaded it. A formula formel
    // cannot read falls back to KaTeX, framed so that it shows.
    return {
      renderToString: function (tex, options) {
        var source = toKatex(tex);
        if (katemark.formel) {
          var withDefs = formelDefs + toDef(source);
          formelDefs += defsOf(toDef(source));
          try {
            return katemark.formel(withDefs, !!options.displayMode);
          } catch (e) {
            return '<span class="formel-fallback" title="' + katemark.escapeHtml(String(e)) + '">' +
              window.katex.renderToString(withDefs, options) + "</span>";
          }
        }
        return window.katex.renderToString(source, options);
      },
    };
  }

  // markdown-it copies a raw HTML block untouched, so "$x$" in a <td> stayed text; MathJax
  // renders it. This renders the formulas found in the text between the tags.
  // shortcut: a formula cut by a tag ("$a<sub>1</sub>$") stays text; join the text nodes
  // in the DOM if a document needs it.
  function mathInHtml(md, texmathOptions) {
    // Nothing but the math rules, so "*" or "_" in the HTML keep their meaning.
    var mathOnly = window.markdownit("zero").use(window.texmath, texmathOptions);
    // The text, then what is copied as is: comments, code and scripts, tags. A "<" not
    // followed by a tag name ("a < b") is text.
    var PARTS = /((?:[^<]|<(?![A-Za-z!/?]))+)|<!--[\s\S]*?-->|<(script|style|pre|code|textarea)\b[\s\S]*?<\/\2\s*>|<[^>]*>/gi;
    var decoder = document.createElement("textarea");

    function withMath(text) {
      if (text.indexOf("$") < 0 && text.indexOf("\\") < 0) {
        return text;
      }
      decoder.innerHTML = text; // decodes "&lt;" and the like, creates no element
      return mathOnly.renderInline(decoder.value);
    }

    md.renderer.rules.html_block = function (tokens, idx) {
      return tokens[idx].content.replace(PARTS, function (part, text) {
        return text ? withMath(text) : part;
      });
    };
  }

  katemark.lenientMath = lenientMath;
  katemark.mathInHtml = mathInHtml;
  katemark.mathOptions = mathOptions;
  katemark.resetMath = resetMath;
  katemark.setGlobalMacros = setGlobalMacros;
})();
