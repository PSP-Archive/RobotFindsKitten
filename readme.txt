robotfindskitten for PSP - Yet another portable Zen simulation

v72617319.4p (or maybe just v1.0) (January 22, 2006)

PSP port by Trevor Wilson <jiji@namakoteam.com>
uses "pg" PSP graphics libraries from "hellopsp" by nem
Original version by Leonard Richardson
Official robotfindskitten site: http://robotfindskitten.org


Installation
------------
You'll need a PSP capable of playing unsigned code.  Copy EBOOT.PBP to your PSP'S /PSP/GAME directory, 
using whatever method you see fit. If you've got a PSP with v1.5 BIOS, copy the contents of the "1.5" 
directory there.  The game will show up under the "Game" tab, under "Memory Stick."

This has only been test on a PSP with version 1.5 BIOS.  I really have no idea how it would perform on other
BIOS versions.  If you've got a 1.0, 2.0, etc., and are able or unable to get it working, please let me know.


How to Play
-----------
You are robot.  Find kitten.  There are things lying around which are not kitten. Touch them to figure out
which is which.


About
-----
I built about half of this in a day or so...and finished it up in another day, several months later.
I started it as a slight break from another PSP project I was working on, and resumed it as an exercise 
to get my head back into that original project.  I started out using a double-sized, 16x16 font, but switched
to an 8x8 font when I realized that the hugely-expanded screen real estate really helped carry the game's feeling
of wandering aimlessly.  

Anyway, it should be relatively bug-free. Please contact me if you find any problems.


Compiling
---------
You'll need an installation of gcc (either under *nix or Cygwin) and PSPSDK.  Follow the instructions at the link
below to get started. After that, a simple 'make' executed in the source directory will get things moving.

http://wiki.pspdev.org/psp:programming_faq


Files included:

EBOOT.PBP        - robotfindskitten PSP executable
readme.txt       - This file.
src/robot.c      - Main program module, based on Leonard Richardson's DOS version and heavily modified by myself.
src/font.c       - Contains font in a binary data structure. Partly from nem's hellopsp; changed to use an old IBM font.
src/colortable.h - Defines color definitions for ANSI-display-like colors, written by myself.
src/messages.h   - Messages file, from Java robotfindskitten by Brian Enigma <enigma@netninja.com>. I added a few messages.
src/pg.c         - nem's pg PSP primitive graphics library, taken from his "hellopsp" demo and modified by myself.
src/pg.h         - "
src/ICON0.PNG    - robotfindskitten PSP menu icon, which is linked into EBOOT.PBP upon compilation.
src/Makefile     - Makefile for this PSP version.
1.5/*            - Files for running the game on a v1.5 PSP.


This software is released under the GNU General Public License.