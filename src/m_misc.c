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
#pragma warning( disable : 4091 )

#include <ShlObj.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>

#if defined(_MSC_VER)
#include <direct.h>
#endif

#else
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#endif

#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

#include "SDL.h"

#include "c_console.h"
#include "doomdef.h"
#include "i_system.h"
#include "m_fixed.h"
#include "m_misc.h"
#include "version.h"
#include "w_file.h"

#if defined(__APPLE__)
#import <Cocoa/Cocoa.h>

#include <dirent.h>
#include <libgen.h>
#include <mach-o/dyld.h>
#include <errno.h>
#endif

#if defined(__OpenBSD__) || defined(__FreeBSD__)
#include <sys/sysctl.h>
#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <unistd.h>
#elif defined(__linux__)
#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <unistd.h>
#endif

//
// Create a directory
//
void M_MakeDirectory(const char *path)
{
#if defined(_WIN32)
    mkdir(path);
#else
    mkdir(path, 0755);
#endif
}

// Check if a file exists
dboolean M_FileExists(const char *filename)
{
    FILE    *fstream = fopen(filename, "r");

    if (fstream)
    {
        fclose(fstream);
        return true;
    }

    return false;
}

// Check if a folder exists
dboolean M_FolderExists(const char *folder)
{
    struct stat status;

    return (!stat(folder, &status) && (status.st_mode & S_IFDIR));
}

// Safe string copy function that works like OpenBSD's strlcpy().
void M_StringCopy(char *dest, const char *src, const size_t dest_size)
{
    if (dest_size >= 1)
    {
        dest[dest_size - 1] = '\0';
        strncpy(dest, src, dest_size - 1);
    }
}

char *M_ExtractFolder(char *path)
{
    char    *pos;
    char    *folder;

    if (!*path)
        return "";

    folder = M_StringDuplicate(path);

    if ((pos = strrchr(folder, DIR_SEPARATOR)))
        *pos = '\0';

    return folder;
}

char *M_GetAppDataFolder(void)
{
    char    *executablefolder = M_GetExecutableFolder();

#if defined(_WIN32)
    return executablefolder;
#else
    // On Linux and macOS, if ../share/doomretro doesn't exist then we're dealing with
    // a portable installation, and we write doomretro.cfg to the executable directory.
    char    *resourcefolder = M_StringJoin(executablefolder,
                DIR_SEPARATOR_S ".." DIR_SEPARATOR_S "share" DIR_SEPARATOR_S PACKAGE, NULL);
    DIR     *resourcedir = opendir(resourcefolder);

    free(resourcefolder);

    if (resourcedir)
    {
        closedir(resourcedir);

#if defined(__APPLE__)
        // On OSX, store generated application files in ~/Library/Application Support/DOOM Retro.
        NSFileManager   *manager = [NSFileManager defaultManager];
        NSURL           *baseAppSupportURL = [manager URLsForDirectory : NSApplicationSupportDirectory
                            inDomains : NSUserDomainMask].firstObject;
        NSURL           *appSupportURL = [baseAppSupportURL URLByAppendingPathComponent : @PACKAGE_NAME];

        return (char *)appSupportURL.fileSystemRepresentation;
#else
        // On Linux, store generated application files in /home/<username>/.config/doomretro
        char            *buffer;

        if (!(buffer = SDL_getenv("HOME")))
            buffer = getpwuid(getuid())->pw_dir;

        free(executablefolder);

        return M_StringJoin(buffer, DIR_SEPARATOR_S ".config" DIR_SEPARATOR_S PACKAGE, NULL);
#endif
    }
    else
        return executablefolder;
#endif
}

char *M_GetResourceFolder(void)
{
    char    *executablefolder = M_GetExecutableFolder();

#if !defined(_WIN32)
    // On Linux and macOS, first assume that the executable is in ../bin and
    // try to load resources from ../share/doomretro.
    char    *resourcefolder = M_StringJoin(executablefolder,
                DIR_SEPARATOR_S ".." DIR_SEPARATOR_S "share" DIR_SEPARATOR_S PACKAGE, NULL);
    DIR     *resourcedir = opendir(resourcefolder);

    if (resourcedir)
    {
        closedir(resourcedir);
        free(executablefolder);
        return resourcefolder;
    }

#if defined(__APPLE__)
    // On OSX, load resources from the Contents/Resources folder within the application bundle
    // if ../share/doomretro is not available.
    NSURL   *resourceURL = [NSBundle mainBundle].resourceURL;

    return (char *)resourceURL.fileSystemRepresentation;
#else
    // And on Linux, fall back to the same folder as the executable.
    return executablefolder;
#endif

#else
    // On Windows, load resources from the same folder as the executable.
    return executablefolder;
#endif
}

