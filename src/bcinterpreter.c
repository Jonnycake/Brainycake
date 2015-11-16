/*
 *    bcinterpreter: Interprets and executes Brainycake - a derivative of brainfuck.
 *    Copyright (C) 2015  Jon Stockton
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *    Contributors:
 *    -------------
 *    Jon Stockton (Jonnycake) <jonstockton1416@gmail.com>
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <brainycake.h>
/**
 * @todo Add/implement debugging instructions
 * @todo Add/implement registers
 * @todo Add/implement stack
 * @todo Add/implement jumps
 * @todo Add/implement conditionals
 * @todo Add/implement shortcuts
 * @todo Add/implement functions
 * @todo Add/implement fun stuff
 * @todo Optimizations!
 *     - Loops definitely need to be optimized, I think most of the performance issues arise from file io
 *       - Files should be read in at the beginning
 *     - When we read in files, we should store without comments/whitespace to preserve memory
 */

int main(int argc, char** argv)
{
    int error = 0;
    char* mainfile;
    char* code;
    extern char verbose, superverbose, debug, quiet;

    if(argc == 1) {
        printf("Syntax: %s [options] <filename>\nFor more information type %s --help\n", argv[0], argv[0]);
    } else {
        if(!strcmp(argv[1], "--help")) {
            printf("Options:\n");
            printf("-v    Verbose mode on by default\n");
            printf("-vv   Verbose mode that can not be turned off\n");
            printf("-d    Debug mode (prints function names as they're called)\n");
            return 0;
        }

        mainfile = argv[argc - 1];
        if(argc > 2) {
            int x;
            for(x = 1; x < argc - 1; x++) {
                if(!strcmp(argv[x], "-vv")) {
                    superverbose = 1;
                } else if(!strcmp(argv[x], "-v")) {
                    verbose = 1;
                } else if(!strcmp(argv[x], "-d")) {
                    debug = 1;
                } else if(!strcmp(argv[x], "-q")) {
                    quiet = 1;
                } else {
                    printf("Invalid argument: '%s'\n", argv[x]);
                    error = ERROR_UNKNOWN;
                    break;
                }
            }
        }
        if(!error) {
            bc_preprocess(mainfile, &code);
            error = bc_execute(code);
        }
    }
    return error;
}