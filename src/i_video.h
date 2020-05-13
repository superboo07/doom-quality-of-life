/*
========================================================================

                           D O O M  R e t r o
         The classic, refined DOOM source port. For Windows PC.

========================================================================

  Copyright © 1993-2012 by id Software LLC, a ZeniMax Media company.
  Copyright © 2013-2020 by Brad Harding.

  DOOM Retro is a fork of Chocolate DOOM. For a list of credits, see
  <https://github.com/bradharding/doomretro/wiki/CREDITS>.

  This file is a part of DOOM Retro.

  DOOM Retro is free software: you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation, either version 3 of the License, or (at your
  option) any later version.

  DOOM Retro is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with DOOM Retro. If not, see <https://www.gnu.org/licenses/>.

  DOOM is a registered trademark of id Software LLC, a ZeniMax Media
  company, in the US and/or other countries, and is used without
  permission. All other trademarks are the property of their respective
  holders. DOOM Retro is in no way affiliated with nor endorsed by
  id Software.

========================================================================
*/

#if !defined(__I_VIDEO_H__)
#define __I_VIDEO_H__

#include "SDL.h"

#include "doomtype.h"

#define MAX_MOUSE_BUTTONS   8

#define GAMMALEVELS         31

void I_InitKeyboard(void);
void I_ShutdownKeyboard(void);
dboolean GetCapsLockState(void);

// Called by D_DoomMain,
// determines the hardware configuration
// and sets up the video mode
void I_InitGraphics(void);
void I_RestartGraphics(void);
void I_ShutdownGraphics(void);
void I_CapFPS(int cap);

void GetWindowPosition(void);
void GetWindowSize(void);
void GetScreenResolution(void);

// Takes full 8 bit values.
void I_SetPalette(byte *playpal);
void I_SetExternalAutomapPalette(void);
void I_SetSimplePalette(byte *playpal);
void I_SetPaletteWithBrightness(byte *playpal, double brightness);

void I_UpdateBlitFunc(dboolean shake);
void I_CreateExternalAutomap(int outputlevel);
void I_DestroyExternalAutomap(void);

void I_ToggleFullscreen(void);
void I_SetPillarboxes(void);
void I_SetMotionBlur(int percent);

void M_QuitDOOM(int choice);
void R_SetViewSize(int blocks);

void I_SetGamma(float value);

#if defined(_WIN32)
void I_WindowResizeBlit(void);
#endif

void (*blitfunc)(void);
void (*mapblitfunc)(void);

extern dboolean     sendpause;
extern dboolean     quitting;

extern int          keydown;

extern dboolean     idclev;
extern dboolean     idmus;
extern dboolean     idbehold;

extern int          gammaindex;
extern const float  gammalevels[GAMMALEVELS];

extern int          windowx;
extern int          windowy;
extern int          windowheight;
extern int          windowwidth;

extern dboolean     windowfocused;

extern SDL_Window   *window;
extern SDL_Renderer *renderer;

extern SDL_Window   *mapwindow;
extern SDL_Renderer *maprenderer;
extern byte         *mapscreen;

extern byte         *PLAYPAL;

extern dboolean     returntowidescreen;
extern dboolean     altdown;

#endif
