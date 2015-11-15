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

// Memory settings
#define INIT_CELL_COUNT   64        // Number of cells to initalize for basic array
#define MAX_CELL_COUNT    256       // Maximum number of cells to allow
#define MAX_STACK_HEIGHT  1024      // Maximum height stack can get to 
#define STACK_WIDTH       4         // Number of bytes per level of the stack
#define MAX_LOOPS         32        // Maximum number of nested loops

// Error codes
#define ERROR_SYNTAX  1 // Syntax errors
#define ERROR_OOB     2 // Out of bounds - trying to push onto full stack, etc.
#define ERROR_RO      3 // Read-only - Trying to write to $0
#define ERROR_UNDEF   4 // Undefined function
#define ERROR_PROG    5 // Program set error condition
#define ERROR_UNKNOWN 6 // All other error conditions

// Parsing modes
#define MODE_JUMP_ABS   1
#define MODE_JUMP_REL   2
#define MODE_COMMENT_SL 3
#define MODE_COMMENT_ML 4
#define MODE_EXEC       5
#define MODE_OUTPUT     6
#define MODE_FUNC_INIT  7
#define MODE_FUNC_DEF   8
#define MODE_INCLUDE    9
#define MODE_REG_MANIP  10
#define MODE_COND_EVAL  11

