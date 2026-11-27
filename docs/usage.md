# Using Katemark

## Using the preview

| To | Do this |
|---|---|
| Show or hide the preview | <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>M</kbd>, the **Preview** button of the main toolbar or Tools > Preview |
| See the document as GitHub will show it | Check **GitHub only** above the preview |
| Export what the preview shows | The **PDF** and **HTML** buttons above the preview, or Tools > Export Preview as PDF and Tools > Export Preview as HTML |
| Follow a link | Click it: a local file opens in Kate, a web address in your browser, a link to a heading or to an Obsidian block (`#^id`) scrolls the preview. Any other link, such as `zotero:` or `obsidian:`, asks before it starts another program. Point at a link to see where it goes |
| Zoom the preview, diagrams included | <kbd>Ctrl</kbd> + mouse wheel over it; <kbd>Ctrl</kbd>+<kbd>0</kbd> or **Reset Zoom** in its context menu puts it back to 100% |
| Paste a picture | Copy an image, then paste in the editor |

- The preview follows the active Markdown document. It keeps its last
  content when you close that document or switch to one that is not
  Markdown, and stays open until you hide it.
- Both exports use GitHub's light colors, whatever the preview shows. The
  HTML file embeds the pictures of the document's folder and holds no script.
- The preview shows raw HTML from a document, but runs none of its
  scripts, frames or forms.
- Katemark saves a pasted picture as `image-<date>-<time>.png` in the folder
  of the document, so save the document first. It inserts the link
  `![](image-<date>-<time>.png)` and leaves the cursor between the brackets.
  It does so when the clipboard holds an image and no text.
- If the toolbar button is missing, show the toolbar: Settings > Toolbars
  Shown > Main Toolbar.

## Editing shortcuts

Tools > Markdown holds these actions. They work in Markdown documents only.

| Action | Shortcut | What it does |
|---|---|---|
| Bold | <kbd>Ctrl</kbd>+<kbd>Alt</kbd>+<kbd>B</kbd> | Wraps the selection or the word under the cursor in `**`, and takes the marker off when it is already there |
| Italic | <kbd>Ctrl</kbd>+<kbd>Alt</kbd>+<kbd>E</kbd> | The same with `*` |
| Strikethrough | <kbd>Ctrl</kbd>+<kbd>Alt</kbd>+<kbd>S</kbd> | The same with `~~` |
| Increase Heading Level | <kbd>Ctrl</kbd>+<kbd>Alt</kbd>+<kbd>=</kbd> | Adds one heading level to the selected lines or to the line of the cursor |
| Decrease Heading Level | <kbd>Ctrl</kbd>+<kbd>Alt</kbd>+<kbd>-</kbd> | Removes one |
| Paste as Link | <kbd>Ctrl</kbd>+<kbd>Shift</kbd>+<kbd>V</kbd> | Turns the selection into a link to the address in the clipboard |
| Format Table | <kbd>Ctrl</kbd>+<kbd>Alt</kbd>+<kbd>F</kbd> | Aligns the columns of the table under the cursor |

<kbd>Ctrl</kbd>+<kbd>B</kbd> and <kbd>Ctrl</kbd>+<kbd>I</kbd> are Kate's
bookmark and indentation shortcuts. To change any shortcut: Settings >
Configure Keyboard Shortcuts, search for Katemark.

On Windows the <kbd>Ctrl</kbd>+<kbd>Alt</kbd> shortcuts are not set: Windows
sends <kbd>AltGr</kbd> as <kbd>Ctrl</kbd>+<kbd>Alt</kbd>, so they would take
the euro sign or the `|` of a table. Give the actions a key of your own in
the same dialog.

## Settings

Settings > Configure Kate, then the **Katemark** page. It appears once you
enable the plugin.

| Setting | Values | What it does |
|---|---|---|
| Style | GitHub, Match editor / system theme | Match recolors the GitHub layout from your Kate theme |
| GitHub variant | Follow system (auto), Light, Dark | Applies in GitHub style only |
| Load media previews from remote URLs | Off (default), On | When off, the preview fetches nothing and says where a picture from the web was left out. Pictures stored beside the document load in both cases |
| Math macros | `\newcommand` lines | Macros available in the formulas of every document |
| Check for updates | Button | Compares the installed version with the latest release of this repository. Katemark contacts GitHub only when you click |

## Troubleshooting

| Problem | Cause and remedy |
|---|---|
| Katemark is not in the list of plugins | Check in Help > About Kate that Kate uses KDE Frameworks 6. After a user-local install, log out and back in |
| The preview stays empty or shows another document | The active document is not Markdown: see [First steps](../README.md#first-steps) |
| `apt` reports unmet dependencies | The package is for another system: see [Install](../README.md#install) |
| A picture from the web is missing | Enable "Load media previews from remote URLs" in the [settings](#settings) |
| <kbd>Ctrl</kbd>+<kbd>B</kbd> does not make bold | See [Editing shortcuts](#editing-shortcuts) |
| Kate's Output panel says `Failed to find server binary 'marksman'` | That message comes from Kate's LSP Client plugin, not from Katemark: it looks for a Markdown language server. Ignore it, install [marksman](https://github.com/artempyanykh/marksman) or turn that plugin off in Settings > Configure Kate > Plugins |
| The console warns about `Qt::AA_ShareOpenGLContexts` | Harmless: the plugin starts Qt WebEngine once Kate is running |
