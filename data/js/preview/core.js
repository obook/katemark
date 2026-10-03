// core.js
// The namespace shared by the files of the preview page, and the helpers that
// several of them use. Loaded first: every other file adds its functions to
// window.katdown.
// Author: Olivier Booklage
// Date: October 2026
// License: GPL-3.0-or-later
(function () {
  "use strict";

  var katdown = (window.katdown = {});

  // Titles of the alerts and callouts, translated: the host sends them before the first
  // render (see __setLabels in preview.js).
  katdown.labels = {};

  function escapeHtml(s) {
    return s
      .replace(/&/g, "&amp;")
      .replace(/</g, "&lt;")
      .replace(/>/g, "&gt;");
  }

  // The id a heading gets from its title, built as GitHub does: lower case, punctuation
  // dropped, spaces turned into hyphens. "What's new?" gives "whats-new".
  // \p{L} and \p{N} are letters and digits of any alphabet, accented ones included.
  function slugify(text) {
    return text
      .trim()
      .toLowerCase()
      .replace(/[^\p{L}\p{N}\s_-]/gu, "")
      .replace(/\s+/g, "-");
  }

  function isDark() {
    return document.documentElement.getAttribute("data-pv-scheme") === "dark";
  }

  katdown.escapeHtml = escapeHtml;
  katdown.slugify = slugify;
  katdown.isDark = isDark;
})();