int main(int argc, char** argv)
{
    FILE** f = NULL;
    int curCellCount = INIT_CELL_COUNT;
    char c;
    char* mainfile;

    char quiet = 0;
    char verbose = 0;
    char superverbose = 0;
    char error = 0;
    char debug = 0;

    char registers[5] = {0};
    int extregisters[5] = {0};

    int loop_positions[MAX_LOOPS] = {0};
    unsigned char numloops = 0;

    char mode = MODE_EXEC;

    int p = 0;

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
                    printf("%s\n", argv[x]);
                }
            }
        }

        char* a = calloc(sizeof(char), INIT_CELL_COUNT);
        f = realloc(f, 1 * sizeof(FILE*));


        f[0] = fopen(mainfile, "rb");
        while( !error
            && (c = fgetc(f[0])) != EOF
        ) {
            if(verbose + superverbose) {
                printf("\n\nINSTRUCTION: %c\nCurrent Stats: %d - %c - %d\n\n", c, p, a[p], a[p]);
            }

            // @todo Refactoring this is required for being able to process include files
            //       without significant code duplication.
            switch(c)
            {
                // Traditional brainfuck
                case '>':
                    if(++p >= curCellCount) {
                        // Provide wrap around rather than error out
                        if((curCellCount + INIT_CELL_COUNT) > MAX_CELL_COUNT) {
                            p = 0;
                        } else {
                            curCellCount += INIT_CELL_COUNT;
                            a = realloc(a, curCellCount * sizeof(char));
                        }
                    }
                    break;
                case '<':
                    if(--p < 0) {
                        p = curCellCount - 1;
                    }
                    break;
                case '.':
                    putchar(a[p]);
                    break;
                case ',':
                    a[p] = getchar();
                    break;
                case '+':
                    a[p]++;
                    break;
                case '-':
                    a[p]--;
                    break;
                case '[':
                    if( a[p] ) {
                        loop_positions[(int)numloops++] = ftell(f[0]);
                    } else {
                        int lastloop = numloops;
                        for( ; c != EOF && numloops != lastloop; c = fgetc(f[0]) ) {
                            if( c == ']' ) {
                                numloops--;
                            } else if ( c == '[' ) {
                                numloops++;
                            }
                        }

                        if( c == EOF ) {
                            printf("Syntax error: No matching ']' for '['!");
                        }
                    }
                    break;
                case ']':
                    if( a[p] ) {
                        fseek(f[0], loop_positions[(int) numloops - 1], SEEK_SET);
                    } else {
                        loop_positions[(int)--numloops] = 0;
                    }
                    break;

                // Shortcuts
                case '_':
                    p = 0;
                    break;
                case '^':
                    a[p] = 0;
                    break;
                case '|':
                    {
                        char v[4] = {0};
                        int count = 0;
                        for( c = fgetc(f[0]);
                             c != EOF
                             && c != '|'
                             && count < 3;
                             c = fgetc(f[0]), count++
                        ) {
                            if(c >= 48 && c < 58) {
                                v[count] = c;
                            } else {
                                error = ERROR_UNKNOWN;
                                break;
                            }
                        }
                        if( c == EOF ) {
                            printf("Syntax Error: Got EOF when attempting to read value for addition.");
                            error = ERROR_SYNTAX;
                        } else if(c == '|') {
                            count = atoi(v);
                            if(count > 255 && !quiet) {
                                printf("Warning: Number is greater than 255, there may be unexpected results.\n");
                            }
                            a[p] += atoi(v);
                        } else {
                            printf("Error: Requested number is out of bounds.\n");
                        }
                    }
                    break;
                // Comments
                case '#':
                    for( ; c != EOF && c != '\n'; c = fgetc(f[0]) ) ;
                    break;
                case '/':
                    for( c = fgetc(f[0]); c != EOF && c!= '/'; c = fgetc(f[0]) ) ;
                    if( c == EOF ) {
                        printf("Syntax error: No matching '/'!\n");
                        error = 1;
                    }
                    break;

                // String output
                case '\\':
                    for( c = fgetc(f[0]); c != EOF && c != '\\'; c = fgetc(f[0]) ) {
                        putchar(c);
                    }
                    if( c == EOF ) {
                        printf("Syntax error: No matching '\\'!\n");
                        error = 1;
                    }
                    break;

                // Registers
                case '$':
                    c = fgetc(f[0]);
                    if( c == EOF ) {
                        printf("Error: Scanned to end of file when attempting to identify a register.");
                    } else {
                        switch(c)
                        {
                            case '0':
                                printf("Error: Attempted to write to read-only register ($0).");
                                error = 1;
                                break;
                            case '1':
                            case '2':
                            case '3':
                            case '4':
                                registers[c - 48] = a[p];
                                break;
                            case '5':
                            case '6':
                            case '7':
                            case '8':
                            case '9':
                                extregisters[c - 53] = a[p];
                                break;
                            case '{':
                                mode = MODE_REG_MANIP;
                                break;
                            case '+':
                                registers[1] += a[p];
                                break;
                            case '-':
                                registers[1] -= a[p];
                                break;
                            case '/':
                                registers[1] /= a[p];
                                break;
                            case '*':
                                registers[1] *= a[p];
                                break;
                            case '%':
                                registers[1] %= a[p];
                                break;
                            case '&':
                                registers[1] &= a[p];
                                break;
                            case '|':
                                registers[1] |= a[p];
                                break;
                            case '^':
                                registers[1] ^= a[p];
                                break;
                            case '!':
                                registers[1] = ~registers[1];
                                break;
                            default:
                                printf("Syntax error: Invalid register operation '%c'.\n", c);
                                error = 1;
                                break;
                        }
                    }
                    break;
                case 's':
                    a[p] ^= registers[1];
                    registers[1] ^= a[p];
                    a[p] ^= registers[1];
                    break;
                case 'p':
                    c = fgetc(f[0]);
                    if( c == EOF ) {
                        printf("Error: Scanned to end of file when attempting to identify register to print.");
                    } else {
                        switch(c)
                        {
                            case '0':
                                printf("NULL");
                                break;
                            case '1':
                            case '2':
                            case '3':
                            case '4':
                                printf("%c", registers[c - 48]);
                                break;
                            case '5':
                                printf("%x", extregisters[0]);
                                break;
                            case '6':
                            case '7':
                            case '8':
                            case '9':
                                printf("%d", extregisters[c - 53]);
                                break;
                            default:
                                printf("Error: Unknown register '%c'.", c);
                                break;
                        }
                    }
                    break;

                // Debugging
                case 'h':
                    printf("0x%x", (a[p] >= 0) ? a[p] : 256 + a[p]);
                    break;
                case 'd':
                    printf("%d", (a[p] >= 0) ? a[p] : 256 + a[p]);
                    break;
                case 'v':
                    verbose = (verbose) ? 0 : 1;
                    break;
                case '@':
                    error = ERROR_PROG;
                    break;
                case '?':
                case '!':
                    break;
            }

            if(verbose + superverbose) {
                printf("\n\nNew Stats: %d - %c - %d\n\n", p, a[p], a[p]);
            }
        }
        fclose(f[0]);

        free(a);
        free(f);
    }
    return error;
}
