#ifndef BINARY_SEARCH
#define BINARY_SEARCH
#include "file_manager.h"
#include "errors.h"
#include<cstring>
#include <random>
#include <vector>
#include <math.h>
#include<string>
#include<queue>
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
BSResult BoundBinarySearch(PageHandler& ph,int t,char type);
MBSResult megaBinarySearch(FileHandler& fh, int t);
BSResult SearchLastPage (PageHandler& ph, int t,char type);
pair<int,int> boundMegaBinarySearch(FileHandler& fh,int t, char type);
#endif