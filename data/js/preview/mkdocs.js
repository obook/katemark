// mkdocs.js
// MkDocs admonitions: "!!! note" followed by an indented body, "??? note" for a folded
// one and "???+ note" for one that folds but starts open.
//
// Part of the preview page: adds its functions to window.katdown (see core.js).
// Author: Olivier Booklage
// Date: October 2026
// License: GPL-3.0-or-later
(function () {
  "use strict";

  var katdown = window.katdown;
  var escapeHtml = katdown.escapeHtml;

  // The marker, the type, then an optional title in double quotes. An empty title, "",
  // asks for no title at all.
  var MARKER = /^(!!!|\?\?\?\+?)[ \t]+([\w-]+)(?:[ \t]+"(.*)")?[ \t]*$/;
  // The body is every following line indented by at least this many spaces.
  var BODY_INDENT = 4;

  // Line after the last line of the body that starts under the marker at startLine.
  function findBodyEnd(state, startLine, endLine) {
    var line = startLine + 1;
    while (line < endLine && (state.isEmpty(line) || state.sCount[line] - state.blkIndent >= BODY_INDENT)) {
      line++;
    }
    // Blank lines at the end belong to what follows the admonition.
    while (line > startLine + 1 && state.isEmpty(line - 1)) {
      line--;
    }
    return line;
  }

  // Parse the lines of the body as Markdown of their own, as if their indentation were
  // the left margin: without that, four spaces would make them a code block.
  function parseBody(state, firstLine, lastLine) {
    var parentType = state.parentType;
    var lineMax = state.lineMax;
    state.parentType = "admonition";
    state.lineMax = lastLine;
    state.blkIndent += BODY_INDENT;
    state.md.block.tokenize(state, firstLine, lastLine);
    state.blkIndent -= BODY_INDENT;
    state.lineMax = lineMax;
    state.parentType = parentType;
  }

  function admonitions(md) {
    // "alt" lists the blocks this rule may interrupt, so that an admonition can start
    // right under a paragraph.
    var interrupts = { alt: ["paragraph", "reference", "blockquote", "list"] };
    md.block.ruler.before("paragraph", "admonition", parseAdmonition, interrupts);

    function parseAdmonition(state, startLine, endLine, silent) {
      var lineStart = state.bMarks[startLine] + state.tShift[startLine];
      var match = MARKER.exec(state.src.slice(lineStart, state.eMarks[startLine]));
      if (!match) {
        return false;
      }
      // markdown-it calls a rule in "silent" mode to ask whether it matches here.
      if (silent) {
        return true;
      }
      var foldable = match[1] !== "!!!";
      var look = katdown.calloutLook(match[2].toLowerCase());
      var bodyEnd = findBodyEnd(state, startLine, endLine);
      var tag = foldable ? "details" : "div";

      var open = state.push("admonition_open", tag, 1);
      open.block = true;
      open.map = [startLine, bodyEnd];
      open.attrSet("class", "markdown-alert markdown-alert-" + look.tone);
      if (match[1] === "???+") {
        open.attrSet("open", "");
      }

      var titleHtml = escapeHtml(look.title);
      if (match[3] !== undefined) {
        titleHtml = md.renderInline(match[3]);
      }
      // A block that folds needs a title to click on, even when "" asked for none.
      if (!titleHtml && foldable) {
        titleHtml = escapeHtml(look.title);
      }
      if (titleHtml) {
        var title = state.push("html_block", "", 0);
        title.block = true;
        title.content = katdown.calloutTitle(look, titleHtml, foldable ? "summary" : "p");
      }

      parseBody(state, startLine + 1, bodyEnd);

      var close = state.push("admonition_close", tag, -1);
      close.block = true;
      state.line = bodyEnd;
      return true;
    }
  }

  katdown.admonitions = admonitions;
})();
