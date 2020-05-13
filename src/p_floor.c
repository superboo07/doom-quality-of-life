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

#include "doomstat.h"
#include "m_config.h"
#include "p_fix.h"
#include "p_local.h"
#include "p_setup.h"
#include "p_tick.h"
#include "s_sound.h"
#include "z_zone.h"

#define FLOOR   0
#define CEILING 1

#define DOWN   -1
#define UP      1

//
// FLOORS
//

//
// Move a plane (floor or ceiling) and check for crushing
//
result_e T_MovePlane(sector_t *sector, fixed_t speed, fixed_t dest, dboolean crush,
    int floororceiling, int direction, dboolean elevator)
{
    fixed_t lastpos;
    fixed_t destheight;

    if (!elevator || floororceiling == FLOOR)
        sector->oldfloorheight = sector->floorheight;

    if (!elevator || floororceiling == CEILING)
        sector->oldceilingheight = sector->ceilingheight;

    sector->oldgametime = gametime;

    switch (floororceiling)
    {
        case FLOOR:
            lastpos = sector->floorheight;

            switch (direction)
            {
                case DOWN:
                    if (sector->floorheight - speed < dest)
                    {
                        sector->floorheight = dest;

                        if (P_ChangeSector(sector, crush))
                        {
                            sector->floorheight = lastpos;
                            P_ChangeSector(sector, crush);
                        }

                        return pastdest;
                    }
                    else
                    {
                        sector->floorheight -= speed;
                        P_ChangeSector(sector, crush);
                    }

                    break;

                case UP:
                    destheight = MIN(dest, sector->ceilingheight);

                    if (sector->floorheight + speed > destheight)
                    {
                        sector->floorheight = destheight;

                        if (P_ChangeSector(sector, crush))
                        {
                            sector->floorheight = lastpos;
                            P_ChangeSector(sector, crush);
                        }

                        return pastdest;
                    }
                    else
                    {
                        // COULD GET CRUSHED
                        sector->floorheight += speed;

                        if (P_ChangeSector(sector, crush))
                        {
                            sector->floorheight = lastpos;
                            P_ChangeSector(sector, crush);
                            return crushed;
                        }
                    }

                    break;
            }

            break;

        case CEILING:
            lastpos = sector->ceilingheight;

            switch (direction)
            {
                case DOWN:
                    destheight = MAX(dest, sector->floorheight);

                    if (sector->ceilingheight - speed < destheight)
                    {
                        sector->ceilingheight = destheight;

                        if (P_ChangeSector(sector, crush))
                        {
                            sector->ceilingheight = lastpos;
                            P_ChangeSector(sector, crush);
                        }

                        return pastdest;
                    }
                    else
                    {
                        // COULD GET CRUSHED
                        sector->ceilingheight -= speed;

                        if (P_ChangeSector(sector, crush))
                        {
                            if (crush)
                                return crushed;

                            sector->ceilingheight = lastpos;
                            P_ChangeSector(sector, crush);
                            return crushed;
                        }
                    }

                    break;

                case UP:
                    if (sector->ceilingheight + speed > dest)
                    {
                        sector->ceilingheight = dest;

                        if (P_ChangeSector(sector, crush))
                        {
                            sector->ceilingheight = lastpos;
                            P_ChangeSector(sector, crush);
                        }

                        return pastdest;
                    }
                    else
                    {
                        sector->ceilingheight += speed;
                        P_ChangeSector(sector, crush);
                    }

                    break;
            }

            break;
    }

    return ok;
}

