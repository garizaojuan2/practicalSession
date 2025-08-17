#include <stdio.h>
#include <stdlib.h>

static char rotd(char c, int r){
    if ('A'<=c && c<='Z') return 'A' + (((c-'A')-r+26)%26);
    if ('a'<=c && c<='z') return 'a' + (((c-'a')-r+26)%26);
    return c;
}

int main(int argc, char* argv[]){
    if (argc < 3){ fprintf(stderr,"Uso: %s <rot> <frase...>\n", argv[0]); return 1; }
    int rot = atoi(argv[1]) % 26;
    for (int i=2;i<argc;i++){
        for (char* p=argv[i]; *p; ++p) putchar(rotd(*p, rot));
        if (i<argc-1) putchar(' ');
    }
    putchar('\n');
    return 0;
}
