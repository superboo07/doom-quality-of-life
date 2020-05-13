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

#if defined(_WIN32)
#include <Windows.h>
#endif

#include "c_console.h"
#include "doomstat.h"
#include "i_swap.h"
#include "i_system.h"
#include "m_misc.h"
#include "w_merge.h"
#include "w_wad.h"
#include "z_zone.h"

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(push, 1)
#endif

typedef struct
{
    // Should be "IWAD" or "PWAD".
    char    id[4];
    int     numlumps;
    int     infotableofs;
} PACKEDATTR wadinfo_t;

typedef struct
{
    int     filepos;
    int     size;
    char    name[8];
} PACKEDATTR filelump_t;

#if defined(_MSC_VER) || defined(__GNUC__)
#pragma pack(pop)
#endif

// Location of each lump on disk.
lumpinfo_t  **lumpinfo;
int         numlumps;

extern char *packagewad;

static dboolean IsFreedoom(const char *iwadname)
{
    FILE        *fp = fopen(iwadname, "rb");
    wadinfo_t   header;
    int         result = false;

    if (!fp)
        I_Error("Can't open IWAD: %s\n", iwadname);

    // read IWAD header
    if (fread(&header, 1, sizeof(header), fp) == sizeof(header))
    {
        filelump_t  lump;
        const char  *n = lump.name;

        fseek(fp, LONG(header.infotableofs), SEEK_SET);

        for (header.numlumps = LONG(header.numlumps); header.numlumps && fread(&lump, sizeof(lump), 1, fp); header.numlumps--)
            if (n[0] == 'F' && n[1] == 'R' && n[2] == 'E' && n[3] == 'E' && n[4] == 'D' && n[5] == 'O' && n[6] == 'O' && n[7] == 'M')
            {
                result = true;
                break;
            }
    }

    fclose(fp);
    return result;
}

dboolean IsBFGEdition(const char *iwadname)
{
    FILE        *fp = fopen(iwadname, "rb");
    wadinfo_t   header;
    int         result1 = false;
    int         result2 = false;

    if (!fp)
        return false;

    // read IWAD header
    if (fread(&header, 1, sizeof(header), fp) == sizeof(header))
    {
        filelump_t  lump;
        const char  *n = lump.name;

        fseek(fp, LONG(header.infotableofs), SEEK_SET);

        for (header.numlumps = LONG(header.numlumps); header.numlumps && fread(&lump, sizeof(lump), 1, fp); header.numlumps--)
            if (n[0] == 'D' && n[1] == 'M' && n[2] == 'E' && n[3] == 'N' && n[4] == 'U' && n[5] == 'P' && n[6] == 'I' && n[7] == 'C')
            {
                result1 = true;

                if (result2)
                    break;
            }
            else if (n[0] == 'M' && n[1] == '_' && n[2] == 'A' && n[3] == 'C' && n[4] == 'P' && n[5] == 'T')
            {
                result2 = true;

                if (result1)
                    break;
            }
    }

    fclose(fp);
    return (result1 && result2);
}

dboolean IsUltimateDOOM(const char *iwadname)
{
    FILE        *fp = fopen(iwadname, "rb");
    wadinfo_t   header;
    int         result = false;

    if (!fp)
        return false;

    // read IWAD header
    if (fread(&header, 1, sizeof(header), fp) == sizeof(header))
    {
        filelump_t  lump;
        const char  *n = lump.name;

        fseek(fp, LONG(header.infotableofs), SEEK_SET);

        for (header.numlumps = LONG(header.numlumps); header.numlumps && fread(&lump, sizeof(lump), 1, fp); header.numlumps--)
            if (n[0] == 'E' && n[1] == '4' && n[2] == 'M' && n[3] == '1')
            {
                result = true;
                break;
            }
    }

    fclose(fp);
    return result;
}

char *GetCorrectCase(char *path)
{
#if defined(_WIN32)
    WIN32_FIND_DATA FindFileData;
    HANDLE          hFile = FindFirstFile(path, &FindFileData);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        FindClose(hFile);
        strreplace(path, FindFileData.cFileName, FindFileData.cFileName);
    }
#endif

    return path;
}

#if defined(_WIN32)
static int LevenshteinDistance(char *string1, char *string2)
{
    size_t  length1 = strlen(string1);
    size_t  length2 = strlen(string2);
    int     result = INT_MAX;

    if (length1 > 0 && length2 > 0)
    {
        int *column = malloc((length1 + 1) * sizeof(int));

        for (int y = 1; (size_t)y <= length1; y++)
            column[y] = y;

        for (int x = 1; (size_t)x <= length2; x++)
        {
            column[0] = x;

            for (int y = 1, lastdiagonal = x - 1, olddiagonal; (size_t)y <= length1; y++)
            {
                olddiagonal = column[y];
                column[y] = MIN(MIN(column[y], column[y - 1]) + 1, lastdiagonal + (string1[y - 1] != string2[x - 1]));
                lastdiagonal = olddiagonal;
            }
        }

        result = column[length1];
        free(column);
    }

    return result;
}