//
// MOVE A FLOOR TO IT'S DESTINATION (UP OR DOWN)
//
// jff 02/08/98 all cases with labels beginning with gen added to support
// generalized line type behaviors.
void T_MoveFloor(floormove_t *floor)
{
    sector_t    *sec = floor->sector;
    result_e    res = T_MovePlane(sec, floor->speed, floor->floordestheight, floor->crush, 0, floor->direction, false);

    if (!(leveltime & 7)
        // [BH] don't make sound once floor is at its destination height
        && sec->floorheight != floor->floordestheight)
        S_StartSectorSound(&sec->soundorg, sfx_stnmov);

    if (res == pastdest)
    {
        if (floor->direction == UP)
        {
            switch (floor->type)
            {
                case donutRaise:
                case genFloorChgT:
                case genFloorChg0:
                    sec->special = floor->newspecial;
                    // fall through

                case genFloorChg:
                    sec->floorpic = floor->texture;
                    P_CheckTerrainType(sec);
                    break;

                default:
                    break;
            }
        }
        else if (floor->direction == DOWN)
        {
            switch (floor->type)
            {
                case lowerAndChange:
                case genFloorChgT:
                case genFloorChg0:
                    sec->special = floor->newspecial;
                    // fall through

                case genFloorChg:
                    sec->floorpic = floor->texture;
                    P_CheckTerrainType(sec);
                    break;

                default:
                    break;
            }
        }

        floor->sector->floordata = NULL;
        P_RemoveThinker(&floor->thinker);

        // jff 2/26/98 implement stair retrigger lockout while still building
        // note this only applies to the retriggerable generalized stairs
        if (sec->stairlock == -2)               // if this sector is stairlocked
        {
            sec->stairlock = -1;                // thinker done, promote lock to -1

            while (sec->prevsec != -1 && sectors[sec->prevsec].stairlock != -2)
                sec = sectors + sec->prevsec;   // search for a non-done thinker

            if (sec->prevsec == -1)             // if all thinkers previous are done
            {
                sec = floor->sector;            // search forward

                while (sec->nextsec != -1 && sectors[sec->nextsec].stairlock != -2)
                    sec = sectors + sec->nextsec;

                if (sec->nextsec == -1)         // if all thinkers ahead are done too
                {
                    while (sec->prevsec != -1)  // clear all locks
                    {
                        sec->stairlock = 0;
                        sec = sectors + sec->prevsec;
                    }

                    sec->stairlock = 0;
                }
            }
        }

        // [BH] don't make stop sound if floor already at its destination height
        if (floor->stopsound)
            S_StartSectorSound(&sec->soundorg, sfx_pstop);
    }
}

//
// T_MoveElevator()
//
// Move an elevator to it's destination (up or down)
// Called once per tic for each moving floor.
//
// Passed an elevator_t structure that contains all pertinent info about the
// move. See P_SPEC.H for fields.
// No return value.
//
// jff 02/22/98 added to support parallel floor/ceiling motion
//
void T_MoveElevator(elevator_t *elevator)
{
    result_e    res;

    if (elevator->direction < 0)                // moving down
    {
        // jff 4/7/98 reverse order of ceiling/floor
        res = T_MovePlane(elevator->sector, elevator->speed, elevator->ceilingdestheight, false, 1, elevator->direction, true);

        // jff 4/7/98 don't move ceil if blocked
        if (res == ok || res == pastdest)
            T_MovePlane(elevator->sector, elevator->speed, elevator->floordestheight, false, 0, elevator->direction, true);
    }
    else                                        // up
    {
        // jff 4/7/98 reverse order of ceiling/floor
        res = T_MovePlane(elevator->sector, elevator->speed, elevator->floordestheight, false, 0, elevator->direction, true);

        // jff 4/7/98 don't move floor if blocked
        if (res == ok || res == pastdest)
            T_MovePlane(elevator->sector, elevator->speed, elevator->ceilingdestheight, false, 1, elevator->direction, true);
    }

    // make floor move sound
    if (!(leveltime & 7))
        S_StartSectorSound(&elevator->sector->soundorg, sfx_stnmov);

    if (res == pastdest)                        // if destination height achieved
    {
        elevator->sector->floordata = NULL;
        elevator->sector->ceilingdata = NULL;
        P_RemoveThinker(&elevator->thinker);    // remove elevator from actives

        // make floor stop sound
        S_StartSectorSound(&elevator->sector->soundorg, sfx_pstop);
    }
}

