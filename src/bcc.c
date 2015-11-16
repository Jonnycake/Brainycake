#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <brainycake.h>

int main(int argc, char** argv)
{
    char* command = "gcc -o a.out ";
    char* header = "int main() \n{\nchar a[256] = {0}; int p = 0;";
    char* footer = "\n}";
    char** translations;
    translations = calloc(sizeof(char*), 8);
    translations[0] = "p++;";
    translations[1] = "p--;";
    translations[2] = "a[p]++;";
    translations[3] = "a[p]--;";
    translations[4] = "write(1, &a[p], 1);";
    translations[5] = "read(stdin, &a[p], 1);";
    translations[6] = "while(a[p]) {";
    translations[7] = "}";
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
        c_code = calloc(sizeof(char), bc_codelen * 13 + strlen(header) + strlen(footer));
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
            }
        }
        strcat(c_code, footer);
        f = fopen(newfile, "w+");
        fwrite(c_code, 1, strlen(c_code), f);
        fclose(f);
        free(c_code);
        printf("%s %s\n", fullcommand, newfile);
        system(fullcommand);
    }
}

