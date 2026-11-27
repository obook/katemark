// Helpers for putting a rendering into a page: colour and label it, size it, embed it, export it.
//
// Shipped as `@manuel-rhdt/formel-wasm/dom`. Pure string work, no DOM access, so everything here
// runs in a browser, a bundler, Node and Deno alike — the one exception is `htmlImage`, which
// returns markup for you to insert.

/** Escapes text for use in XML/HTML text and double-quoted attributes. */
export function escapeXml(text) {
  return String(text)
    .replace(/&/g, "&amp;")
    .replace(/</g, "&lt;")
    .replace(/>/g, "&gt;")
    .replace(/"/g, "&quot;");
}

/** Collapses whitespace so that the text fits on one line (alt texts, titles). */
export function oneLine(text) {
  return String(text).replace(/\s+/g, " ").trim();
}

/**
 * Applies the chosen colors and adds an accessible name to an SVG produced by formel-wasm.
 *
 * The renderer paints everything in `black`, so recoloring is a replacement of that keyword.
 * `bg` of `null` keeps the background transparent.
 */
let svgIdCounter = 0;

const XML_ENTITIES = new Set(["amp;", "lt;", "gt;", "quot;", "apos;"]);
// The HTML named entities math-core emits, as codepoints. Only amp/lt/gt/quot/apos exist in XML.
const HTML_ENTITIES = { ApplyFunction: 0x2061, InvisibleTimes: 0x2062, InvisibleComma: 0x2063, nbsp: 0xa0 };

/**
 * Makes MathML safe to embed in an XML document. math-core writes HTML named entities such as
 * `&ApplyFunction;`, which are undefined in XML and make an SVG image fail to load; they become
 * numeric character references. Anything unrecognised is escaped rather than left to break the file.
 */
export function mathmlForXml(mathml) {
  return String(mathml).replace(/&([A-Za-z][A-Za-z0-9]+;|#x[0-9a-fA-F]+;|#\d+;)?/g, (match, entity) => {
    if (!entity) return "&amp;";
    if (entity[0] === "#" || XML_ENTITIES.has(entity)) return match;
    const name = entity.slice(0, -1);
    if (name in HTML_ENTITIES) return `&#x${HTML_ENTITIES[name].toString(16)};`;
    return `&amp;${entity}`;
  });
}

/**
 * Applies the chosen colors and makes an SVG produced by formel-wasm self-describing.
 *
 * The renderer paints everything in `black`, so recoloring is a replacement of that keyword.
 * `bg` of `null` keeps the background transparent.
 *
 * `alt` becomes the title (the accessible name). With `mathml`, the SVG also gets a `desc`
 * pointing at the embedded MathML in `<metadata>`: an image cannot expose a formula's structure
 * to a screen reader, but the file can still carry the expression for tools and for assistive
 * technology that understands MathML. Ids are unique per call, so several of these SVGs can be
 * inlined into one document.
 */
export function styleSvg(rawSvg, { fg = "#000000", bg = null, alt = "", mathml = "" } = {}) {
  let svg = rawSvg.replaceAll('"black"', `"${fg}"`);
  const label = escapeXml(oneLine(alt));
  const open = svg.match(/^<svg\s([^>]*)>/);
  if (!open) return svg;

  // The renderer writes its own `<title>` when it was given an accessible name. Keep that one: a
  // second title would override it, and its ids are per document while the caller's are per call.
  const titled = /<title[\s>]/.test(svg);

  const id = `m${++svgIdCounter}`;
  let extra = "";
  if (label && !titled) extra += `<title id="${id}t">${label}</title>`;
  let labelled = "";
  if (mathml) {
    extra += `<desc id="${id}d">Vector rendering of a formula. The MathML it was typeset from is in the metadata element.</desc>`;
    extra += `<metadata>${mathmlForXml(mathml).replace(/^<math\b/, '<math xmlns="http://www.w3.org/1998/Math/MathML"')}</metadata>`;
    labelled = titled
      ? ` aria-describedby="${id}d"`
      : ` aria-labelledby="${id}t" aria-describedby="${id}d"`;
  }
  if (bg) {
    const box = open[1].match(/viewBox="([^"]+)"/);
    if (box) {
      const [x, y, w, h] = box[1].trim().split(/\s+/);
      extra += `<rect x="${x}" y="${y}" width="${w}" height="${h}" fill="${bg}"/>`;
    }
  }
  const attrs = label && !titled
    ? `${open[1]} role="img" aria-label="${label}"${labelled}`
    : labelled
      ? `${open[1]}${labelled}`
      : open[1];
  return `<svg ${attrs}>${extra}${svg.slice(open[0].length)}`;
}

/** The parsed `viewBox` of an SVG string, or null. */
export function viewBox(svg) {
  const m = svg.match(/viewBox="([^"]+)"/);
  if (!m) return null;
  const [x, y, width, height] = m[1].trim().split(/\s+/).map(Number);
  return { x, y, width, height };
}

/**
 * Replaces the SVG's `width` and `height`, which the renderer already writes in `em`. An element
 * cannot carry the same attribute twice, so the old ones have to be dropped first.
 */
