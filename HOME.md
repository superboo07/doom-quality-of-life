![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/introduction.png?raw=true)
<br>
<br>

*DOOM Retro* is the classic, refined *DOOM* source port for Windows PC. It represents how I like my *DOOM* to be today, in all its dark, gritty, unapologetically pixelated glory. I’ve strived to craft a set of features that uphold a certain level of attention to detail, and while still respecting that classic, nostalgic *DOOM* experience many of us hold dear.

*DOOM Retro* has been under relentless, meticulous development since its debut on Tuesday, December 10, 2013 to commemorate *DOOM’s* 20th anniversary, and has absolutely no intention of stopping. Its [source code](https://github.com/bradharding/doomretro) was originally forked from [*Chocolate DOOM*](https://github.com/chocolate-doom/chocolate-doom) but is now very much its own beast. It includes enhancements from other *DOOM* source ports as well as many original ideas.

*DOOM Retro* is intentionally minimalistic, and does a few things differently if compared with other modern *DOOM* source ports. It supports vanilla, [limit removing](https://doomwiki.org/wiki/Limit_removing), [*BOOM*](https://doomwiki.org/wiki/Boom) and [*MBF*](https://doomwiki.org/wiki/MBF)-compatible maps and mods. In order to freely implement certain features, and due to the nature of *DOOM* demos, *DOOM Retro* does not support their recording or playback.

*DOOM Retro* is single-player only, and runs under <a name="windowsxp"></a>*Windows XP*<sup>[1](#1)</sup> and above. Written in [*C*](https://en.wikipedia.org/wiki/C_(programming_language)) and released as open source under version 3 of the [*GNU General Public License*](https://github.com/bradharding/doomretro/wiki/LICENSE), *DOOM Retro’s* 81,200 lines of code are maintained in a public [*Git*](https://git-scm.com/) repository and compiled into both 32 and 64-bit *Windows* binaries using [*Microsoft Visual Studio Community 2019*](https://www.visualstudio.com/vs/). *DOOM Retro* may also be compiled and run under *Linux* and *macOS*.

The latest version of *DOOM Retro* may be downloaded [here](https://github.com/bradharding/doomretro/releases/latest).
<br>
<br>
<a name="FEATURES"></a> 
![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/features.png?raw=true)

- [x] A simple, convenient, built-in [WAD launcher](https://github.com/bradharding/doomretro/wiki/STARTING-A-GAME#USING-THE-WAD-LAUNCHER).
- [x] The use of version 2.0.12 of the [*SDL (Simple DirectMedia Layer)*](https://www.libsdl.org/) library.
- [x] A resolution of 640×400 (twice that of the original *DOOM*), accommodating for any screen size while maintaining the correct 4:3 aspect ratio.
- [x] A highly optimized 256-color software renderer, where each frame is scaled up onto the screen using hardware acceleration with either the [*OpenGL*](https://www.opengl.org/) or [*Direct3D*](https://docs.microsoft.com/en-au/windows/win32/getting-started-with-direct3d) APIs.
- [x] A 16:10 widescreen mode with a choice of two different heads-up displays.
- [x] Windowed and fullscreen modes, switchable at any time by pressing <kbd>ALT</kbd> + <kbd>ENTER</kbd>.
- [x] A powerful and unobtrusive [console](https://github.com/bradharding/doomretro/wiki/THE-CONSOLE) is always available by pressing the <kbd>~</kbd> key, where all messages are displayed and all settings may be changed, keeping the options menu uncluttered and consistent with the original *DOOM*.
- [x] The framerate is uncapped.
- [x] Motion blur can be applied when the player turns around quickly.
- [x] MUS, MIDI, MP3, Ogg Vorbis, WAV, FLAC and MOD music lumps are supported.
- [x] There are several alpha, additive and per-pixel translucency effects.
- [x] More blood is produced when shooting monsters.
- [x] All blood spilled leaves permanent splats on the floor.
- [x] Cacodemons have blue blood, hell knights and barons of hell have green blood, spectres have partially invisible blood, and lost souls have no blood.
- [x] Friendly monsters, including *MBF*-compatible “helper dogs”, may be spawned in a map.
- [x] Dynamic shadows are cast by monsters.
- [x] Liquid sectors bob up and down.
- [x] Objects are partially submerged and bob up and down when in liquid, and the player’s view is lowered.
- [x] Brightmaps have been applied to certain wall textures.
- [x] Corpses are randomly mirrored.
- [x] Corpses react to splash damage from nearby rockets and barrel explosions, and smear blood on the floor as they slide about.
- [x] The screen shakes when the player is injured or a nearby barrel explodes.
- [x] The player’s field of view can be changed.
- [x] There are 10 darker gamma correction levels to accommodate for today’s brighter LCD screens (with a darker gamma correction level of 0.90 set as the default).
- [x] The various limits of the original *DOOM* are either raised or removed completely.
- [x] There is support for [*DeHackEd*](https://doomwiki.org/wiki/DeHackEd) files and lumps, including those with [*BOOM* extensions](https://doomwiki.org/wiki/BEX).
- [x] *DeHackEd* support has been extended to allow for an additional 2,910 states (numbered 1,089 to 3,999), 100 additional map objects (numbered 150 to 249) and 100 additional sprites (numbered 145 to 244).
- [x] There is partial support for *Hexen*-style [<code><b>MAPINFO</b></code>](https://doomwiki.org/wiki/MAPINFO) lumps.
- [x] There is specific support for [*SIGIL*](https://www.romerogames.ie/si6il) by John Romero (which is automatically loaded if found).
- [x] The maps [*E1M4B: Phobos Mission Control*](https://www.doomworld.com/idgames/levels/doom/Ports/d-f/e1m4b) and [*E1M8B: Tech Gone Bad*](https://www.doomworld.com/idgames/levels/doom/Ports/d-f/e1m8b) by John Romero are included with permission. (These can be played by entering <code><b>map E1M4B</b></code> or <code><b>map E1M8B</b></code> in the [console](https://github.com/bradharding/doomretro/wiki/THE-CONSOLE) when a *DOOM* or *The Ultimate DOOM* IWAD is loaded.)
- [x] There is specific support for the *BFG Editions* of *DOOM* and *DOOM II: Hell On Earth*, including Nerve Software’s *No Rest For The Living* expansion (which is automatically loaded if found).
- [x] There is also specific support for:
  - [x] [*Freedoom: Phase 1*](https://freedoom.github.io/) and [*Freedoom: Phase 2*](https://freedoom.github.io/),
  - [x] [*Chex Quest*](https://doomwiki.org/wiki/Chex_Quest),
  - [x] [*HacX: Twitch ‘n Kill*](http://www.drnostromo.com/hacx/).
- [x] Maps with regular nodes, extended *DeePBSP v4* nodes and extended, uncompressed *ZDoom* nodes are supported.
- [x] There are various rendering improvements, which include:
  - [x] The removal of many “firelines” and “slime trails”,
  - [x] The removal of “sparkles” along the bottom edge of doors,
  - [x] Better drawn floors and ceilings,
  - [x] Changes in height are drawn more accurately,
  - [x] Smoother and slightly darker lighting,
  - [x] A better and more consistent “fuzz” effect for spectres and the player’s weapon when they have a partial invisibility power-up.
- [x] The majority of [“bugs” from the original *DOOM*](https://doomwiki.org/wiki/Engine_bug) have been fixed.
- [x] A “vanilla” mode is available that allows you to easily compare the many but often subtle differences between *DOOM Retro* and the original *DOOM*.
- [x] Keyboard support is improved:
  - [x] The <kbd>W</kbd> / <kbd>A</kbd> / <kbd>S</kbd> / <kbd>D</kbd> keys are set by default,
  - [x] The <kbd>CAPSLOCK</kbd> key can be used to “always run”.
- [x] Support has been added for *DirectInput* and *XInput* gamepads:
  - [x] *XInput* gamepads vibrate when the player is injured, fires their weapon, or a barrel explodes nearby,
  - [x] Gamepads can be connected in the middle of a game.
- [x] There is improved mouse support:
  - [x] A higher mouse sensitivity is allowed,
  - [x] Vertical mouse movement is disabled by default,
  - [x] The mousewheel can be used to change weapons,
  - [x] A mouselook feature where the vertical position of the player’s weapon is updated dynamically.
- [x] Over 1,400 level-specific fixes for the official *DOOM* and *DOOM II* WADs have been implemented, which include:
  - [x] Missing textures that cause floors and ceilings to “bleed”,
  - [x] Incorrect textures,
  - [x] Misaligned textures,
  - [x] Monsters that are stuck in walls or each other.
- [x] Extensive [player](https://github.com/bradharding/doomretro/wiki/PLAYER-STATS) and [map stats](https://github.com/bradharding/doomretro/wiki/MAP-STATS) are available.
- [x] Significant changes have been made to the [automap](https://github.com/bradharding/doomretro/wiki/THE-AUTOMAP):
  - [x] The automap updates dynamically as the player moves about the map,
  - [x] The automap may be shown on an [external display](https://github.com/bradharding/doomretro/wiki/THE-AUTOMAP#THE-EXTERNAL-AUTOMAP),
  - [x] A rotate mode can be toggled on or off using the <kbd>R</kbd> key, 
  - [x] Thing triangles are scaled based on the sizes of the things they represent,
  - [x] Solid walls are twice as wide and are drawn over other lines,
  - [x] Secrets are drawn correctly as they are discovered by the player,
  - [x] The [path](https://github.com/bradharding/doomretro/wiki/THE-AUTOMAP#THE-PLAYERS-PATH) the player takes around a map can be displayed.
- [x] Several improvements have been made to the menu and message systems.
- [x] Screenshots have been enhanced:
  - [x] Screenshots are saved as PNGs instead of the now obsolete PCX format,
  - [x] Screenshots are the same size as the screen,
  - [x] There is no limit to the number of screenshots that can be taken.
- [x] Several improvements have been made to the monsters’ animations.

<br>

<a name="SCREENSHOTS"></a> 
![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/screenshots.png?raw=true)
![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/screenshot1.png?raw=true)
![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/screenshot2.png?raw=true)
![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/screenshot3.png?raw=true)
![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/screenshot4.png?raw=true)
<br>
<br>
<br>
![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/bigdivider.png?raw=true)

<a name="1"></a><sup>[1](#windowsxp)</sup>*Windows XP* requires *Service Pack 3*.