#include <iostream>
#include <stdlib.h>
using namespace std;
void print_path(char* str, int h, int l);
int main(){
    int tc;
    cin>>tc;
    for(int t=1;t<=tc;t++){
        int n;
        cin>>n;
        char* str = new char[2*n-1];
        cin>>str;
        cout<<"Case #"<<t<<": ";
        print_path(str,n-1,n-1);
        cout<<endl;
    }
}
void print_path(char* str, int h,int l){
    if (h==0 || l==0)
        return;
    if (str[0]=='E' && str[h+l-1]=='S'){
        for(int i=0;i<h;i++){
            cout<<'S';
        }
        for(int i=h;i<h+l;i++){
            cout<<'E';
        }
    }
    else if (str[0]=='S' && str[h+l-1]=='E'){
        for(int i=0;i<l;i++){
            cout<<'E';
        }
        for(int i=l;i<h+l;i++){
            cout<<'S';
        }
    }
    else if(str[0]=='S'){
        cout<<'E';
        int j=1;
        while(str[j++]=='S'){
            cout<<'S';
        }
        cout<<'S';
        print_path(str+j,h-j+1,l-1);
    }
    else if (str[0]=='E'){
        cout<<'S';
        int j=1;
        while(str[j++]=='E'){
            cout<<'E';
        }
        cout<<'E';
        print_path(str+j,h-1,l-j+1);
    }
}