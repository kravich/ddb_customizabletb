Customizable toolbar plugin for DeaDBeeF music player
=====================================================
This plugin provides toolbar widget which intended to be used instead of default static DeadBeef's toolbar.
It allows to add any action on toolbar as a button, put it in required position and assign custom icon to it.
Or it could be used to simply rearrange default stop/play/pause/prev/next actions in a preferrable order.

Installation using precompiled binaries
---------------------------------------
1. Get zip archive for your architecture:
    x86:    https://yadi.sk/d/XzCszqRNgqHJ7
    x86_64: https://yadi.sk/d/QoR1uMJFgqHJb
2. Extract two .so's from archive and put them in ~/.local/lib/deadbeef

Building and installing from source
-----------------------------------
First, intall development packages for gtk2 and gtk3 (libgtk2.0-dev and libgtk-3-dev in Debian/Ubuntu).
Then,
```
git clone https://github.com/kravich/ddb_customizabletb.git
cd ddb_customizabletb
make
mkdir -p ~/.local/lib/deadbeef
cp ddb_customizabletb*.so ~/.local/lib/deadbeef
```

Usage
-----
1. Put DeadBeef in design mode (View -> Design mode)
2. Right click on a default toolbar -> Replace with... -> Customizable toolbar
   New toolbar will appear on a place of default toolbar
3. Right click on a new tollbar -> Uncheck "Expand" and "Fill" options to make toolbar to display properly
4. To manage actions of toolbar, while being in design mode, right click on it and select "Customize"

Todo
----
* Improve icon selection dialog to allow selection of arbitrary icon, not only the one from predefined list
* Add "Reset to default" button in customization dialog
* Add translations
* Add option to select arbitrary size of toolbar buttons
