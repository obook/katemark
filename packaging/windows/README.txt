Katemark for Kate on Windows
============================

Katemark is a Kate plugin that renders a GitHub-styled preview of the Markdown file you are
editing, in a panel beside the editor.


What is in here
---------------

  install.ps1   installs and uninstalls
  payload\      the plugin, plus the Qt WebEngine runtime it needs

Kate for Windows ships no Qt WebEngine, and the preview is a web view, so the runtime has to come
with the plugin. Windows resolves a plugin's dependencies from the folder holding kate.exe, which
is why those files land next to Kate rather than beside the plugin.


Requirements
------------

  * Kate for Windows from the installer at https://kate-editor.org/get-it/
  * Kate built on Qt 6.11.x. install.ps1 checks this and refuses otherwise, because a mismatch
    produces a plugin that never loads and never says why.

The Microsoft Store version of Kate cannot be used. Its install directory is locked down by
Windows, so nothing can add a plugin to it.


Installing
----------

Close Kate first, then in an ELEVATED PowerShell (Kate normally lives under Program Files):

    powershell -ExecutionPolicy Bypass -File install.ps1

It finds Kate on its own. If it cannot, or you have several installs, name one:

    powershell -ExecutionPolicy Bypass -File install.ps1 -KateDir "D:\Kate"

Add -WhatIf to see what it would do without touching anything.

Then start Kate and turn the plugin on: Settings, then Configure Kate, then Plugins, then check
Katemark. Open a Markdown file and press Ctrl+Shift+M.


Uninstalling
------------

    powershell -ExecutionPolicy Bypass -File install.ps1 -Uninstall

It removes exactly the files it installed, tracked in a manifest written at install time, and
leaves Kate's own files alone.


If the plugin does not appear in Kate's plugin list
---------------------------------------------------

That is what a failed load looks like: Kate does not report it. Check that
bin\kf6\ktexteditor\katemark.dll exists under your Kate install, that bin\Qt6WebEngineCore.dll is
there next to kate.exe, and that Kate's bin\Qt6Core.dll is version 6.11.x.


https://github.com/obook/katemark