function withSize(svg, width, height) {
  const open = svg.match(/<svg\b[^>]*>/);
  if (!open) return svg;
  const tag = open[0]
    .replace(/\s(?:width|height)="[^"]*"/g, "")
    .replace(/^<svg/, `<svg width="${width}" height="${height}"`);
  return tag + svg.slice(open[0].length);
}

/**
 * Gives the SVG an absolute size, so that it works on its own (the renderer's SVG only has a
 * `viewBox`). `pxPerEm` is the size of one em of the font; `unitsPerEm` converts font units.
 */
export function sizeSvg(svg, pxPerEm, unitsPerEm) {
  const box = viewBox(svg);
  if (!box) return { svg, width: 0, height: 0 };
  const scale = pxPerEm / unitsPerEm;
  const width = box.width * scale;
  const height = box.height * scale;
  const sized = withSize(svg, width.toFixed(2), height.toFixed(2));
  return { svg: `<?xml version="1.0" encoding="UTF-8"?>\n${sized}\n`, width, height };
}

/**
 * The SVG as a standalone file whose width and height are in pt, so that a formula for
 * `textSize`-pt text is inserted at exactly that size by apps that honor SVG units. pt rather
 * than px: px is 1/96 in in CSS, but some apps take it as 1/72 in.
 */
export function svgInPoints(svg, textSize, unitsPerEm) {
  const box = viewBox(svg);
  if (!box) return { svg, widthPt: 0, heightPt: 0 };
  const scale = textSize / unitsPerEm;
  const widthPt = box.width * scale;
  const heightPt = box.height * scale;
  const sized = withSize(svg, `${widthPt.toFixed(3)}pt`, `${heightPt.toFixed(3)}pt`);
  return { svg: `<?xml version="1.0" encoding="UTF-8"?>\n${sized}\n`, widthPt, heightPt };
}

/** Pixels per em for a PNG of a formula for `textSize`-pt text at `dpi`. */
export function pixelsPerEm(textSize, dpi) {
  return (textSize / 72) * dpi;
}

let crcTable = null;
function crc32(bytes) {
  if (!crcTable) {
    crcTable = new Uint32Array(256);
    for (let n = 0; n < 256; n++) {
      let c = n;
      for (let k = 0; k < 8; k++) c = c & 1 ? 0xedb88320 ^ (c >>> 1) : c >>> 1;
      crcTable[n] = c >>> 0;
    }
  }
  let c = 0xffffffff;
  for (const b of bytes) c = crcTable[(c ^ b) & 0xff] ^ (c >>> 8);
  return (c ^ 0xffffffff) >>> 0;
}

/**
 * Tags PNG bytes with a resolution (a `pHYs` chunk right after `IHDR`, replacing any existing
 * one), so that apps which read it insert the image at its intended physical size. Canvas
 * encoders write no resolution. Returns the input unchanged if it is not a PNG.
 */
export function setPngResolution(png, dpi) {
  const bytes = png instanceof Uint8Array ? png : new Uint8Array(png);
  const SIGNATURE = [137, 80, 78, 71, 13, 10, 26, 10];
  if (bytes.length < 33 || SIGNATURE.some((b, i) => bytes[i] !== b)) return bytes;
  const view = new DataView(bytes.buffer, bytes.byteOffset, bytes.byteLength);
  const type = (at) => String.fromCharCode(...bytes.subarray(at + 4, at + 8));
  if (type(8) !== "IHDR") return bytes;

  const ppm = Math.round(dpi / 0.0254);   // pixels per metre
  const chunk = new Uint8Array(21);
  const cv = new DataView(chunk.buffer);
  cv.setUint32(0, 9);
  chunk.set([0x70, 0x48, 0x59, 0x73], 4);   // "pHYs"
  cv.setUint32(8, ppm);
  cv.setUint32(12, ppm);
  chunk[16] = 1;                             // unit: metre
  cv.setUint32(17, crc32(chunk.subarray(4, 17)));

  // copy the chunks, dropping any existing pHYs and inserting ours after IHDR
  const parts = [bytes.subarray(0, 8)];
  let at = 8;
  while (at + 12 <= bytes.length) {
    const length = view.getUint32(at);
    const end = at + 12 + length;
    if (end > bytes.length) return bytes;    // damaged; leave it alone
    if (type(at) !== "pHYs") parts.push(bytes.subarray(at, end));
    if (type(at) === "IHDR") parts.push(chunk);
    at = end;
  }
  const out = new Uint8Array(parts.reduce((n, p) => n + p.length, 0));
  let offset = 0;
  for (const p of parts) { out.set(p, offset); offset += p.length; }
  return out;
}

/** Base64 of the UTF-8 encoding of `text`. */
export function toBase64(text) {
  const bytes = new TextEncoder().encode(text);
  let binary = "";
  const chunk = 0x8000;
  for (let i = 0; i < bytes.length; i += chunk) {
    binary += String.fromCharCode(...bytes.subarray(i, i + chunk));
  }
  return btoa(binary);
}

export function svgDataUri(svg) {
  return `data:image/svg+xml;base64,${toBase64(svg)}`;
}

