# Installing Katemark

## Debian package

Download the file for your system from the [latest
release](https://github.com/obook/katemark/releases/latest), then:

```bash
sudo apt install ./katemark_*_amd64.deb
```

`sudo apt remove katemark` uninstalls it. Each package installs only on the
system named in its file, because KDE neon and Ubuntu name their Qt and KDE
Frameworks packages differently.

## Windows

Download `katemark-<version>-windows-x86_64.zip` from the [latest
release](https://github.com/obook/katemark/releases/latest), unzip it, close
Kate and run this in an **elevated** PowerShell:

```powershell
powershell -ExecutionPolicy Bypass -File install.ps1
```

It finds Kate on its own, or takes `-KateDir "D:\Kate"`. `-WhatIf` shows
what it would do, `-Uninstall` removes exactly what it installed.

The zip is about 90 MB and unpacks to 220 MB: Kate for Windows ships no Qt
WebEngine, so the runtime comes with the plugin. It installs next to
`kate.exe`, where Windows looks for the libraries a plugin needs.

- Kate has to come from the [installer](https://kate-editor.org/get-it/).
  Katemark does not support the Microsoft Store version.
- The build targets the Qt of Kate's own Windows build, 6.12 in November
  2026. `install.ps1` refuses to install on another version.
- A workflow builds the zip. One user runs it on Windows 11, where the
  preview and both exports work: please report what you see on yours.

## Arch Linux

Install [`katemark-git`](https://aur.archlinux.org/packages/katemark-git)
from the AUR:

```bash
git clone https://aur.archlinux.org/katemark-git.git
cd katemark-git
makepkg -si
```

`sudo pacman -R katemark-git` uninstalls it.

## openSUSE

Katemark is in the `KDE:Extra` repository. On Tumbleweed:

```bash
sudo zypper addrepo https://download.opensuse.org/repositories/KDE:/Extra/openSUSE_Tumbleweed/KDE:Extra.repo
sudo zypper refresh
sudo zypper install katemark
```

On Leap 16.0, write `openSUSE_Leap_16.0` instead of `openSUSE_Tumbleweed`.
`sudo zypper remove katemark` uninstalls it.

## Build from source

Install the build dependencies:

```bash
# KDE neon
sudo apt install g++ cmake extra-cmake-modules gettext dpkg-dev qt6-webengine-dev \
    kf6-ktexteditor-dev kf6-kcoreaddons-dev kf6-ki18n-dev kf6-kconfig-dev \
    kf6-kxmlgui-dev kf6-kwidgetsaddons-dev kf6-syntax-highlighting-dev

# Ubuntu 26.04
sudo apt install g++ cmake extra-cmake-modules gettext dpkg-dev qt6-webengine-dev \
    libkf6texteditor-dev libkf6coreaddons-dev libkf6i18n-dev libkf6config-dev \
    libkf6xmlgui-dev libkf6widgetsaddons-dev libkf6syntaxhighlighting-dev

# Arch Linux
sudo pacman -S --needed base-devel cmake extra-cmake-modules \
    ktexteditor qt6-webengine kcoreaddons ki18n kconfig kxmlgui kwidgetsaddons \
    syntax-highlighting
```

Get the sources:

```bash
git clone https://github.com/obook/katemark.git
cd katemark
```

Then choose one of three ways to install.

### As a Debian package

On Debian, Ubuntu and KDE neon. `build.sh` configures, compiles, runs the
tests and makes the package:

```bash
./build.sh
sudo apt install ./build/katemark_*.deb
```

### System-wide install

On any distribution:

```bash
cmake -B build -S . -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCMAKE_INSTALL_PREFIX=/usr
cmake --build build
sudo cmake --install build
```

### User-local install

Without root. Configure and build as above, then:

```bash
cmake --install build --prefix ~/.local
mkdir -p ~/.config/environment.d
plugins=$(grep -om1 '.*/plugins' build/install_manifest.txt)
printf 'QT_PLUGIN_PATH=%s\n' "$plugins" > ~/.config/environment.d/katemark.conf
```

Log out and back in for Kate to find the plugin.

### Uninstall

From the source folder, with its `build` folder still there:

```bash
sudo cmake --build build --target uninstall   # system-wide install
cmake --build build --target uninstall        # user-local install
```

After a user-local install, also delete
`~/.config/environment.d/katemark.conf`.
