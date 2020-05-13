<br>

🚧 Under construction. If you'd like to contribute to this page, please [let me know](mailto:brad@doomretro.com).

<br>
<br>
<br>

<a name="DEHACKED-LUMPS"></a>
![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/dehacked-lumps.png?raw=true)

*DOOM Retro* fully supports both [*DeHackEd*](https://doomwiki.org/wiki/DeHackEd) and [*DeHackEd with BOOM extensions*](http://doomwiki.org/wiki/BEX) patches, in the form of either a <code><b>.deh</b></code> or <code><b>.bex</b></code> file, or a <code><b>DEHACKED</b></code> lump in a PWAD file.

*DeHackEd* support has been extended to allow for an additional 2,910 states (numbered 1,089 to 3,999), 100 additional things (numbered 150 to 249) and 100 additional sprites (numbered 145 to 244). Examples of this extended support being put to great use include the [*SMOOTHED*](https://www.doomworld.com/forum/topic/85991/) and [*Black Ops*](https://www.doomworld.com/forum/topic/84859/) mods by VGA.

The following additional entries are supported in <code><b>Thing</b></code> blocks:

&nbsp; | ![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/entry.png?raw=true) | ![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/description.png?raw=true)
:--- | :--- | :---
<b>1.</b> | <code><b>Gib&nbsp;health</b></code> | -
<b>2.</b> | <code><b>Dropped&nbsp;item</b></code> | -
<b>3.</b> | <code><b>Pickup&nbsp;width</b></code> | -
<b>4.</b> | <code><b>Projectile&nbsp;pass&nbsp;height</b></code> | -
<b>5.</b> | <code><b>Retro&nbsp;bits</b></code> | Can be set to one or more of the following flags: <code><b>TRANSLUCENT</b></code>, <code><b>TRANSLUCENT_REDONLY</b></code>, <code><b>TRANSLUCENT_GREENONLY</b></code>, <code><b>TRANSLUCENT_BLUEONLY</b></code>, <code><b>TRANSLUCENT_33</b></code>, <code><b>TRANSLUCENT_50</b></code>, <code><b>TRANSLUCENT_REDWHITEONLY</b></code>, <code><b>TRANSLUCENT_REDTOGREEN_33</b></code>, <code><b>TRANSLUCENT_REDTOBLUE_33</b></code>, <code><b>TRANSLUCENT_BLUE_25</b></code>, <code><b>TRANSLUCENT</b></code>, <code><b>GREENTORED</b></code>, <code><b>REDTOBLUE</b></code>, <code><b>FLOATBOB</b></code>, <code><b>MIRRORED</b></code>, <code><b>FALLING</b></code>, <code><b>ONMOBJ</b></code>, <code><b>PASSMOBJ</b></code>, <code><b>RESURRECTING</b></code>, <code><b>FOOTCLIP</b></code>, <code><b>NOLIQUIDBOB</b></code>, <code><b>FEETARECLIPPED</b></code>, <code><b>CASTSHADOW</b></code>, <code><b>BLOOD</b></code>, <code><b>DONTMAP</b></code>, <code><b>SMOKETRAIL</b></code>, <code><b>CRUSHABLE</b></code>, <code><b>MASSACRE</b></code>, <code><b>DECORATION</b></code>, <code><b>DONTDRAW</b></code> and <code><b>MONSTERMISSILE</b></code>
<b>6.</b> | <code><b>Fullbright</b></code> | -
<b>7.</b> | <code><b>Blood</b></code> | -
<b>8.</b> | <code><b>Shadow&nbsp;offset</b></code> | -

<br>
<br>
<br>

<a name="MAPINFO-LUMPS"></a>
![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/mapinfo-lumps.png?raw=true)

*DOOM Retro* partially supports <code><b>MAPINFO</b></code> lumps. They need to be in the older format first introduced in *Hexen*, rather than the newer format used by the *ZDoom* family of source ports. These lumps can also be named <code><b>RMAPINFO</b></code> and can contain some entries specific to *DOOM Retro’s* features.

Each entry must begin with <code><b>MAP</b> <i>xy</i>|<b>E</b><i>x</i><b>M</b><i>y</i>|<b>MAP</b><i>xy</i> <b>"</b><i>mapname</i><b>"</b></code> and then one or more of the following:

&nbsp; | ![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/entry.png?raw=true) | ![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/description.png?raw=true)
:--- | :--- | :---
<b>1.</b> | <code><b>AUTHOR</b> <i>string</i></code> | -
<b>2.</b> | <code><b>LIQUID</b> <i>flatname</i></code> | -
<b>3.</b> | <code><b>MUSIC</b> <i>lumpname</i></code> | -
<b>4.</b> | <code><b>MUSICCOMPOSER</b> <i>string</i></code> | -
<b>5.</b> | <code><b>MUSICTITLE</b> <i>string</i></code> | -
<b>6.</b> | <code><b>NEXT</b> <i>xy</i>&#124;<b>E</b><i>x</i><b>M</b><i>y</i>&#124;<b>MAP</b><i>xy</i></code> | -
<b>7.</b> | <code><b>NOBRIGHTMAP</b> <i>texturename</i></code> | -
<b>8.</b> | <code><b>NOJUMP</b></code> | -
<b>9.</b> | <code><b>NOLIQUID</b> <i>flatname</i></code> | -
<b>10.</b> | <code><b>NOMOUSELOOK</b></code> or <code><b>NOFREELOOK</b></code> | -
<b>11.</b> | <code><b>PAR</b> <i>partime</i></code> | -
<b>12.</b> | <code><b>PISTOLSTART</b></code> | -
<b>13.</b> | <code><b>SECRETNEXT</b> <i>xy</i>&#124;<b>E</b><i>x</i><b>M</b><i>y</i>&#124;<b>MAP</b><i>xy</i></code> | -
<b>14.</b> | <code><b>SKY1</b> <i>texturename</i> <i>scrolldelta</i></code> | -
<b>15.</b> | <code><b>TITLEPATCH</b> <i>patchname</i></code> | -

<br>
<br>
<br>

![](https://github.com/bradharding/www.doomretro.com/blob/master/wiki/bigdivider.png?raw=true)