function em(value) {
  // up to 3 decimals, without trailing zeros
  return `${Number(value.toFixed(3))}em`;
}

/**
 * An SVG as a `data:` URI that sits in a double-quoted HTML attribute. Percent-encodes only what
 * has to be (not everything, like `encodeURIComponent`, and not base64, which is a third larger),
 * so the snippet stays short and mostly readable.
 */
export function svgDataUriEncoded(svg) {
  const body = svg
    .replace(/<\?xml[^>]*\?>\s*/, "")
    .replace(/\s+/g, " ")
    .replace(/%/g, "%25")
    .replace(/#/g, "%23")
    .replace(/</g, "%3C")
    .replace(/>/g, "%3E")
    .replace(/"/g, "%22")
    .replace(/&/g, "%26");
  return `data:image/svg+xml,${body}`;
}

/**
 * Snippets for embedding a rendering in a web page.
 *
 * @param {object} p
 * @param {string} p.svg        SVG for inline use (see `styleSvg`); it may paint with `currentColor`
 * @param {string} [p.imgSvg]   SVG for an `<img>`, which cannot inherit the text color; defaults to `svg`
 * @param {string} [p.mathml]   the formula's MathML, offered as-is for browsers that render it natively
 * @param {string} p.alt        alternative text
 * @param {boolean} p.display   block formula (centered) rather than inline
 * @param {number} p.width      viewBox width in font units
 * @param {number} p.height     viewBox height in font units
 * @param {number} p.baseline   distance from the top of the viewBox to the baseline
 * @param {number} p.unitsPerEm font units per em
 *
 * Sizes are in em, so the formula is as big as the surrounding text. For inline formulas the
 * image is shifted down by the part of the box below the baseline, so it lines up with the text.
 */
export function snippets({ svg, imgSvg = svg, mathml = "", alt, display, width, height, baseline, unitsPerEm }) {
  const w = em(width / unitsPerEm);
  const h = em(height / unitsPerEm);
  const below = em((height - baseline) / unitsPerEm);
  const attrAlt = escapeXml(oneLine(alt));
  const position = display ? "display:block;margin:1em auto" : `vertical-align:-${below}`;

  return {
    // inline <svg> needs explicit sizes, otherwise browsers default to 300x150
    svg: withSize(svg, w, h).replace(/(height="[^"]*")/, `$1 style="${position}"`),
    img: `<img src="${svgDataUriEncoded(imgSvg)}" alt="${attrAlt}" style="height:${h};${position}">`,
    mathml,
  };
}

/** A CSS length, as `inlineHtml` accepts it: `<number>` (px) or `<number><unit>`. */
const CSS_LENGTH = /^([\d.]+(?:e[-+]?\d+)?)\s*(em|rem|px|pt|pc|ex|ch|vw|vh|vmin|vmax)?$/i;

/**
 * A rendering as an element for an HTML document, at a chosen size.
 *
 * `size` is one em of the formula: `"1.2em"` for a formula a fifth larger than the text it sits
 * in, `"18px"` for an absolute one, a bare number meaning px. The formula sits on the line's
 * baseline, so `element.innerHTML = inlineHtml(rendering)` is all an inline formula needs; with
 * `display: true` it becomes a centred block instead.
 *
 * `color` defaults to `currentColor`, so the formula takes the page's text colour.
 *
 * ```js
 * const rendering = renderer.render_latex("\\frac{a}{b}", false);
 * element.innerHTML = inlineHtml(rendering, { size: "1.2em" });
 * ```
 */
export function inlineHtml(
  rendering,
  { size = "1em", display = false, color = "currentColor", alt = rendering.alt } = {},
) {
  const match = typeof size === "number" ? [null, String(size), "px"] : CSS_LENGTH.exec(String(size).trim());
  if (!match) {
    throw new TypeError(`inlineHtml: expected a CSS length for size, got ${JSON.stringify(size)}`);
  }
  const value = Number(match[1]);
  const unit = match[2] ?? "em";
  const upem = rendering.units_per_em;
  if (!(upem > 0)) {
    throw new TypeError("inlineHtml: the rendering needs units_per_em");
  }

  const scale = (units) => Number(((units * value) / upem).toFixed(3));
  const width = `${scale(rendering.width)}${unit}`;
  const height = `${scale(rendering.height)}${unit}`;
  const below = scale(rendering.height - rendering.baseline);
  const position = display ? "display:block;margin:1em auto" : `vertical-align:${-below}${unit}`;

  const styled = styleSvg(rendering.svg, { fg: color, alt });
  return withSize(styled, width, height).replace(/(height="[^"]*")/, `$1 style="${position}"`);
}

/**
 * An HTML `<img>` for transferring an image to rich-text apps (drag and drop, clipboard).
 * `width` and `height` are in CSS px and set the size the image gets when inserted.
 */
export function htmlImage({ src, alt = "", width, height }) {
  const size = width && height ? ` width="${Math.round(width)}" height="${Math.round(height)}"` : "";
  return `<img src="${src}" alt="${escapeXml(oneLine(alt))}"${size}>`;
}
