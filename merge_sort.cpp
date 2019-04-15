#include "functions.h"
#include <vector>
#include <iostream>
#include <queue>
#include <string>
using namespace std;
typedef pair<int,int> pi;

int main(int argc, char** argv) {
	FileManager fm;
    if(argc>3) return 0;
    string input(argv[1]);
    MergeSort(argv[1],fm,argv[2]);
	return 0;
}

int valueAt(char *data,int index) {
	int t; memcpy(&t,&data[index*4],sizeof(int));
	return t;
}
int valueAt(PageHandler ph,int index) {
	char * data = ph.GetData();
	return valueAt(data,index);
}
int getCountPages (FileHandler& fh) {
	int lastpg = fh.LastPage().GetPageNum();
	fh.UnpinPage(lastpg);
	return lastpg+1;
}
void printPage(PageHandler& ph,bool print,bool& sort) {
	char* data = ph.GetData();
	int first = INT_MIN;
	int num,i;
	for(i=0;i<PAGE_CONTENT_SIZE/4;i++){
		memcpy(&num,&data[i*4],sizeof(int));
		if (num==INT_MIN) break;
		if(num<first) sort=false;
		first = num;
		if(print) cout<<num<<", ";
	}
	cout<<"Total Entries at page:"<<ph.GetPageNum()<<" : "<<i<<endl;
}
void printFile(FileHandler& fh, bool Complete) {
	int lastpg = getCountPages(fh)-1;
	PageHandler ph;
	bool sort = true;
	int pgno=-1;
	do {
		ph = fh.NextPage(pgno);
		printPage(ph,Complete,sort);
		pgno = ph.GetPageNum();
		fh.UnpinPage(pgno);
	} while(pgno!=lastpg);
	cout<<(sort?"SORTED":"NOT SORTED")<<endl;
} 
void PageCopy (PageHandler &source,PageHandler &target) {
	char* source_data = source.GetData();
	char* target_data = target.GetData();
	memcpy(target_data,source_data,PAGE_CONTENT_SIZE);
}
// Get Txt file from number
string getFilename (int i,int mr) {
	return std::to_string(mr)+"_"+to_string(i)+".txt";
}
void pushValuetoPage(PageHandler& oph,int index,int value) {
	char* data = oph.GetData();
	memcpy(&data[4*index],&value,sizeof(int));
}
// Merge N way [L,R) runs to a single run
void NwayMerge (FileManager& fm,int L, int R, const char * merge_output, int mr) {
	fm.ClearBuffer();
	int runs = R-L;
	FileHandler runfiles[runs];
	PageHandler page[runs];
	int run_index[runs];
	for(int i=L;i<R;i++) {
		string filename = getFilename(i,mr);
		cout<<filename<<endl;
		runfiles[i-L] = fm.OpenFile(filename.c_str());
		page[i-L] = runfiles[i-L].FirstPage();
	}

	// file handler and page of output
	int output_eof = -1;
	FileHandler outputFile = fm.CreateFile(merge_output);
	PageHandler outputPage = outputFile.NewPage();
	// Initializing the heap
	priority_queue <pi, vector<pi>, greater<pi> > X;
	for(int i=0;i<runs;i++) {
		run_index[i]=0;
		char * data = page[i].GetData();
		int t = valueAt(data,0);
		if(t!=INT_MIN) X.push(make_pair(t,i));
	}

	// std::cout<<"Sorting through heap";
	pair<int,int> min_node;
	while(!X.empty()) {
		output_eof++;
		if(output_eof==PAGE_CONTENT_SIZE/4-1) {
			pushValuetoPage(outputPage,output_eof,INT_MIN);
			outputFile.UnpinPage(outputPage.GetPageNum());
			outputPage = outputFile.NewPage();
			output_eof = 0;
		}
		min_node = X.top(); X.pop();
		pushValuetoPage(outputPage,output_eof,min_node.first);   

		int i = min_node.second;
		run_index[i]++;
		if(run_index[i]==PAGE_CONTENT_SIZE/4-1) {
			int pg = page[i].GetPageNum();
			runfiles[i].UnpinPage(pg);
			try {
				page[i] = runfiles[i].NextPage(pg);
				run_index[i]=0;
				int t = valueAt(page[i],run_index[i]);
				if(t==INT_MIN) {
					int pg = page[i].GetPageNum();
					runfiles[i].UnpinPage(pg);
				} else
					X.push(make_pair(t,i));					
			} catch (exception e) {
				// Dont do anything
			}
		} else {
			int t = valueAt(page[i],run_index[i]);
			if(t!=INT_MIN) {
				X.push(make_pair(t,i));
			} else {
				int pg = page[i].GetPageNum();
				runfiles[i].UnpinPage(pg);
			}
		}
	}

	// put INT_MIN AT THE END OF THE FILE
	// if(output_eof!=PAGE_CONTENT_SIZE/4) {
		pushValuetoPage(outputPage,output_eof+1,INT_MIN);
	// }
	// UNPIN, FLUSH AND CLOSE FILE
	outputFile.UnpinPage(outputPage.GetPageNum());
	outputFile.FlushPages();
	// printFile(outputFile);
	fm.CloseFile(outputFile);

	// Clearing all the remaining files and buffer
	for(int i=L;i<R;i++) {
		string filename = getFilename(i,mr);
		fm.CloseFile(runfiles[i-L]);
		fm.DestroyFile(filename.c_str());
	}
	fm.ClearBuffer();
}
int MergePass (int total_runs, int* max_run_size, FileManager& fm, int mr) {
	int total_new_runs = ceil(((float)total_runs)/(BUFFER_SIZE-1));
	*max_run_size = (BUFFER_SIZE-1)*(*max_run_size);
	int mrs = *max_run_size;
	int Nway = BUFFER_SIZE-1;
	// cout<<total_runs<<total_new_runs<<" "<<*max_run_size<<" "<<endl;
	for(int i=0;i<total_new_runs;i++) {
		int l = Nway*(i);
		int r = min(Nway*(i+1),total_runs);
		string file = getFilename(i,mr);
		cout<<"N way sort "<<file<<endl;
		NwayMerge(fm,l,r,file.c_str(),mr-1);   // N-way sort [L,R)
	}

	for(int i=0;i<total_runs;i++) {
		string file = getFilename(i,mr-1);
		fm.DestroyFile(file.c_str());
	}
	return total_new_runs;
}
void MakeRunI (int L, int R, FileHandler& ifh, FileHandler& ofh) {
	int run_size = R-L;
	priority_queue<pi, vector<pi>, greater<pi> > X;
	PageHandler pages[run_size];
	int page_index[run_size];
	int eof = -1;
	cout<<"Run-size: "<<R-L<<endl;
	for(int i=L;i<R;i++) {
		// cout<<"Page Requested: "<<i<<endl;
		pages[i-L] = ifh.PageAt(i);
		page_index[i-L] = 0;
		char * data = pages[i-L].GetData();
		int t = valueAt(data,0);
		if(t!=INT_MIN) X.push(make_pair(t,i-L));
	}

	pair<int,int> min_node;
	PageHandler outputPage = ofh.NewPage();

	// X.printheap();
	while(!X.empty()) {
		eof++;
		if(eof==PAGE_CONTENT_SIZE/4-1) {
			pushValuetoPage(outputPage,eof,INT_MIN);
			ofh.UnpinPage(outputPage.GetPageNum());
			outputPage = ofh.NewPage();
			eof = 0;
		}
		min_node = X.top();
		X.pop();
		pushValuetoPage(outputPage,eof,min_node.first);  

		int i = min_node.second;
		page_index[i]++;
		if(page_index[i]==PAGE_CONTENT_SIZE/4-1) {
			int pg = pages[i].GetPageNum();
			ifh.UnpinPage(pg);
		}
		else {
			// cout<<"Putting New value from page "<<i<<endl;
			int t = valueAt(pages[i],page_index[i]);
			if(t==INT_MIN) {
				int pg = pages[i].GetPageNum();
				ifh.UnpinPage(pg);
				// cout<<"Page unpinned "<<i<<endl;
			} else {
				X.push(make_pair(t,i));
			}
		}
	}
	// if(eof!=PAGE_CONTENT_SIZE/4-1) {
	pushValuetoPage(outputPage,eof+1,INT_MIN);
	// }
	ofh.FlushPages();
}
// Sort Complete Page  -- check if works
void SortPage (PageHandler &page) {
	char* data = page.GetData();
	int N = (PAGE_CONTENT_SIZE/4)-1;
	int * A = (int*) data;
	sort(A,A+N);
}
// Sort Last Page
void SortLastPage (PageHandler &page) {
	int i;
	for(i=0;i<PAGE_CONTENT_SIZE/4;i++) {
		int t = valueAt(page,i);
		if(t==INT_MIN) break;
	}
	int*A = (int*) page.GetData();
	sort(A,A+i);
}
// Copy File and Sort Each Page
void SortAndCopyFile (FileHandler& output, FileHandler& input) {
	PageHandler ph = input.LastPage();
	int lastpg = ph.GetPageNum();
	input.UnpinPage(lastpg);
	ph =  input.FirstPage();
	while(true) {
		PageHandler sph = output.NewPage();
		PageCopy(ph,sph);
		int in_pg = ph.GetPageNum();
		int ou_pg = sph.GetPageNum();
		if(in_pg!=lastpg)
			SortLastPage(sph);
		else {
			SortLastPage(sph);
			break;
		}
		// output page as dirty and unpin
		output.MarkDirty(ou_pg);
		output.UnpinPage(ou_pg);

		// unpin input page and Call next page
		input.UnpinPage(in_pg);
		ph = input.NextPage(in_pg);
	}
	output.FlushPages();
}
// Create Intital Runs as txt file
int CreateInitialRuns (FileHandler& input_file,FileManager& fm,int total_pages) {
	cout<<"Creating Initial Runs"<<endl;
	FileHandler sortedpage;
	try {
		sortedpage = fm.CreateFile("sortedpage.txt");	
		SortAndCopyFile(sortedpage,input_file);
		fm.ClearBuffer();
	} catch (exception e) {
		sortedpage = fm.OpenFile("sortedpage.txt");
	}

	int max_Run_Size = BUFFER_SIZE-1;
	int total_runs = ceil(((float)total_pages)/max_Run_Size);
	cout<<"Total Initial Runs "<<total_runs<<endl;
	for(int i=0;i<total_runs;i++) {
		int l = max_Run_Size*i;
		int r = min(max_Run_Size*(i+1),total_pages);
		FileHandler ofh;
		ofh = fm.CreateFile(getFilename(i,0).c_str());
		// cout<<"Run "<<i<<", L,R -> "<<l<<","<<r<<"Created Initial Run Name:"<<getFilename(i,0)<<endl;
		MakeRunI(l,r,sortedpage,ofh);   // merge sort [L,R)
		// printFile(ofh);
		fm.CloseFile(ofh);
		fm.ClearBuffer();
	}
	fm.CloseFile(sortedpage);
	fm.DestroyFile("sortedpage.txt");
	return total_runs;
}
void MergeSort(const char * inputfilename, FileManager& fm, const char * mergeFilename){
    FileHandler fh = fm.OpenFile(inputfilename);
	PageHandler ph = fh.LastPage();
	int total_pages = ph.GetPageNum()+1;
	cout<<"Total Pages:"<<total_pages<<endl;
	fh.UnpinPage(total_pages-1);
	int total_runs = CreateInitialRuns(fh,fm,total_pages);
	int max_run_size = BUFFER_SIZE-1;
	int merge_round=0;
	while(total_runs>BUFFER_SIZE-1) {
		merge_round++;
		cout<<"Merge Pass "<<merge_round<<endl;
		total_runs = MergePass(total_runs,&max_run_size,fm,merge_round);
	}
	cout<<"Final N-way merge"<<endl;
	NwayMerge(fm,0,total_runs,mergeFilename,merge_round);
    fm.CloseFile(fh);
}

