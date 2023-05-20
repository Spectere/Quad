/*
Copyright (C) 2023 Ian Burgmyer

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/
// sys_sdl2.c -- Generic SDL2 system interface code

#define SDL_MAIN_HANDLED

#include <stddef.h>
#include <sys/stat.h>
#include <errno.h>
#include "sys_sdl2.h"

#if defined(POSIX) || defined(MSYS)
#include <sys/file.h>
#include <sys/time.h>
#include <unistd.h>
#endif // POSIX || MSYS

#ifdef MSVC
#include <direct.h>
#include <fcntl.h>
#include <io.h>
#endif // MSVC

#include "host.h"

#define DEFAULT_HEAPSIZE (32 * 1024 * 1024)  // 32MiB

char *basedir = ".";

qboolean isDedicated = false;
int no_stdout = 0;

int Sys_FileOpenRead(char *path, int *handle) {
    int	h;
    struct stat	fileinfo;

    h = open(path, O_RDONLY, 0666);
    *handle = h;
    if (h == -1)
        return -1;

    if(fstat (h,&fileinfo) == -1) {
        Sys_Error("Error fstating %s", path);
    }

    return fileinfo.st_size;
}

int Sys_FileOpenWrite(char *path) {
    int handle;

    umask(0);

    handle = open(path, O_RDWR | O_CREAT | O_TRUNC, 0666);

    if(handle == -1) {
        Sys_Error("Error opening %s: %s", path, strerror(errno));
    }

    return handle;
}

void Sys_FileClose(int handle) {
    close(handle);
}

void Sys_FileSeek(int handle, int position) {
    lseek(handle, position, SEEK_SET);
}

int Sys_FileRead(int handle, void *dest, int count) {
    return read(handle, dest, count);
}

int Sys_FileWrite(int handle, void *data, int count) {
    return write(handle, data, count);
}

int	Sys_FileTime(char *path) {
    struct stat buf;

    if(stat(path, &buf) == -1)
        return -1;

    return buf.st_mtime;
}

void Sys_mkdir(char *path) {
#ifdef WIN32
    mkdir(path);
#else
    mkdir(path, 0777);
#endif // WIN32
}

void Sys_Error(char *error, ...) {
    va_list argptr;
    char string[1024];

    va_start(argptr, error);
    vsprintf(string, error, argptr);
    va_end(argptr);
    fprintf(stderr, "Error: %s\n", string);

    Host_Shutdown();
    exit(1);
}

void Sys_Printf(char *fmt, ...) {
    va_list argptr;
    char text[MAXPRINTMSG];
    unsigned char *p;

    va_start(argptr, fmt);
    int result = vsnprintf(text, MAXPRINTMSG, fmt, argptr);
    va_end(argptr);

    if(no_stdout) {
        return;
    }

    for(p = (unsigned char *)text; *p; p++) {
        *p &= 0x7f;
        if((*p > 128 || *p < 32) && *p != 10 && *p != 13 && *p != 9) {
            printf("[%02x]", *p);
        } else {
            putc(*p, stdout);
        }
    }

    // If there are excess characters from the vsnprintf call, append a newline.
    if(result > strlen(text)) {
        putc('\n', stdout);
    }
}

void Sys_Quit(void) {
    Host_Shutdown();
    fflush(stdout);
    SDL_Quit();
    exit(0);
}

double Sys_FloatTime(void) {
#if defined(POSIX) || defined(MSYS)
    // POSIX-y time implementation.
    struct timeval tp;
    struct timezone tzp;
    static int secbase;

    gettimeofday(&tp, &tzp);

    if(!secbase) {
        secbase = tp.tv_sec;
        return tp.tv_usec / 1000000.0;
    }

    return (tp.tv_sec - secbase) + tp.tv_usec / 1000000.0;
#else
    // SDL time implementation.
    return (double)SDL_GetTicks64() / 1000;
#endif // POSIX || MSYS
}

char *Sys_ConsoleInput(void) {  // TODO: Might have to rework this for Win32 (does it have unistd.h?)
#ifdef POSIX
    static char text[256];
    long len;
    fd_set fdset;
    struct timeval timeout;

    if (cls.state == ca_dedicated) {
        FD_ZERO(&fdset);
        FD_SET(0, &fdset); // stdin
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
        if (select (1, &fdset, NULL, NULL, &timeout) == -1 || !FD_ISSET(0, &fdset)) {
            return NULL;
        }

        len = read(0, text, sizeof(text));
        if(len < 1) {
            return NULL;
        }
        text[len - 1] = 0;    // rip off the \n and terminate

        return text;
    }
#endif // POSIX

    return NULL;
}

/*
 * Entrypoint
 */
int main(int argc, char** argv) {
    extern int vcrFile, recording;
    int param_no;
    double time, time_now, time_prev;
    quakeparms_t parms;

    memset(&parms, 0, sizeof(parms));

    // Init argument parser.
    COM_InitArgv(argc, argv);
    parms.argc = com_argc;
    parms.argv = com_argv;

    // Set hardcoded parameters.
    parms.basedir = basedir;

    // Figure out the desired heapsize and allocate it.
    parms.memsize = DEFAULT_HEAPSIZE;

    param_no = COM_CheckParm("-mem");
    if(param_no) {
        parms.memsize = (int)(Q_atof(com_argv[param_no + 1]) * 1024 * 1024);
    }

    parms.membase = malloc(parms.memsize);

    // Initialize game.
    Host_Init(&parms);

    // Disable stdout?
    if(COM_CheckParm("-nostdout")) {
        no_stdout = 1;
    } else {
        printf("Quad -- Version %0.2f\n", QUAD_VERSION);
    }

    time_prev = Sys_FloatTime() - 0.1;
    while(1) {
        time_now = Sys_FloatTime();
        time = time_now - time_prev;

        if(cls.state == ca_dedicated) {
            // Play demos at max speed.
            if(time < sys_ticrate.value && (vcrFile || recording)) {
                SDL_Delay(1);
                continue;
            }
            time = sys_ticrate.value;
        }

        if(time > sys_ticrate.value * 2) {
            time_prev = time_now;
        } else {
            time_prev += time;
        }

        Host_Frame(time);
        SDL_Delay(1);
    }
}
