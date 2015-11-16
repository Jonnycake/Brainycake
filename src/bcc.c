#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <brainycake.h>

int main(int argc, char** argv)
{
    char* command = "gcc -o a.out ";
    char* header = "#include <stdio.h>\nint main() \n{\nchar a[256] = {0}; int p = 0;";
    char* footer = "\n}";
    char** translations;
    translations = calloc(sizeof(char*), 13);
    translations[0] = "p++;";
    translations[1] = "p--;";
    translations[2] = "a[p]++;";
    translations[3] = "a[p]--;";
    translations[4] = "putchar(a[p]);";
    translations[5] = "a[p] = getchar();";
    translations[6] = "while(a[p]) {";
    translations[7] = "}";
    translations[8] = "a[p] += %d;";
    translations[9] = "a[p] = 0;";
    translations[10] = "p = 0;";
    translations[11] = "printf(\"%d\", a[p]);";
    translations[12] = "printf(\"0x%x\", a[p]);";
    FILE* f;
    char* mainfile;
    char* code;
    char* c_code;
    int bc_codelen;
    if( argc > 1 ) {
        int x, top;
        mainfile = argv[argc - 1];
        char* newfile = calloc(sizeof(char), strlen(mainfile) + 3);
        char* fullcommand = calloc(sizeof(char), strlen(command) + MAX_FILENAME + 1);
        strcat(newfile, mainfile);
        strcat(newfile, ".c");
        strcat(fullcommand, command);
        strcat(fullcommand, newfile);
        bc_preprocess(mainfile, &code);
        bc_codelen = strlen(code);
        c_code = calloc(sizeof(char), bc_codelen * 23 + strlen(header) + strlen(footer));
        strcat(c_code, header);
        for(x = 0; x < bc_codelen ; x++) {
            top = strlen(c_code);
            switch(code[x])
            {
                case '>':
                    strcat(&c_code[top], translations[0]);
                    break;
                case '<':
                    strcat(&c_code[top], translations[1]);
                    break;
                case '+':
                    strcat(&c_code[top], translations[2]);
                    break;
                case '-':
                    strcat(&c_code[top], translations[3]);
                    break;
                case '.':
                    strcat(&c_code[top], translations[4]);
                    break;
                case ',':
                    strcat(&c_code[top], translations[5]);
                    break;
                case '[':
                    strcat(&c_code[top], translations[6]);
                    break;
                case ']':
                    strcat(&c_code[top], translations[7]);
                    break;
                case '|':
                    // @todo add extra logic for bringing integer into code
                    break;
                case '^':
                    strcat(&c_code[top], translations[9]);
                    break;
                case '_':
                    strcat(&c_code[top], translations[10]);
                    break;
                case 'd':
                    strcat(&c_code[top], translations[11]);
                    break;
                case 'h':
                    strcat(&c_code[top], translations[12]);
                    break;
            }
        }
        strcat(c_code, footer);
        f = fopen(newfile, "w+");
        fwrite(c_code, 1, strlen(c_code), f);
        fclose(f);
        system(fullcommand);
        free(c_code);
        free(fullcommand);
        free(code);
    }

    return 0;
}

