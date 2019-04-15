#ifndef FUNCTIONS_2
#define FUNCTIONS_2
#include "file_manager.h"
#include "errors.h"
#include<cstring>
#include <random>
#include <vector>
#include <math.h>
#include<string>
#include<algorithm>
#include <list>
#define INT_MIN -2147483648
struct BSResult{
	BSResult(char type='E',int pgn=0, int pos=0):type(type),result(make_pair(pgn,pos)){};
	char type; //L for seek left R for seek right F for finished E for empty
	pair<int,int> result;
	BSResult& operator=(BSResult& bsr1){type = bsr1.type; result = bsr1.result;return *this;}
};
struct MBSResult{
	pair<int,int> lower_bound; 
	pair<int,int> upper_bound;
};
void createInput(FileManager& fm, char* filename);
void Insertion(FileHandler&, int,FileHandler&);
void Insertion(FileHandler& fh, int t);
pair<int,bool> ShiftPage (PageHandler &ph,PageHandler &nph, int index, int value);
pair<int,bool> ShiftPage (PageHandler &ph, int index, int value);
void PageCopy (PageHandler &source,PageHandler &target);
BSResult BoundBinarySearch(PageHandler& ph,int t,char type);
MBSResult megaBinarySearch(FileHandler& fh, int t);
BSResult SearchLastPage (PageHandler& ph, int t,char type);
pair<int,int> boundMegaBinarySearch(FileHandler& fh,int t, char type);
void createSortedInput(FileManager& fm,const char* filename,int page,int lastoff);
void insertion_test(FileManager& fm);
void printFile(FileHandler& fh, bool Complete=false);

int valueAt(char *data,int index);
int valueAt(PageHandler ph,int index);
int getCountPages (FileHandler& fh) ;
void printPage(PageHandler& ph,bool print,bool& sort);
void createInput(FileManager& fm,const char* filename,int page,int lastoff);
void createSortedInput(FileManager& fm,const char* filename,int page,int lastoff);

string getFilename (int i,int mr) ;
void DestroyRun (FileManager& fm, int i);
void pushValuetoPage(PageHandler& oph,int index,int value);

void NwayMerge (FileManager& fm,int L, int R, const char * merge_output, int mr);
int MergePass (int total_runs, int* max_run_size, FileManager& fm, int mr);
void MakeRunI (int L, int R, FileHandler& ifh, FileHandler& ofh);
void SortPage (PageHandler &page);
void SortLastPage (PageHandler &page);
void SortAndCopyFile (FileHandler& output, FileHandler& input);
int CreateInitialRuns (FileHandler& input_file,FileManager& fm,int total_pages);

void MergeSort(FileHandler& fh, FileManager& fm, const char * mergeFilename);

pair<int,int> heapnode (PageHandler& ph,int page_index,int index) ;
#endif