// frontmatter.js
// GitHub task-list checkboxes and the metadata table made from leading YAML front matter.
//
// Part of the preview page: adds its functions to window.katemark (see core.js).
// Code by uwuclxdy, moved out of preview.js.
(function () {
  "use strict";

  var katemark = window.katemark;
  var escapeHtml = katemark.escapeHtml;

  // GitHub-flavored task list checkboxes (adapted from markdown-it-task-lists).
  function taskLists(md) {
    md.core.ruler.after("inline", "github-task-lists", function (state) {
      var tokens = state.tokens;
      for (var i = 2; i < tokens.length; i++) {
        if (isTodoItem(tokens, i)) {
          todoify(tokens[i], state.Token);
          attrSet(tokens[i - 2], "class", "task-list-item");
          var p = parentList(tokens, i - 2);
          if (p >= 0) attrSet(tokens[p], "class", "contains-task-list");
        }
      }
    });
    function attrSet(token, name, value) {
      var idx = token.attrIndex(name);
      if (idx < 0) token.attrPush([name, value]);
      else token.attrs[idx][1] = value;
    }
    function parentList(tokens, index) {
      var target = tokens[index].level - 1;
      for (var i = index - 1; i >= 0; i--) {
        if (tokens[i].level === target) return i;
      }
      return -1;
    }
    function isTodoItem(tokens, i) {
      return (
        tokens[i].type === "inline" &&
        tokens[i - 1].type === "paragraph_open" &&
        tokens[i - 2].type === "list_item_open" &&
        /^\[[ xX]\] /.test(tokens[i].content)
      );
    }
    function todoify(token, Token) {
      var checked = /^\[[xX]\] /.test(token.content);
      var box = new Token("html_inline", "", 0);
      box.content =
        '<input class="task-list-item-checkbox"' +
        (checked ? ' checked=""' : "") +
        ' disabled="" type="checkbox"> ';
      token.children.unshift(box);
      token.children[1].content = token.children[1].content.replace(/^\[[ xX]\] /, "");
    }
  }

  // Leading YAML frontmatter (very first line "---" through a closing "---")
  // renders as a GitHub-style metadata table instead of an <hr> plus raw text.
  var FRONT_MATTER = /^---[ \t]*\r?\n([\s\S]*?\n)?---[ \t]*(?:\r?\n|$)/;

  function frontMatterTable(src) {
    var m = FRONT_MATTER.exec(src);
    if (!m) {
      return null;
    }
    var data;
    try {
      data = window.jsyaml ? window.jsyaml.load(m[1] || "") : null;
    } catch (e) {
      return null;
    }
    if (!data || typeof data !== "object" || Array.isArray(data)) {
      return null;
    }
    var rows = "";
    Object.keys(data).forEach(function (key) {
      var v = data[key];
      var text = v == null ? "" : typeof v === "object" ? JSON.stringify(v) : String(v);
      rows += "<tr><td>" + escapeHtml(key) + "</td><td>" + escapeHtml(text) + "</td></tr>";
    });
    if (!rows) {
      return null;
    }
    return { html: "<table>" + rows + "</table>\n", body: src.slice(m[0].length), lines: m[0].split("\n").length - 1 };
  }

  katemark.taskLists = taskLists;
  katemark.frontMatterTable = frontMatterTable;
})();