char *W_NearestFilename(char *path, char *string)
{
    WIN32_FIND_DATA FindFileData;
    char            *file = M_StringJoin(path, DIR_SEPARATOR_S "*.wad", NULL);
    HANDLE          hFile = FindFirstFile(file, &FindFileData);
    int             bestdistance = INT_MAX;
    char            filename[MAX_PATH];
    char            *string1;

    if (hFile == INVALID_HANDLE_VALUE)
        return path;

    M_StringCopy(filename, string, sizeof(filename));
    string1 = removeext(string);

    do
    {
        if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
        {
            char    *string2 = removeext(FindFileData.cFileName);
            int     distance = LevenshteinDistance(string1, string2);

            if (distance <= 2 && distance < bestdistance)
            {
                M_StringCopy(filename, FindFileData.cFileName, sizeof(filename));
                bestdistance = distance;
            }

            free(string2);
        }
    } while (FindNextFile(hFile, &FindFileData));

    FindClose(hFile);
    free(file);
    free(string1);
    return M_StringJoin(path, DIR_SEPARATOR_S, filename, NULL);
}
#endif

//
// LUMP BASED ROUTINES.
//

//
// W_AddFile
// All files are optional, but at least one file must be
//  found (PWAD, if all required lumps are present).
// Files with a .wad extension are wadlink files
//  with multiple lumps.
//
dboolean W_AddFile(char *filename, dboolean automatic)
{
    static dboolean packagewadadded;
    wadinfo_t       header;
    int             length;
    int             startlump;
    filelump_t      *fileinfo;
    filelump_t      *filerover;
    lumpinfo_t      *filelumps;
    char            *temp;

    // open the file and add to directory
    wadfile_t       *wadfile = W_OpenFile(filename);

    if (!wadfile)
        return false;

    M_StringCopy(wadfile->path, GetCorrectCase(filename), sizeof(wadfile->path));

    if ((wadfile->freedoom = IsFreedoom(filename)))
        FREEDOOM = true;

    // WAD file
    W_Read(wadfile, 0, &header, sizeof(header));

    // Homebrew levels?
    if (strncmp(header.id, "IWAD", 4) && strncmp(header.id, "PWAD", 4))
        I_Error("%s doesn't have an IWAD or PWAD id.", filename);

    wadfile->type = (!strncmp(header.id, "IWAD", 4) || M_StringCompare(leafname(filename), "DOOM2.WAD") ? IWAD : PWAD);

    if (wadfile->type == IWAD)
        bfgedition = IsBFGEdition(filename);

    header.numlumps = LONG(header.numlumps);
    header.infotableofs = LONG(header.infotableofs);
    length = header.numlumps * sizeof(filelump_t);
    fileinfo = malloc(length);
    W_Read(wadfile, header.infotableofs, fileinfo, length);

    // Increase size of numlumps array to accommodate the new file.
    filelumps = calloc(header.numlumps, sizeof(lumpinfo_t));

    startlump = numlumps;
    numlumps += header.numlumps;
    lumpinfo = I_Realloc(lumpinfo, numlumps * sizeof(lumpinfo_t *));
    filerover = fileinfo;

    for (int i = startlump; i < numlumps; i++)
    {
        lumpinfo_t  *lump_p = &filelumps[i - startlump];

        lump_p->wadfile = wadfile;
        lump_p->position = LONG(filerover->filepos);
        lump_p->size = LONG(filerover->size);
        lump_p->cache = NULL;
        strncpy(lump_p->name, filerover->name, 8);
        lumpinfo[i] = lump_p;
        filerover++;
    }

    free(fileinfo);

    temp = commify((int64_t)numlumps - startlump);
    C_Output("%s %s lump%s from %s <b>%s</b>.", (automatic ? "Automatically added" : "Added"), temp,
        (numlumps - startlump == 1 ? "" : "s"), (wadfile->type == IWAD ? "IWAD" : "PWAD"), wadfile->path);
    free(temp);

    if (M_StringCompare(leafname(filename), "SIGIL_v1_21.wad")
        || M_StringCompare(leafname(filename), "SIGIL_v1_2.wad")
        || M_StringCompare(leafname(filename), "SIGIL_v1_1.wad")
        || M_StringCompare(leafname(filename), "SIGIL.wad"))
    {
        autosigil = automatic;
        C_Output("<i><b>SIGIL</b></i> is now available to play from the episode menu.");
    }
    else if (M_StringCompare(leafname(filename), "SIGIL_SHREDS.WAD")
        || M_StringCompare(leafname(filename), "SIGIL_SHREDS_COMPAT.wad"))
    {
        buckethead = true;
        C_Output("Buckethead's soundtrack will now be used when playing <i><b>SIGIL.</b></i>");
    }
    else if (M_StringCompare(leafname(filename), "DOOM.WAD"))
        C_Output("<i><b>E1M4B: Phobos Mission Control</b></i> and <i><b>E1M8B: Tech Gone Bad</b></i> "
            "are now available to play using the <b>map</b> CCMD.");
    else if (M_StringCompare(leafname(filename), "NERVE.WAD"))
        C_Output("<i><b>No Rest For The Living</b></i> is now available to play from the expansion menu.");

    if (!packagewadadded)
    {
        packagewadadded = true;

        if (!W_MergeFile(packagewad, true))
            I_Error("%s is invalid.", packagewad);
    }

    return true;
}