//
// HANDLE FLOOR TYPES
//
dboolean EV_DoFloor(line_t *line, floor_e floortype)
{
    int         secnum = -1;
    dboolean    rtn = false;
    sector_t    *sec;

    if (P_ProcessNoTagLines(line, &sec, &secnum))
    {
        if (zerotag_manual)
            goto manual_floor;
        else
            return false;
    }

    while ((secnum = P_FindSectorFromLineTag(line, secnum)) >= 0)
    {
        floormove_t *floor;

        sec = sectors + secnum;

manual_floor:
        // ALREADY MOVING? IF SO, KEEP GOING...
        if (P_SectorActive(floor_special, sec))
        {
            if (!zerotag_manual)
                continue;
            else
                return rtn;
        }

        // new floor thinker
        rtn = true;
        floor = Z_Calloc(1, sizeof(*floor), PU_LEVSPEC, NULL);

        floor->thinker.function = T_MoveFloor;
        P_AddThinker(&floor->thinker);

        sec->floordata = floor;
        floor->type = floortype;

        switch (floortype)
        {
            case lowerFloor:
                floor->direction = DOWN;
                floor->sector = sec;
                floor->speed = FLOORSPEED;
                floor->floordestheight = P_FindHighestFloorSurrounding(sec);
                break;

            case lowerFloor24:
                floor->direction = DOWN;
                floor->sector = sec;
                floor->speed = FLOORSPEED;
                floor->floordestheight = floor->sector->floorheight + 24 * FRACUNIT;
                break;

            case lowerFloor32Turbo:
                floor->direction = DOWN;
                floor->sector = sec;
                floor->speed = FLOORSPEED * 4;
                floor->floordestheight = floor->sector->floorheight + 32 * FRACUNIT;
                break;

            case lowerFloorToLowest:
                floor->direction = DOWN;
                floor->sector = sec;
                floor->speed = FLOORSPEED;
                floor->floordestheight = P_FindLowestFloorSurrounding(sec);
                break;

            case lowerFloorToNearest:
                floor->direction = DOWN;
                floor->sector = sec;
                floor->speed = FLOORSPEED;
                floor->floordestheight = P_FindNextLowestFloor(sec, sec->floorheight);
                break;

            case turboLower:
                floor->direction = DOWN;
                floor->sector = sec;
                floor->speed = FLOORSPEED * 4;
                floor->floordestheight = P_FindHighestFloorSurrounding(sec);

                if (floor->floordestheight != sec->floorheight)
                    floor->floordestheight += 8 * FRACUNIT;

                break;

            case raiseFloorCrush:
                floor->crush = true;

            case raiseFloor:
                floor->direction = UP;
                floor->sector = sec;
                floor->speed = FLOORSPEED;
                floor->floordestheight = MIN(P_FindLowestCeilingSurrounding(sec), sec->ceilingheight)
                    - 8 * FRACUNIT * (floortype == raiseFloorCrush);
                break;

            case raiseFloorTurbo:
                floor->direction = UP;
                floor->sector = sec;
                floor->speed = FLOORSPEED * 4;
                floor->floordestheight = P_FindNextHighestFloor(sec, sec->floorheight);
                break;

            case raiseFloorToNearest:
                floor->direction = UP;
                floor->sector = sec;
                floor->speed = FLOORSPEED;
                floor->floordestheight = P_FindNextHighestFloor(sec, sec->floorheight);
                break;

            case raiseFloor24:
                floor->direction = UP;
                floor->sector = sec;
                floor->speed = FLOORSPEED;
                floor->floordestheight = sec->floorheight + 24 * FRACUNIT;
                break;

            case raiseFloor32Turbo:
                floor->direction = UP;
                floor->sector = sec;
                floor->speed = FLOORSPEED * 4;
                floor->floordestheight = sec->floorheight + 32 * FRACUNIT;
                break;

            case raiseFloor512:
                floor->direction = UP;
                floor->sector = sec;
                floor->speed = FLOORSPEED;
                floor->floordestheight = sec->floorheight + 512 * FRACUNIT;
                break;

            case raiseFloor24AndChange:
                floor->direction = UP;
                floor->sector = sec;
                floor->speed = FLOORSPEED;
                floor->floordestheight = sec->floorheight + 24 * FRACUNIT;

                if (E2M2)
                    sec->floorpic = R_FlatNumForName("FLOOR5_4");
                else if (MAP12)
                    sec->floorpic = R_FlatNumForName("FLOOR7_1");
                else
                    sec->floorpic = line->frontsector->floorpic;

                P_CheckTerrainType(sec);
                sec->special = line->frontsector->special;
                break;

            case raiseToTexture:
            {
                int minsize = 32000 << FRACBITS;

                floor->direction = UP;
                floor->sector = sec;
                floor->speed = FLOORSPEED;

                for (int i = 0; i < sec->linecount; i++)
                    if (twoSided(secnum, i))
                    {
                        side_t  *side = getSide(secnum, i, 0);

                        if (textureheight[side->bottomtexture] < minsize)
                            minsize = textureheight[side->bottomtexture];

                        side = getSide(secnum, i, 1);

                        if (textureheight[side->bottomtexture] < minsize)
                            minsize = textureheight[side->bottomtexture];
                    }

                floor->floordestheight = MIN((sec->floorheight >> FRACBITS) + (minsize >> FRACBITS), 32000) << FRACBITS;
                break;
            }

            case lowerAndChange:
                floor->direction = DOWN;
                floor->sector = sec;
                floor->speed = FLOORSPEED;
                floor->floordestheight = P_FindLowestFloorSurrounding(sec);
                floor->texture = sec->floorpic;
                floor->newspecial = sec->special;

                for (int i = 0; i < sec->linecount; i++)
                    if (twoSided(secnum, i))
                    {
                        if (getSide(secnum, i, 0)->sector->id == secnum)
                        {
                            if ((sec = getSector(secnum, i, 1))->floorheight == floor->floordestheight)
                            {
                                floor->texture = sec->floorpic;
                                floor->newspecial = sec->special;
                                break;
                            }
                        }
                        else
                        {
                            if ((sec = getSector(secnum, i, 0))->floorheight == floor->floordestheight)
                            {
                                floor->texture = sec->floorpic;
                                floor->newspecial = sec->special;
                                break;
                            }
                        }
                    }

            default:
                break;
        }

        floor->stopsound = (sec->floorheight != floor->floordestheight);

        // [BH] floor is no longer secret
        for (int i = 0; i < sec->linecount; i++)
            sec->lines[i]->flags &= ~ML_SECRET;

        if (zerotag_manual)
            return rtn; //e6y
    }

    return rtn;
}