char *M_GetExecutableFolder(void)
{
#if defined(_WIN32)
    char    *pos;
    char    *folder = malloc(MAX_PATH);
    TCHAR   buffer[MAX_PATH];

    if (!folder)
        return NULL;

    GetModuleFileName(NULL, buffer, MAX_PATH);
    M_StringCopy(folder, buffer, MAX_PATH);

    if ((pos = strrchr(folder, '\\')))
        *pos = '\0';

    return folder;
#elif defined(__linux__)
    char    *exe = malloc(MAX_PATH);
    ssize_t len = readlink("/proc/self/exe", exe, MAX_PATH - 1);

    if (len == -1)
    {
        strcpy(exe, ".");
        return exe;
    }
    else
    {
        exe[len] = '\0';
        return dirname(exe);
    }
#elif defined(__FreeBSD__)
    char    *exe = malloc(MAX_PATH);
    size_t  len = MAX_PATH;
    int     mib[] = { CTL_KERN, KERN_PROC, KERN_PROC_PATHNAME, -1 };

    if (!sysctl(mib, 4, exe, &len, NULL, 0))
    {
        exe[len] = '\0';
        return dirname(exe);
    }
    else
    {
        strcpy(exe, ".");
        return exe;
    }
#elif defined(__APPLE__)
    char        *exe = malloc(MAX_PATH);
    uint32_t    len = MAX_PATH;

    if (_NSGetExecutablePath(exe, &len))
    {
        strcpy(exe, ".");
        return exe;
    }

    return dirname(exe);
#else
    char    *folder = malloc(2);

    strcpy(folder, ".");
    return folder;
#endif
}

char *M_TempFile(char *s)
{
    char    *tempdir;

#if defined(_WIN32)
    tempdir = getenv("TEMP");

    if (!tempdir)
        tempdir = ".";
#else
    tempdir = "/tmp";
#endif

    return M_StringJoin(tempdir, DIR_SEPARATOR_S, s, NULL);
}

// Return a newly-malloced string with all the strings given as arguments
// concatenated together.
char *M_StringJoin(char *s, ...)
{
    char    *result;
    char    *v;
    va_list args;
    size_t  result_len = strlen(s) + 1;

    va_start(args, s);

    for (;;)
    {
        v = va_arg(args, char *);

        if (!v)
            break;

        result_len += strlen(v);
    }

    va_end(args);

    if (!(result = malloc(result_len)))
        return NULL;

    M_StringCopy(result, s, result_len);

    va_start(args, s);

    for (;;)
    {
        if (!(v = va_arg(args, char *)))
            break;

        strncat(result, v, result_len);
    }

    va_end(args);

    return result;
}

dboolean M_StrToInt(const char *str, unsigned int *result)
{
    return (sscanf(str, " 0x%2x", result) == 1 || sscanf(str, " 0X%2x", result) == 1
        || sscanf(str, " 0%3o", result) == 1 || sscanf(str, " %10u", result) == 1);
}

//
// M_StrCaseStr
//
// Case-insensitive version of strstr()
//
const char *M_StrCaseStr(const char *haystack, const char *needle)
{
    int haystack_len = (int)strlen(haystack);
    int needle_len = (int)strlen(needle);
    int len;

    if (haystack_len < needle_len)
        return NULL;

    len = haystack_len - needle_len;

    for (int i = 0; i <= len; i++)
        if (!strncasecmp(haystack + i, needle, needle_len))
            return (haystack + i);

    return NULL;
}

static char *stristr(char *ch1, char *ch2)
{
    char    *chN1 = M_StringDuplicate(ch1);
    char    *chN2 = M_StringDuplicate(ch2);
    char    *chRet = NULL;

    if (chN1 && chN2)
    {
        char    *chNdx = chN1;

        while (*chNdx)
        {
            *chNdx = (char)tolower(*chNdx);
            chNdx++;
        }

        chNdx = chN2;

        while (*chNdx)
        {
            *chNdx = (char)tolower(*chNdx);
            chNdx++;
        }

        if ((chNdx = strstr(chN1, chN2)))
            chRet = ch1 + (chNdx - chN1);
    }

    free(chN1);
    free(chN2);

    return chRet;
}

//
// String replace function.
//
char *M_StringReplace(char *haystack, char *needle, char *replacement)
{
    static char buffer[4096];
    char        *p;

    if (!(p = stristr(haystack, needle)))
        return haystack;

    strncpy(buffer, haystack, p - haystack);
    buffer[p - haystack] = '\0';
    sprintf(buffer + (p - haystack), "%s%s", replacement, p + strlen(needle));
    return buffer;
}

