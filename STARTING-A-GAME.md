<br>

As with all *DOOM* source ports, *DOOM Retro* doesn’t come with the content you need to actually play *DOOM*. Before you can start using *DOOM Retro*, this content (in the form of an “IWAD file”) first needs to be obtained by purchasing an official, commercial release of *DOOM* or *DOOM II* by id Software, such as from [*Steam*](http://store.steampowered.com/sub/18397/), [*GOG.com*](http://www.gog.com/game/the_ultimate_doom) or [*Bethesda.net*](https://bethesda.net/en/store/product/DO1GNGPCBG01). Once you have such a version of *DOOM* installed on your PC, you then need to let *DOOM Retro* know where this content is. You can do this by using either the [WAD launcher](#using-the-wad-launcher) or the [command-line](#using-the-command-line).

<br>
<br>
<br>

<a name="USING-THE-WAD-LAUNCHER"></a>
![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/using-the-wad-launcher.png?raw=true)

![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/wadlauncher.png?raw=true)

The easiest way to start a game in *DOOM Retro* is to double-click on <code><b>![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/icon.png?raw=true) doomretro.exe</b></code>. A dialog box will then appear asking you “where’s all the data”. *DOOM Retro* will do its best to find where *DOOM* or *DOOM II: Hell On Earth* has been installed, but if it doesn’t find them, navigate to the correct folder. Once there, select one of the following IWADs that *DOOM Retro* supports:

![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/iwad.png?raw=true)|![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/game.png?raw=true)
:--- | :---
<code><b>doom1.wad</b></code>             |*DOOM (Shareware)*
<code><b>doom.wad</b></code>              |*DOOM* or *The Ultimate DOOM*
<code><b>doom2.wad</b></code>             |*DOOM II: Hell On Earth*
<code><b>plutonia.wad</b></code>          |*Final DOOM: The Plutonia Experiment*
<code><b>tnt.wad</b></code>               |*Final DOOM: TNT – Evilution*

*DOOM Retro* also supports the countless number of third-party WADs — called PWADs (or “Patch WADs”) — made by members of the *DOOM* community over the past 26 years. <a name="shareware"></a>All PWADs need one of the above IWADs in order to work.<sup>[1](#1)</sup> To open a PWAD, click on it, and then <kbd>CTRL</kbd>-click on the IWAD from the list above that it was designed for. With the two WAD files selected, click the “Open” button.

If a PWAD is selected on its own, *DOOM Retro* will try to automatically open the IWAD it requires if both WADs are in the same folder, or if *DOOM Retro* has previously been shown where the IWAD is.

The next time *DOOM Retro* is run, the folder that you last navigated to will open again.

<br>
<br>
<br>

<a name="USING-THE-COMMAND-LINE"></a>
![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/using-the-command-line.png?raw=true)

Alternatively, you may want to create shortcuts for each WAD in your collection. To do this, right-click on <code><b>![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/icon.png?raw=true) doomretro.exe</b></code> and then click “Create shortcut”. A new file called <code><b>![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/icon.png?raw=true) doomretro.exe - Shortcut.lnk</b></code> will be created. Right-click on its icon and then click “Properties”. At the end of the text in the “Target” field, add the <code><b>-iwad</b></code> [command-line parameter](https://github.com/bradharding/doomretro/wiki/COMMAND-LINE-PARAMETERS) followed by the full path of your chosen IWAD file. For example:
```
doomretro.exe -iwad C:\DOOM\doom.wad
```
To load a PWAD as well, use the <code><b>–file</b></code> command-line parameter, like in the following example:
```
doomretro.exe -iwad C:\DOOM2\doom2.wad -file C:\wads\brigandine.wad
```
Double-clicking on the shortcut’s icon will then load the WAD(s) with *DOOM Retro*.

<br>
<br>
<br>
<br>

![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/bigdivider.png?raw=true)

<a name="1"></a><sup>[1](#shareware)</sup>PWADs may not be loaded with *DOOM (Shareware)’s* IWAD.