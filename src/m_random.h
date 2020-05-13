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

#if !defined(__M_RANDOM_H__)
#define __M_RANDOM_H__

static unsigned int seed;

#define FASTRAND    ((seed = 214013 * seed + 2531011) >> 16)

static inline int M_Random(void)
{
    return (FASTRAND & 255);
}

static inline int M_SubRandom(void)
{
    return ((FASTRAND & 510) - 255);
}

static inline int M_RandomInt(int lower, int upper)
{
    return (FASTRAND % (upper - lower + 1) + lower);
}

static inline int M_RandomIntNoRepeat(int lower, int upper, int previous)
{
    int randomint;

    while ((randomint = (FASTRAND % (upper - lower + 1) + lower)) == previous);

    return randomint;
}

static inline void M_Seed(unsigned int value)
{
    seed = value;
}

#endif
