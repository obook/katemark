// media.js
// Says why a picture from the web is missing when the settings keep it out.
//
// Part of the preview page: adds its functions to window.katemark (see core.js).
// Author: Olivier Booklage
// Date: October 2026
// License: GPL-3.0-or-later
(function () {
  "use strict";

  var katemark = window.katemark;

  // Translated name of the setting that lets pictures from the web in. The host sends it
  // while that setting is off, and an empty text while it is on.
  var blockedHint = "";

  function setRemoteMediaHint(text) {
    blockedHint = text;
  }

  // A picture the settings keep out would show as a broken image, with nothing to tell
  // why. Put a note in its place: the name of the setting to turn on, and the address.
  function explainBlockedMedia(article) {
    if (!blockedHint) {
      return;
    }
    article.querySelectorAll("img").forEach(function (picture) {
      // "src" as a property is the full address, whatever the document wrote.
      if (!/^https?:/i.test(picture.src)) {
        return;
      }
      var settingName = document.createElement("em");
      settingName.textContent = blockedHint;
      var note = document.createElement("span");
      note.className = "blocked-media";
      note.appendChild(settingName);
      note.appendChild(document.createElement("br"));
      note.appendChild(document.createTextNode(picture.src));
      picture.replaceWith(note);
    });
  }

  katemark.setRemoteMediaHint = setRemoteMediaHint;
  katemark.explainBlockedMedia = explainBlockedMedia;
})();
