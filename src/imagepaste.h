// imagepaste.h
// Paste a clipboard image into a Markdown document.
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
 * Kate's own paste ignores a clipboard that only holds an image. For a Markdown document,
 * save that image beside the document and insert a link to it at the cursor.
 *
 * Returns true when a link was inserted. When the clipboard held an image that could not
 * be pasted, returns false and sets @p error to a message for the user.
 */
bool pasteClipboardImage(KTextEditor::View *view, QString *error);