// Safe version of strdup() that checks the string was successfully allocated.
char *M_StringDuplicate(const char *orig)
{
    char    *result = strdup(orig);

    if (!result)
        I_Error("Failed to duplicate string %s.", stringize(orig));

    return result;
}

// Returns true if str1 and str2 are the same.
// (Case-insensitive, return value reverse of strcasecmp() to avoid confusion.
dboolean M_StringCompare(const char *str1, const char *str2)
{
    return !strcasecmp(str1, str2);
}

// Returns true if string begins with the specified prefix.
dboolean M_StringStartsWith(const char *s, const char *prefix)
{
    size_t  len = strlen(prefix);

    return (strlen(s) >= len && !strncasecmp(s, prefix, len));
}

// Returns true if string ends with the specified suffix.
dboolean M_StringEndsWith(const char *s, const char *suffix)
{
    size_t  len1 = strlen(s);
    size_t  len2 = strlen(suffix);

    return (len1 >= len2 && M_StringCompare(s + len1 - len2, suffix));
}

// Safe, portable vsnprintf().
int M_vsnprintf(char *buf, int buf_len, const char *s, va_list args)
{
    int result;

    if (buf_len < 1)
        return 0;

    // Windows (and other OSes?) has a vsnprintf() that doesn't always
    // append a trailing \0. So we must do it, and write into a buffer
    // that is one byte shorter; otherwise this function is unsafe.
    result = vsnprintf(buf, buf_len, s, args);

    // If truncated, change the final char in the buffer to a \0.
    // A negative result indicates a truncated buffer on Windows.
    if (result < 0 || result >= buf_len)
    {
        buf[buf_len - 1] = '\0';
        result = buf_len - 1;
    }

    return result;
}

// Safe, portable snprintf().
int M_snprintf(char *buf, int buf_len, const char *s, ...)
{
    va_list args;
    int     result;

    va_start(args, s);
    result = M_vsnprintf(buf, buf_len, s, args);
    va_end(args);
    return result;
}

#if !defined(strndup)
char *strndup(const char *s, size_t n)
{
    size_t  len = strnlen(s, n);
    char    *new = malloc(len + 1);

    if (!new)
        return NULL;

    new[len] = '\0';
    return memcpy(new, s, len);
}
#endif

char *M_SubString(const char *str, size_t begin, size_t len)
{
    size_t  length = strlen(str);

    if (!length || length < begin || length < begin + len)
        return 0;

    return strndup(str + begin, len);
}

char *uppercase(const char *str)
{
    char    *newstr;
    char    *p = newstr = M_StringDuplicate(str);

    while ((*p = toupper(*p)))
        p++;

    return newstr;
}

char *lowercase(char *str)
{
    for (char *p = str; *p; p++)
        *p = tolower(*p);

    return str;
}

char *titlecase(const char *str)
{
    char    *newstr = M_StringDuplicate(str);
    int     len = (int)strlen(newstr);

    if (len > 0)
    {
        newstr[0] = toupper(newstr[0]);

        if (len > 1)
            for (int i = 1; i < len; i++)
                if ((newstr[i - 1] != '\'' || (i >= 2 && newstr[i - 2] == ' '))
                    && !isalnum((unsigned char)newstr[i - 1])
                    && isalnum((unsigned char)newstr[i]))
                    newstr[i] = toupper(newstr[i]);
    }

    return newstr;
}

char *sentencecase(const char *str)
{
    char    *newstr = M_StringDuplicate(str);

    if (newstr[0] != '\0')
        newstr[0] = toupper(newstr[0]);

    return newstr;
}

char *commify(int64_t value)
{
    char    result[64];

    M_snprintf(result, sizeof(result), "%lli", value);

    if (value <= -1000 || value >= 1000)
    {
        char    *pt;
        size_t  n;

        for (pt = result; *pt && *pt != '.'; pt++);

        n = result + sizeof(result) - pt;

        do
        {
            pt -= 3;

            if (pt > result)
            {
                memmove(pt + 1, pt, n);
                *pt = ',';
                n += 4;
            }
            else
                break;
        } while (true);
    }

    return M_StringDuplicate(result);
}

char *uncommify(const char *input)
{
    char    *p;

    if (!*input)
        return "";

    if ((p = malloc(strlen(input) + 1)))
    {
        char    *p2 = p;

        while (*input != '\0')
            if (*input != ',' || *(input + 1) == '\0')
                *p2++ = *input++;
            else
                input++;

        *p2 = '\0';
    }

    return p;
}