// Hash function used for lump names.
// Must be modded with table size.
// Can be used for any 8-character names.
// by Lee Killough
unsigned int W_LumpNameHash(const char *s)
{
    unsigned int    hash;

    (void)((hash = toupper(s[0]), s[1])
        && (hash = hash * 3 + toupper(s[1]), s[2])
        && (hash = hash * 2 + toupper(s[2]), s[3])
        && (hash = hash * 2 + toupper(s[3]), s[4])
        && (hash = hash * 2 + toupper(s[4]), s[5])
        && (hash = hash * 2 + toupper(s[5]), s[6])
        && (hash = hash * 2 + toupper(s[6]),
            hash = hash * 2 + toupper(s[7])));

    return hash;
}

dboolean HasDehackedLump(const char *pwadname)
{
    FILE        *fp = fopen(pwadname, "rb");
    filelump_t  lump;
    wadinfo_t   header;
    const char  *n = lump.name;
    int         result = false;

    if (!fp)
        return false;

    // read IWAD header
    if (fread(&header, 1, sizeof(header), fp) == sizeof(header))
    {
        fseek(fp, LONG(header.infotableofs), SEEK_SET);

        for (header.numlumps = LONG(header.numlumps); header.numlumps && fread(&lump, sizeof(lump), 1, fp); header.numlumps--)
            if (n[0] == 'D' && n[1] == 'E' && n[2] == 'H' && n[3] == 'A' && n[4] == 'C' && n[5] == 'K' && n[6] == 'E' && n[7] == 'D')
            {
                result = true;
                break;
            }
    }

    fclose(fp);
    return result;
}

GameMission_t IWADRequiredByPWAD(char *pwadname)
{
    char            *leaf = leafname(pwadname);
    FILE            *fp = fopen(pwadname, "rb");
    filelump_t      lump;
    wadinfo_t       header;
    const char      *n = lump.name;
    GameMission_t   result = none;

    if (!fp)
        I_Error("Can't open PWAD: %s\n", pwadname);

    if (fread(&header, 1, sizeof(header), fp) != sizeof(header)
        || (header.id[0] != 'I' && header.id[0] != 'P') || header.id[1] != 'W' || header.id[2] != 'A' || header.id[3] != 'D')
        I_Error("%s doesn't have an IWAD or PWAD id.", pwadname);

    fseek(fp, LONG(header.infotableofs), SEEK_SET);

    for (header.numlumps = LONG(header.numlumps); header.numlumps && fread(&lump, sizeof(lump), 1, fp); header.numlumps--)
        if (n[0] == 'E' && isdigit((int)n[1]) && n[2] == 'M' && isdigit((int)n[3]) && n[4] == '\0')
            result = doom;
        else if (n[0] == 'M' && n[1] == 'A' && n[2] == 'P' && isdigit((int)n[3]) && isdigit((int)n[4]) && n[5] == '\0')
            result = doom2;

    fclose(fp);

    if (result == doom2)
    {
        if (M_StringCompare(leaf, "pl2.wad") || M_StringCompare(leaf, "plut3.wad"))
            result = pack_plut;
        else if (M_StringCompare(leaf, "tntr.wad") || M_StringCompare(leaf, "tnt-ren.wad") || M_StringCompare(leaf, "resist.wad"))
            result = pack_tnt;
    }

    return result;
}

//
// W_WadType
// Returns IWAD, PWAD or 0.
//
int W_WadType(char *filename)
{
    wadinfo_t   header;
    wadfile_t   *wadfile = W_OpenFile(filename);

    if (!wadfile)
        return 0;

    W_Read(wadfile, 0, &header, sizeof(header));
    W_CloseFile(wadfile);

    if (!strncmp(header.id, "IWAD", 4) || M_StringCompare(leafname(filename), "DOOM2.WAD"))
        return IWAD;
    else if (!strncmp(header.id, "PWAD", 4))
        return PWAD;
    else
        return 0;
}

