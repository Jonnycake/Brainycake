#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 * @todo Implement loops
 * @todo Add/implement debugging instructions
 * @todo Add/implement registers
 * @todo Add/implement stack
 * @todo Add/implement jumps
 * @todo Add/implement conditionals
 * @todo Add/implement shortcuts
 * @todo Add/implement functions
 * @todo Add/implement fun stuff
 */

// Memory settings
#define INIT_CELL_COUNT   64        // Number of cells to initalize for basic array
#define MAX_CELL_COUNT    256       // Maximum number of cells to allow
#define MAX_STACK_HEIGHT  1024      // Maximum height stack can get to 
#define STACK_WIDTH       4         // Number of bytes per level of the stack
#define MAX_LOOPS         32        // Maximum number of nested loops

// Parsing modes
#define MODE_JUMP       1
#define MODE_COMMENT_SL 2
#define MODE_COMMENT_ML 3
#define MODE_EXEC       4
#define MODE_OUTPUT     5
#define MODE_FUNC_INIT  6
#define MODE_FUNC_DEF   7
#define MODE_INCLUDE    8

int main(int argc, char** argv)
{
	FILE** f = NULL;
	char* a = calloc(sizeof(char), INIT_CELL_COUNT);
	int curCellCount = INIT_CELL_COUNT;
	char c;
	char* mainfile;

	char verbose = 0;
	char superverbose = 0;
	char error = 0;
	char debug = 0;

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
				} else {
					printf("%s\n", argv[x]);
				}
			}
		}

		f = realloc(f, 1 * sizeof(FILE*));

		
		f[0] = fopen(mainfile, "r");
		while( !error
		    && (c = fgetc(f[0])) != EOF
		) {
			if(verbose + superverbose) {
				printf("\n\nINSTRUCTION: %c\nCurrent Stats: %d - %c - %d\n\n", c, p, a[p], a[p]);
			}

			// @todo Refactor to clean up code a bit
			// @todo Add extensions
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
					break;
				case ']':
					break;

				// Comments
				case '#':
					for( ; c != EOF && c != '\n'; c = fgetc(f[0]) ) ;
					break;
				case '/':
					for( c = fgetc(f[0]); c != EOF && c!= '/'; c = fgetc(f[0]) ) ;
					if( c == EOF ) {
						printf("Syntax error: No matching '/'!\n");
					}
					break;

				// String output
				case '\\':
					for( c = fgetc(f[0]); c != EOF && c != '\\'; c = fgetc(f[0]) ) {
						putchar(c);
					}
					if( c == EOF ) {
						printf("Syntax error: No matching '\\'!\n");
					}
					break;

				// Debugging
				case 'h':
				case 'd':
				case 'v':
					verbose = (verbose) ? 0 : 1;
					break;
				case '?':
				case '@':
				case '!':
				case 'p':
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
	return 0;
}
