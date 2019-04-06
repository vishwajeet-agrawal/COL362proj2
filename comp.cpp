#include <iostream>
#include <stdlib.h>

using namespace std;
int main(){
    int tc;
    cin>>tc;
    for(int t=0;t<tc;t++){
        char* str = new char[101];
        cin>>str;
        char* b = new char[101];
        int j=0;
        int i=0;
        while(true){
            if (str[i]==0){
                break;
            }
            else if(str[i]==52){
                str[i]=51;
                b[j++]=49;
            }
            else if (j>0){
                b[j++]=48;
            }
            i++;
        }
        b[j]=0;     
        cout<<"Case #"<<t+1<<": "<<str<<" "<<b<<endl;
    }
}