//
// W_CheckNumForName
// Returns -1 if name not found.
//
// Rewritten by Lee Killough to use hash table for performance. Significantly
// cuts down on time -- increases DOOM performance over 300%. This is the
// single most important optimization of the original DOOM sources, because
// lump name lookup is used so often, and the original DOOM used a sequential
// search. For large wads with > 1000 lumps this meant an average of over
// 500 were probed during every search. Now the average is under 2 probes per
// search. There is no significant benefit to packing the names into longwords
// with this new hashing algorithm, because the work to do the packing is
// just as much work as simply doing the string comparisons with the new
// algorithm, which minimizes the expected number of comparisons to under 2.
//
int W_CheckNumForName(const char *name)
{
    // Hash function maps the name to one of possibly numlump chains.
    // It has been tuned so that the average chain length never exceeds 2.
    int i = lumpinfo[W_LumpNameHash(name) % numlumps]->index;

    while (i >= 0 && strncasecmp(lumpinfo[i]->name, name, 8))
        i = lumpinfo[i]->next;

    // Return the matching lump, or -1 if none found.
    return i;
}

//
// W_CheckMultipleLumps
// Check if there's more than one of the same lump.
//
int W_CheckMultipleLumps(const char *name)
{
    int count = 0;

    if (FREEDOOM || hacx)
        return 3;

    for (int i = numlumps - 1; i >= 0; i--)
        if (!strncasecmp(lumpinfo[i]->name, name, 8))
            count++;

    return count;
}

//
// W_RangeCheckNumForName
// Linear Search that checks for a lump number ONLY
// inside a range, not all lumps.
//
int W_RangeCheckNumForName(int min, int max, const char *name)
{
    for (int i = min; i <= max; i++)
        if (!strncasecmp(lumpinfo[i]->name, name, 8))
            return i;

    return -1;
}

void W_Init(void)
{
    for (int i = 0; i < numlumps; i++)
        lumpinfo[i]->index = -1;                       // mark slots empty

    // Insert nodes to the beginning of each chain, in first-to-last
    // lump order, so that the last lump of a given name appears first
    // in any chain, observing pwad ordering rules. killough
    for (int i = 0; i < numlumps; i++)
    {
        // hash function:
        int j = W_LumpNameHash(lumpinfo[i]->name) % numlumps;

        lumpinfo[i]->next = lumpinfo[j]->index;       // Prepend to list
        lumpinfo[j]->index = i;
    }
}

//
// W_GetNumForName
// Calls W_CheckNumForName, but bombs out if not found.
//
int W_GetNumForName(const char *name)
{
    int i = W_CheckNumForName(name);

    if (i < 0)
        I_Error("W_GetNumForName: %s not found!", name);

    return i;
}

// Go forwards rather than backwards so we get lump from IWAD and not PWAD
int W_GetLastNumForName(const char *name)
{
    int i;

    for (i = 0; i < numlumps; i++)
        if (!strncasecmp(lumpinfo[i]->name, name, 8))
            break;

    if (i == numlumps)
        I_Error("W_GetLastNumForName: %s not found!", name);

    return i;
}

int W_GetSecondNumForName(const char *name)
{
    int count = 0;
    int i;

    for (i = 0; i < numlumps; i++)
        if (!strncasecmp(lumpinfo[i]->name, name, 8))
            if (++count == 2)
                break;

    if (i == numlumps)
        I_Error("W_GetSecondNumForName: %s not found!", name);

    return i;
}

//
// W_LumpLength
// Returns the buffer size needed to load the given lump.
//
int W_LumpLength(int lump)
{
    if (lump >= numlumps)
        I_Error("W_LumpLength: %i >= numlumps", lump);

    return lumpinfo[lump]->size;
}

//
// W_ReadLump
// Loads the lump into the given buffer,
//  which must be >= W_LumpLength().
//
void W_ReadLump(int lump, void *dest)
{
    size_t      c;
    lumpinfo_t  *l = lumpinfo[lump];

    if (!l->size || !dest)
        return;

    c = W_Read(l->wadfile, l->position, dest, l->size);

    if (c < (size_t)l->size)
        I_Error("W_ReadLump: only read %zd of %i on lump %i", c, l->size, lump);
}

void *W_CacheLumpNum(int lumpnum)
{
    lumpinfo_t  *lump = lumpinfo[lumpnum];

    if (!lump->cache)
        W_ReadLump(lumpnum, Z_Malloc(lump->size, PU_CACHE, &lump->cache));

    return lump->cache;
}

void W_ReleaseLumpNum(int lumpnum)
{
    Z_ChangeTag(lumpinfo[lumpnum]->cache, PU_CACHE);
}
