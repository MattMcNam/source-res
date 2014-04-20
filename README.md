SourceRes
===========

About
-
SourceRes is a simple plugin for Source Engine games which allows you to run and record the game at custom resolutions, including resolutions larger than your desktop/monitor.  
This lets you record at, for example, 2160p on a 1080p monitor.  

Usage
-
`sr_forceres [width] [height]`  
Forces the game to switch to this width and height.  
  
Warning
-
I have not tested what happens in fullscreen.  
It will not work in fullscreen, but I don't know if it will put the game in windowed mode or try to force your monitor to that resolution (a bad thing!)
*Run in windowed mode!*  

Supported games
-
Currently, all Source 2013 games seem to be supported. The games I have tested are:  
- Team Fortress 2  
- Counter-Strike: Source  
- Portal 1  
- Half-Life 2  

The following are not working yet:  
- Counter-Strike: Global Offensive  

The following are games that are untested, but suspected not working:  
- Portal 1 (Pre-SteamPipe)  
- Half-Life 2 (Pre-SteamPipe)  
- DotA 2  
- Portal 2  
- Left4Dead 1 & 2  

Building
-
Requires source-sdk-2013.  
`cmake -DHL2SDK="c:\path\to\source-sdk-2013\mp\src" -G "Visual Studio 10" ..`  
Then open in Visual Studio/C++ Express and build.

License
-
SourceRes is provided under the BSD 2-Clause License, which is available in full in LICENSE.md
