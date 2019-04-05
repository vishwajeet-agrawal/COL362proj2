//Sample file for students to get their code running

#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>
#include <random>
using namespace std;
void printPage(FileHandler* fh);
void createInput(FileManager& fm, char* filename);
int main() {
	FileManager fm;
	
	// Create a brand new file
	FileHandler fh = fm.CreateFile("temp.txt");
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
	fh = fm.OpenFile ("temp.txt");
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
	fm.DestroyFile ("temp.txt");
	// fh = fm.OpenFile("sort_output1.txt");
	// printPage(&fh);
	// cout<<fh.LastPage().GetPageNum();
	// ph=fh.LastPage();
	// fm.CloseFile(fh);
	createInput(fm,"test_input1");
	fh = fm.OpenFile("test_input1");
	printPage(&fh);
	cout<<fh.hdr.firstFreePage;
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
		fh.FlushPage(ph.GetPageNum());
	}
	fm.CloseFile(fh);
}