void P_CheckTerrainType(sector_t *sector)
{
    terraintype_t   oldterraintype = sector->terraintype;

    if ((sector->terraintype = terraintypes[sector->floorpic]) != oldterraintype)
    {
        dboolean    isliquid = (sector->terraintype != SOLID);

        if (isliquid)
        {
            bloodsplat_t    *splat = sector->splatlist;

            while (splat)
            {
                bloodsplat_t    *next = splat->snext;

                P_UnsetBloodSplatPosition(splat);
                r_bloodsplats_total--;
                splat = next;
            }
        }
        else
        {
            sector->floor_xoffs = 0;
            sector->floor_yoffs = 0;
        }

        for (msecnode_t *node = sector->touching_thinglist; node; node = node->m_snext)
        {
            mobj_t  *thing = node->m_thing;

            if (isliquid && !(thing->flags & MF_SPAWNCEILING) && (thing->flags2 & MF2_FOOTCLIP))
                thing->flags2 |= MF2_FEETARECLIPPED;
            else
                thing->flags2 &= ~MF2_FEETARECLIPPED;
        }
    }
}

//
// EV_DoChange()
//
// Handle pure change types. These change floor texture and sector type
// by trigger or numeric model without moving the floor.
//
// The linedef causing the change and the type of change is passed
// Returns true if any sector changes
//
// jff 3/15/98 added to better support generalized sector types
//
dboolean EV_DoChange(line_t *line, change_e changetype)
{
    int         secnum = -1;
    dboolean    rtn = false;

    // change all sectors with the same tag as the linedef
    while ((secnum = P_FindSectorFromLineTag(line, secnum)) >= 0)
    {
        sector_t    *sec = sectors + secnum;
        sector_t    *secm;

        rtn = true;

        // handle trigger or numeric change type
        switch (changetype)
        {
            case trigChangeOnly:
                sec->floorpic = line->frontsector->floorpic;
                P_CheckTerrainType(sec);
                sec->special = line->frontsector->special;
                break;

            case numChangeOnly:
                if ((secm = P_FindModelFloorSector(sec->floorheight, secnum)))  // if no model, no change
                {
                    sec->floorpic = secm->floorpic;
                    P_CheckTerrainType(sec);
                    sec->special = secm->special;
                }

                break;
        }
    }

    return rtn;
}

//
// BUILD A STAIRCASE!
//

// cph 2001/09/21 - compatibility nightmares again
// There are three different ways this function has, during its history, stepped
// through all the stairs to be triggered by the single switch
// - original DOOM used a linear P_FindSectorFromLineTag, but failed to preserve
// the index of the previous sector found, so instead it would restart its
// linear search from the last sector of the previous staircase
// - MBF/PrBoom with comp_stairs fail to emulate this, because their
// P_FindSectorFromLineTag is a chained hash table implementation. Instead they
// start following the hash chain from the last sector of the previous
// staircase, which will (probably) have the wrong tag, so they miss any further
// stairs
// - BOOM fixed the bug, and MBF/PrBoom without comp_stairs work right
//
static int P_FindSectorFromLineTagWithLowerBound(line_t *l, int start, int min)
{
    do
    {
        start = P_FindSectorFromLineTag(l, start);
    } while (start >= 0 && start <= min);

    return start;
}