dboolean wildcard(char *input, char *pattern)
{
    if (!*pattern)
        return true;

    for (int i = 0; pattern[i] != '\0'; i++)
    {
        if (pattern[i] == '?')
            continue;
        else if (pattern[i] == '*')
        {
            for (int z = i; input[z] != '\0'; z++)
                if (wildcard(input + z, pattern + i + 1))
                    return true;

            return false;
        }
        else if (pattern[i] != input[i])
            return false;
    }

    return false;
}

int gcd(int a, int b)
{
    return (!b ? a : gcd(b, a % b));
}

int numspaces(char *str)
{
    int result = 0;
    int len = (int)strlen(str);

    for (int i = 0; i < len; i++)
        result += (str[i] == ' ');

    return result;
}

char *removespaces(const char *input)
{
    char    *p;

    if (!*input)
        return "";

    if ((p = malloc(strlen(input) + 1)))
    {
        char    *p2 = p;

        while (*input != '\0')
            if (!isspace((unsigned char)*input))
                *p2++ = *input++;
            else
                input++;

        *p2 = '\0';
    }

    return p;
}

char *removenonalpha(const char *input)
{
    char    *p;

    if (!*input)
        return "";

    if ((p = malloc(strlen(input) + 1)))
    {
        char    *p2 = p;

        while (*input != '\0')
            if (!isspace((unsigned char)*input) && *input != '-' && *input != '(' && *input != ')')
                *p2++ = *input++;
            else
                input++;

        *p2 = '\0';
    }

    return p;
}

char *trimwhitespace(char *input)
{
    char    *end;

    while (isspace((unsigned char)*input))
        input++;

    if (!*input)
        return input;

    end = input + strlen(input) - 1;

    while (end > input && isspace((unsigned char)*end))
        end--;

    *(end + 1) = '\0';

    return input;
}

char *makevalidfilename(const char *input)
{
    char    *newstr = M_StringDuplicate(input);
    int     len = (int)strlen(newstr);

    for (int i = 0; i < len; i++)
        if (strchr("\\/:?\"<>|", newstr[i]))
            newstr[i] = ' ';

    return newstr;
}

char *leafname(char *path)
{
    char    cc;
    char    *ptr = path;

    do
    {
        cc = *ptr++;

        if (cc == '\\' || cc == '/')
            path = ptr;
    } while (cc);

    return path;
}

char *removeext(const char *file)
{
    char    *newstr = M_StringDuplicate(file);
    char    *lastdot = strrchr(newstr, '.');

    *lastdot = '\0';

    return newstr;
}

dboolean isvowel(const char ch)
{
    return !!strchr("aeiouAEIOU", ch);
}

dboolean ispunctuation(const char ch)
{
    return !!strchr(".!?", ch);
}

dboolean isbreak(const char ch)
{
    return !!strchr(" /\\-", ch);
}

char *striptrailingzero(float value, int precision)
{
    char    *result = malloc(100);

    if (result)
    {
        int len;

        M_snprintf(result, 100, "%.*f", (precision == 2 ? 2 : (value != floor(value))), value);
        len = (int)strlen(result);

        if (len >= 4 && result[len - 3] == '.' && result[len - 1] == '0')
            result[len - 1] = '\0';
    }

    return result;
}

void strreplace(char *target, char *needle, const char *replacement)
{
    char    buffer[1024] = "";
    char    *insert_point = &buffer[0];
    char    *tmp = target;
    int     needle_len = (int)strlen(needle);
    int     repl_len = (int)strlen(replacement);

    while (true)
    {
        char    *p = stristr(tmp, needle);

        if (!p)
        {
            strcpy(insert_point, tmp);
            break;
        }

        memcpy(insert_point, tmp, p - tmp);
        insert_point += p - tmp;

        memcpy(insert_point, replacement, repl_len);
        insert_point += repl_len;

        tmp = p + needle_len;
    }

    strcpy(target, buffer);
}

static const long hextable[] =
{
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 10, 11, 12, 13, 14, 15, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
};

int hextodec(char *hex)
{
    int ret = 0;

    while (*hex && ret >= 0)
        ret = ((ret << 4) | hextable[*hex++]);

    return ret;
}

void M_StripQuotes(char *string)
{
    int len = (int)strlen(string);

    if (len > 2 && ((string[0] == '"' && string[len - 1] == '"') || (string[0] == '\'' && string[len - 1] == '\'')))
    {
        len -= 2;
        memmove(string, string + 1, len);
        string[len] = '\0';
    }
}
