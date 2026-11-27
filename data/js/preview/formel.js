// formel.js
// SPIKE (spike/formel): loads formel, a math typesetter that uses an OpenType MATH font,
// and hands it to math.js. Until it is ready, and for any formula it cannot read, KaTeX
// renders instead.
//
// formel comes as classic scripts (js/formel/make-classic.sh): the page's file:// origin
// gets neither ES modules nor fetch() from qrc:.
// Author: Olivier Booklage
// Date: November 2026
// License: GPL-3.0-or-later
(function () {
  "use strict";

  function decode(base64) {
    var text = atob(base64);
    var bytes = new Uint8Array(text.length);
    for (var i = 0; i < text.length; i++) {
      bytes[i] = text.charCodeAt(i);
    }
    return bytes;
  }

  var source = window.formelBytes;
  window.formelWasm
    .init({ module_or_path: decode(source.wasm) })
    .then(function () {
      var renderer = new window.formelWasm.MathRenderer(decode(source.font));
      delete window.formelBytes;
      katemark.formel = function (tex, display) {
        var rendering = renderer.render_latex(tex, display);
        try {
          return window.formelDom.inlineHtml(rendering, { display: display });
        } finally {
          rendering.free();
        }
      };
      katemark.rerender();
      console.log("formel: ready, " + document.querySelectorAll(".formel-fallback").length + " fallback(s)");
    })
    .catch(function (e) {
      console.error("formel: " + e);
    });
})();
