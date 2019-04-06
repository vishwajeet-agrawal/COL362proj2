//Sample file for students to get their code running

#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>
#include <random>
#include <vector>
#include <list>

#define LowerBoundBS 0
#define UpperBoundBS -1

using namespace std;
void printPage(FileHandler* fh);
void createInput(FileManager& fm, char* filename);
int main() {
	FileManager fm;
	
	// Create a brand new file
	FileHandler fh = fm.CreateFile("mid_value.txt");
	cout << "File created " << endl;

	// Create a new page
	PageHandler ph = fh.NewPage ();
	char *data = ph.GetData ();

	// Store an integer at the very first location
	int num = 5;
	memcpy (&data[0], &num, sizeof(int));

	// Store an integer at the second location
	num = 1000;
	memcpy (&data[4], &num, sizeof(int));

	// Flush the page
	fh.FlushPages ();
	cout << "Data written and flushed" << endl;

	// Close the file
	fm.CloseFile(fh);

	// Reopen the same file, but for reading this time
	fh = fm.OpenFile ("mid_value.txt");
	cout << "File opened" << endl;

	// Get the very first page and its data
	ph = fh.FirstPage ();
	data = ph.GetData ();

	// Output the first integer
	memcpy (&num, &data[0], sizeof(int));
	cout << "First number: " << num << endl;

	// Output the second integer
	memcpy (&num, &data[4], sizeof(int));
	cout << "Second number: " << num << endl;;

	// Close the file and destory it
	fm.CloseFile (fh);
	fm.DestroyFile ("mid_value.txt");
	// fh = fm.OpenFile("sort_output1.txt");
	// printPage(&fh);
	// cout<<fh.LastPage().GetPageNum();
	// ph=fh.LastPage();
	// fm.CloseFile(fh);
	createInput(fm,"test_input1");
	fh = fm.OpenFile("test_input1");
	printPage(&fh);
	
	fm.CloseFile(fh);
	fm.DestroyFile("test_input1");
	return 0;
}

void printPage(FileHandler* fh){
	PageHandler ph = fh->FirstPage();
	char* data = ph.GetData();
	int num;
	for(int i=0;i<PAGE_CONTENT_SIZE;i=i+4){
		memcpy(&num,&data[i],sizeof(int));
		if (num==-2147483648) break;
		cout<<num<<endl;
	}
}
void createInput(FileManager& fm, char* filename){
	FileHandler fh = fm.CreateFile(filename);
	for(int j=0;j<BUFFER_SIZE;j++){
		PageHandler ph = fh.NewPage();
		char* data = ph.GetData();
		for(int i=0,num;i<PAGE_CONTENT_SIZE;i+=4){
			num = rand();
			memcpy(&data[i],&num,sizeof(int));
		}
		// fh.FlushPage(ph.GetPageNum());
	}
	fm.CloseFile(fh);
}

char ValueExistsInPage (PageHandler& ph,int value) {
	char * data = ph.GetData();
	int lo_val; 
	memcpy(&lo_val, &data[0], sizeof(int));
	int hi_val;
	memcpy(&hi_val, &data[PAGE_CONTENT_SIZE-4], sizeof(int));
	if(value>=lo_val && value<=hi_val) return true;
	return false;
}

pair<int,int> BoundBinarySearch (PageHandler& ph, int t, int typeBS) {
	char*data = ph.GetData();
	int lo = 0;
	int hi = (PAGE_CONTENT_SIZE-4)/4;
	int mid = (lo+hi)/2;
	int mid_value;
	while (lo<=hi) {
		memcpy(&mid_value, &data[mid*4], sizeof(int));
		if(lo==hi) {
			if(mid_value==t) return make_pair(ph.GetPageNum(),mid);
			else return make_pair(-1,0);
		}
		if (mid_value<t) lo=mid+1;
		if (mid_value>t) hi=mid-1;
		if (mid_value==t) {
			if(typeBS == LowerBoundBS) {
				hi = mid;
				mid = (lo+hi)/2;
			} else {
				lo = mid;
				mid = (lo+hi+1)/2;
			}
		}					
	}
}

BSResult binarySearch (PageHandler& ph,int t){
	pair<int,int> lb;
	
	lb = BoundBinarySearch(ph,t,LowerBoundBS);
	
	


	//  head of vector is -1 for not found 0 for left 1 for middle 2 for right 
}
struct BSResult{
	BSResult () {}
	BSResult(char x) {
		type = x;
	}
	char type; //L for seek left R for seek right F for finished E for empty l for immediate left, r for immediate right
	list<pair<int,int>> result;
};