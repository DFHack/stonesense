:orphan:

.. _stonesense:

##########
Stonesense
##########

.. Note to editors:  this document is rendered as part of the DFHack docs,
.. using Sphinx.  Some of it may not render properly on Github; that's OK.


Stonesense is an isometric visualizer for Dwarf Fortress,
implemented as `a DFHack plugin <plugin-stonesense>`.

Useful links:

- :forums:`Official Stonesense thread <106497>` for feedback,
  questions, requests or bug reports
- :forums:`Screenshots thread <48172>`
- :wiki:`Main wiki page <Utility:Stonesense>`
- :wiki:`How to add content <Utility:Stonesense/Adding_Content>`
- `Stonesense on Github <https://github.com/DFHack/stonesense>`_

Usage
=====
The DFHack command ``stonesense`` (or ``ssense``) opens Stonesense
in a new window.  This viewer window has read-only access to the game,
and can follow the game view, or be moved independently.

``stonesense overlay`` replaces the map section of your DF window.
Note that it's not (yet) suitable for use as your only interface.
Use DF's ``[PRINT_MODE:2D]`` init option for stability.
For best results, edit ``[segmentsize_z:2]`` in ``init.txt``, and if
you are unable to see the edges of the map with the overlay active
try decreasing the ``[segmentsize_XY]`` - normal values are 50 to 80
depending on your screen resolution.

.. figure:: ssense-roadtruss.jpg
   :align: center
   :target: http://www.bay12forums.com/smf/index.php?topic=48172.msg3198664#msg3198664

   The above-ground part of the fortress *Roadtruss*.

Configuration options are documented and can be pre-set in
:download:`../resources/init.txt`.

Controls
--------
Mouse controls are hard-coded and cannot be changed.

:Left click:    Move debug cursor (if available)
:Right click:   Recenter screen
:Scrollwheel:   Move up and down
:Ctrl-Scroll:   Increase/decrease Z depth shown

Follow mode makes the Stonesense view follow the location of the DF
window.  The offset can be adjusted by holding :kbd:`Ctrl` while using the
keyboard window movement keys.  When you turn on cursor follow mode, the
Stonesense debug cursor will follow the DF cursor when the latter exists.

You can take screenshots with :kbd:`F5`, larger screenshots with
:kbd:`Ctrl`:kbd:`F5`, and screenshot the whole map at full resolution with
:kbd:`Ctrl`:kbd:`Shift`:kbd:`F5`.  Screenshots are saved to the DF directory.
Note that feedback is printed to the DFHack console, and you may need
to zoom out before taking very large screenshots.

See :download:`../resources/keybinds.txt` to learn or set keybindings,
including zooming, changing the dimensions of the rendered area, toggling
various views, fog, and rotation.  Here's the important section:

.. include:: ../resources/keybinds.txt
   :literal:
   :end-before: VALID ACTIONS:


Known Issues
============
If Stonesense gives an error saying that it can't load
:file:`creatures/large_256/*.png`, your video card cannot handle the high
detail sprites used. Either open :file:`creatures/init.txt` and remove the
line containing that folder, or :dffd:`use these smaller sprites <6096>`.

Stonesense requires working graphics acceleration, and we recommend
at least a dual core CPU to avoid slowing down your game of DF.
