// math.js
// Makes KaTeX accept the formulas of documents written for MathJax (CodiMD), and keeps
// the macros defined along the way.
//
// Part of the preview page: adds its functions to window.katdown (see core.js).
// Author: Olivier Booklage
// Date: October 2026
// License: GPL-3.0-or-later
(function () {
  "use strict";

  var katdown = window.katdown;

  // Macros defined so far in the document being rendered (\newcommand, \def...). With
  // globalGroup, KaTeX writes each definition into this object, so it serves the
  // formulas that follow, as with MathJax.
  var macros = {};
  // \newcommand lines from the plugin's settings, available in every document.
  var globalMacros = "";
  // The one options object handed to KaTeX for every formula.
  var mathOptions = { globalGroup: true, macros: macros, throwOnError: false };

  function setGlobalMacros(tex) {
    globalMacros = tex;
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
    if (globalMacros) {
      // The output is thrown away: only the definitions it leaves in "macros" matter.
      window.katex.renderToString(globalMacros, mathOptions);
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

    function toKatex(tex) {
      // \require{...} loads a MathJax extension; KaTeX has those built in.
      var withoutRequire = tex.replace(/\\require\s*\{[^}]*\}/g, "");
      // MathJax shows a "$" met inside a formula as a dollar sign; KaTeX wants "\$".
      return withoutRequire.replace(/(^|[^\\])\$/g, function (all, before) {
        return before + "\\$";
      });
    }

    return {
      renderToString: function (tex, options) {
        return window.katex.renderToString(toKatex(tex), options);
      },
    };
  }

  katdown.lenientMath = lenientMath;
  katdown.mathOptions = mathOptions;
  katdown.resetMath = resetMath;
  katdown.setGlobalMacros = setGlobalMacros;
})();
