#!/bin/sh
# SPIKE (spike/formel): the preview page has a file:// origin, so qrc: serves it classic scripts
# only. ES modules are refused by CORS and fetch() refuses the qrc: scheme. This turns formel's
# modules into classic scripts and carries the .wasm and the font as base64.
set -eu
cd "$(dirname "$0")"

{
  echo '(function () {'
  sed -e 's/^export function /function /' -e 's/^export class /class /' -e '/^export {/d' \
    -e 's/import\.meta\.url/document.baseURI/' \
    -e 's/^export default __wbg_init;/window.formelWasm = { init: __wbg_init, MathRenderer: MathRenderer };/' \
    formel_wasm.js
  echo '})();'
} > formel_wasm.classic.js

{
  echo '(function () {'
  sed -e 's/^export function /function /' dom.js
  echo 'window.formelDom = { inlineHtml: inlineHtml };'
  echo '})();'
} > dom.classic.js

{
  printf 'window.formelBytes = {\n  wasm: "'
  base64 -w0 formel_wasm_bg.wasm
  printf '",\n  font: "'
  base64 -w0 ../../fonts/NotoSansMath-Regular.otf
  printf '",\n};\n'
} > bytes.classic.js
