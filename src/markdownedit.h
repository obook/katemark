// markdownedit.h
// Editing helpers for Markdown documents: inline markers, heading levels, links, tables.
// Author: Olivier Booklage
// Date: October 2026
// License: GPL-3.0-or-later

#pragma once

#include <QString>

namespace KTextEditor
{
class View;
}

/**
 * Wrap the selection, or the word under the cursor, in an inline marker such as "**",
 * or take the marker off when the text already has it.
 */
void toggleInlineMarker(KTextEditor::View *view, const QString &marker);

/**
 * Add (@p delta 1) or remove (@p delta -1) one heading level on the selected lines, or
 * on the line of the cursor.
 */
void shiftHeadingLevel(KTextEditor::View *view, int delta);

/**
 * Turn the selection into a link to the address held by the clipboard.
 * Returns false, and changes nothing, when the clipboard does not hold an address.
 */
bool pasteClipboardLink(KTextEditor::View *view);

/**
 * Align the columns of the table the cursor is in.
 * Returns false when the cursor is not in a table.
 */
bool formatTableAtCursor(KTextEditor::View *view);
