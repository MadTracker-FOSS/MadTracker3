This is the source code to MadTracker 3, a continuation of the MadTracker
project by Yannick Delwiche.

About MadTracker
----------------
MadTracker is a music composition tool based on
[tracker-sequencer](https://en.wikipedia.org/wiki/Music_tracker)
concept with full VST™, ASIO™ and ReWire™ support. MadTracker was,
originally, started as a closed source project and is still available
at its [official  website](http://www.madtracker.org/about.php).

**Note: Project is in early stages of development and is guaranteed
to be full of bugs and warts.** (see Roadmap below.)

Compiling
------
```shell
cmake .
make
```

Roadmap
-------
- Support for Linux, Mac
    - OpenAL device in MTAudio
    - SDL device in MTDisplay
    - GTK/wxWidgets implementation of MTGUI
- Cross-architecture support
    - Replace ASM routines with pure C


License
-------
MadTracker is released under the GNU LGPLv2. See LICENSE for details.

