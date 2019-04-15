#include <iostream>
#include <cstring>
#include "file_manager.h"
#include "errors.h"
#include <bits/stdc++.h> 
using namespace std;


int main(int argc, const char* argv[]) {
	FileManager fm;
	FileHandler fh;
	PageHandler ph;
	char*data;


    fh = fm.OpenFile(argv[1]);
    int pagenum=0;
    int pageoffset=0;
    int num;
    int i=0;
    vector<int> vec;
    while(true){
        try
        {
            ph = fh.PageAt(pagenum);
        }
        catch(const InvalidPageException& e)
        {
            cout<<"file over"<<endl;
            break;
        }
        
        
        data=ph.GetData();
        while(true){
            memcpy(&num,&data[pageoffset*4],sizeof(int));
            if(num==INT_MIN){
                pagenum+=1;
                pageoffset=0;
                cout<< endl <<"page over\n";
                break;
            }
            vec.push_back(num);
            cout <<num << " ";
            pageoffset+=1;
        }
        fh.UnpinPage(pagenum-1);
    }
    cout << "num pages is "<<pagenum<<endl;
    cout << "num elements is "<<vec.size()<<endl;
    
}