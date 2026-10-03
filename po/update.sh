#!/usr/bin/env bash
#
# Name: update.sh
# Description: Refresh po/katemark.pot from the sources, then merge it into
#              every translation. Run from the repository root.
# Author: Olivier Booklage
# Date: October 2026
# License: GPL-3.0-or-later
#
set -euo pipefail

# The -k options name the KDE translation functions and say which of their
# arguments is the text (and, with "c", which one is the context).
xgettext --from-code=UTF-8 -C --kde -ci18n \
    -ki18n:1 -ki18nc:1c,2 -ki18np:1,2 -ki18ncp:1c,2,3 \
    --package-name=katemark \
    --msgid-bugs-address=https://github.com/obook/katemark/issues \
    -o po/katemark.pot src/*.cpp src/*.h

for translation in po/*/katemark.po; do
    if [ -e "${translation}" ]; then
        msgmerge --quiet --update --backup=none "${translation}" po/katemark.pot
    fi
done
