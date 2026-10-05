# Roadmap

Ideas and open work, not scheduled yet.

## Distribution packages

Today CPack makes the `.deb` files and they sit on the GitHub release page,
so nobody gets an update through `apt`.

### Ubuntu, through Debian

Ubuntu copies Debian unstable into `universe`, so the way in is a Debian
package. A release that is already out never gets it: 26.04 is too late and
27.04 needs the package in unstable before the Debian import freeze, usually
in late February.

- [ ] Sort out the bundled JavaScript (next section).
- [ ] Write a `debian/` directory and get it clean under `lintian`. The
      CPack package does not count: Debian builds from a source package.
- [ ] File an ITP bug against `wnpp` (`reportbug wnpp`).
- [ ] Find a sponsor, since only a Debian developer can upload. Ask the
      Debian Qt/KDE team first (`debian-qt-kde` list, salsa.debian.org).
      Otherwise upload to mentors.debian.net and file an RFS bug.
- [ ] Get through the NEW queue, where the licenses are reviewed.

Ubuntu has its own door, a `needs-packaging` bug on Launchpad and a MOTU
sponsor, but it recommends going through Debian.

### Bundled JavaScript

Debian rejects a minified file that comes without its source and `data/js`
holds 12 of them. Checked against the Ubuntu 24.04 archive on 2026-10-05:

| Library | Package |
|---|---|
| KaTeX | `libjs-katex`, `fonts-katex` |
| highlight.js | `libjs-highlight.js` |
| markdown-it | `libjs-markdown-it` |
| markdown-it-sub, markdown-it-sup | `libjs-markdown-it-sub`, `libjs-markdown-it-sup` |
| js-yaml | `node-js-yaml` |
| markdown-it-container, -emoji, -footnote, -ins, -mark | none |
| Mermaid | none |

- [ ] Run the same check against Debian unstable and compare the packaged
      versions with the bundled ones.
- [ ] For each library with no package, either ship the unminified source
      next to the file or package the library first. Mermaid is the largest
      and brings many dependencies of its own.

### KDE neon

neon builds KDE's own software, hosted on invent.kde.org, and takes no
outside project. Katemark would have to come in through KDE:

- [ ] Ask the Kate maintainers whether the plugin could live in `addons/`
      of `utilities/kate`. It would then ship with Kate everywhere.
- [ ] Otherwise apply to the
      [KDE incubator](https://community.kde.org/Incubator). Katemark
      becomes a KDE project and neon builds it.

### In the meantime

- [ ] Open a Launchpad PPA so that Ubuntu 26.04 users get updates through
      `apt`. It cannot serve neon while neon sits on 24.04: that archive has
      Qt 6.4 and no KF6 TextEditor, where Katemark needs Qt 6.5 and KF6.