dboolean EV_BuildStairs(line_t *line, fixed_t speed, fixed_t stairsize, dboolean crushing)
{
    int         ssec = -1;
    int         minssec = -1;
    dboolean    rtn = false;
    int         secnum = -1;
    sector_t    *sec;

    if (P_ProcessNoTagLines(line, &sec, &secnum))
    {
        if (zerotag_manual)
            goto manual_stair;
        else
            return false;
    }

    while ((ssec = P_FindSectorFromLineTagWithLowerBound(line, ssec, minssec)) >= 0)
    {
        floormove_t *floor;
        dboolean    okay;
        int         height;
        int         texture;

        secnum = ssec;
        sec = sectors + secnum;

manual_stair:
        // ALREADY MOVING? IF SO, KEEP GOING...
        if (P_SectorActive(floor_special, sec))
            continue;

        // new floor thinker
        rtn = true;
        floor = Z_Calloc(1, sizeof(*floor), PU_LEVSPEC, NULL);

        floor->thinker.function = T_MoveFloor;
        P_AddThinker(&floor->thinker);

        sec->floordata = floor;
        floor->direction = UP;
        floor->sector = sec;

        floor->speed = speed;
        height = sec->floorheight + stairsize;
        floor->floordestheight = height;
        floor->crush = crushing;
        floor->stopsound = (sec->floorheight != height);

        texture = sec->floorpic;

        // Find next sector to raise
        // 1. Find 2-sided line with same sector side[0]
        // 2. Other side is the next sector to raise
        do
        {
            okay = false;

            for (int i = 0; i < sec->linecount; i++)
            {
                line_t      *li = sec->lines[i];
                sector_t    *tsec;

                if (!(li->flags & ML_TWOSIDED))
                    continue;

                if (secnum != li->frontsector->id)
                    continue;

                if (!(tsec = li->backsector))
                    continue;

                if (tsec->floorpic != texture)
                    continue;

                height += stairsize;

                if (P_SectorActive(floor_special, tsec))
                    continue;

                sec = tsec;
                secnum = tsec->id;
                floor = Z_Calloc(1, sizeof(*floor), PU_LEVSPEC, NULL);

                floor->thinker.function = T_MoveFloor;
                P_AddThinker(&floor->thinker);

                sec->floordata = floor;
                floor->direction = UP;
                floor->sector = sec;
                floor->speed = speed;
                floor->floordestheight = height;
                floor->crush = crushing;
                floor->stopsound = (sec->floorheight != height);
                okay = true;
                break;
            }
        } while (okay);
    }

    return rtn;
}

//
// EV_DoElevator
//
// Handle elevator linedef types
//
// Passed the linedef that triggered the elevator and the elevator action
//
// jff 2/22/98 new type to move floor and ceiling in parallel
//
dboolean EV_DoElevator(line_t *line, elevator_e elevtype)
{
    int         secnum = -1;
    dboolean    rtn = false;

    // act on all sectors with the same tag as the triggering linedef
    while ((secnum = P_FindSectorFromLineTag(line, secnum)) >= 0)
    {
        sector_t    *sec = sectors + secnum;
        elevator_t  *elevator;

        // If either floor or ceiling is already activated, skip it
        if (sec->floordata || sec->ceilingdata)
            continue;

        // create and initialize new elevator thinker
        rtn = true;
        elevator = Z_Calloc(1, sizeof(*elevator), PU_LEVSPEC, NULL);

        elevator->thinker.function = T_MoveElevator;
        P_AddThinker(&elevator->thinker);

        sec->floordata = elevator;
        sec->ceilingdata = elevator;
        elevator->type = elevtype;

        // set up the fields according to the type of elevator action
        switch (elevtype)
        {
            // elevator down to next floor
            case elevateDown:
                elevator->direction = DOWN;
                elevator->sector = sec;
                elevator->speed = ELEVATORSPEED;
                elevator->floordestheight = P_FindNextLowestFloor(sec, sec->floorheight);
                elevator->ceilingdestheight = elevator->floordestheight + sec->ceilingheight - sec->floorheight;
                break;

            // elevator up to next floor
            case elevateUp:
                elevator->direction = UP;
                elevator->sector = sec;
                elevator->speed = ELEVATORSPEED;
                elevator->floordestheight = P_FindNextHighestFloor(sec, sec->floorheight);
                elevator->ceilingdestheight = elevator->floordestheight + sec->ceilingheight - sec->floorheight;
                break;

            // elevator to floor height of activating switch's front sector
            case elevateCurrent:
                elevator->sector = sec;
                elevator->speed = ELEVATORSPEED;
                elevator->floordestheight = line->frontsector->floorheight;
                elevator->ceilingdestheight = elevator->floordestheight + sec->ceilingheight - sec->floorheight;
                elevator->direction = (elevator->floordestheight > sec->floorheight ? UP : DOWN);
                break;
        }
    }

    return rtn;
}
