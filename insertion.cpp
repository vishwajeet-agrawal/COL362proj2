#include "functions.h"
#include <fstream>
#include <algorithm>
#define MAX_ARRAY_SIZE 100000
void InsertArray(int* input, int size, FileHandler& fh);
using namespace std;
int main(int argc, char* argv[]){
    if (argc<3){
        return 1;
    }
    FileManager fm;
    FileHandler fh = fm.OpenFile(argv[1]);
    int counter=0;
    fstream fs;
    fs.open(argv[2]);
    int* input = new int[MAX_ARRAY_SIZE];
    while(true){
        while(counter<MAX_ARRAY_SIZE && !fs.eof()){
            fs>>*(input+counter);
            counter++;
        }
        InsertArray(input,counter,fh);
        if(counter==MAX_ARRAY_SIZE){
            counter=0;
        }
        else{
            break;
        }
    }
}
void InsertArray(int* input, int size, FileHandler& fh){
    sort(input,input+size);
}