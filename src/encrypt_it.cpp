#include <bits/stdc++.h>
using namespace std;

char rotc(char c, int r){
    if ('A'<=c && c<='Z') return char('A' + ((c-'A'+r)%26));
    if ('a'<=c && c<='z') return char('a' + ((c-'a'+r)%26));
    return c;
}
int main(int argc, char* argv[]){
    if (argc < 3){ cerr << "Uso: " << argv[0] << " <rot> <frase...>\n"; return 1; }
    int r = stoi(argv[1]) % 26;
    for (int i=2;i<argc;i++){
        string s = argv[i];
        for (char &c : s) cout << rotc(c, r);
        if (i<argc-1) cout << ' ';
    }
    cout << '\n';
    return 0;
}
