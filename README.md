Welcome to Stonesense
=====================

Stonesense is a third party visualizer that lets you view your Dwarf Fortress world in a classic isometric perspective.

Compatibility
-------------
All versions of Stonesense since DF2010 must be compiled with the appropriate DFHack version.  Stonesense is thus usually distributed with DFHack, and will not work with any other version.  

Usage
-----
To run Stonesense in a separate window, use the DFHack command `stonesense`, alias `ssense`.

You can try the in-game overlay with `stonesense overlay`; be aware it's incomplete and not yet suitable for use as your only interface.

Configuration options can be pre-set and are documented in the `init.txt` file.  Many can be toggled by the controls listed in `keybinds.txt`.

Controls
--------

**Mouse**

Mouse controls are hard-coded and cannot be changed.

    Left click:    Move debug cursor (if available)
    Right click:   Recenter screen
    Scrollwheel:   Move up and down 
    Ctrl-Scroll:   Increase/decrease Z depth shown

**Keyboard**

Check `keybinds.txt` for the configured key bindings and descriptions, including zooming , changing the dimensions of the rendered area, toggling various views, fog, and rotation.

**Follow mode**

Follow mode makes the Stonesense view follow the location of the DF window.  The offset can be adjusted by holding `Ctrl` while using the keyboard window movement keys.  When you turn on cursor follow mode, the Stonesense debug cursor will follow the DF cursor when the latter exists.

Contacts
--------

* [Stonesense on Bay12Games forums](http://www.bay12forums.com/smf/index.php?topic=106497)
* [Stonesense mailing list](http://groups.google.com/group/stonesense)

Known Issues
------------
See the issue tracker on GitHub for recent bugs and issues.

* If Stonesense gives an error saying that it can't load `creatures/large_256/*.png`, your video card cannot handle the high detail sprites used. Either open `creatures/init.txt` and remove the line containing that folder, or [use these smaller sprites](http://dffd.wimbli.com/file.php?id=6096).
