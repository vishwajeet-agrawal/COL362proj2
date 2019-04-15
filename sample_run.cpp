//Sample file for students to get their code running

#include<iostream>
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

using namespace std;
void createInput(FileManager& fm, char* filename);
void Insertion(FileHandler&, int,FileHandler&);
void Insertion(FileHandler& fh, int t);
int ShiftPage (PageHandler &ph,PageHandler &nph, int index, int value);
int ShiftPage (PageHandler &ph, int index, int value);
void PageCopy (PageHandler &source,PageHandler &target);
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


// Get Value from char array or page
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
// Prints Page -- Works
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
	// cout<<"Total Entries at page:"<<ph.GetPageNum()<<" : "<<i<<endl;
}
// Print File -- Works
void printFile(FileHandler& fh, bool Complete=false) {
	int lastpg = getCountPages(fh)-1;
	PageHandler ph;
	bool sort = true;
	int pgno=-1;
	do {
		// cout<<"Print Page:"<<pgno+1<<" ";
		ph = fh.NextPage(pgno);
		printPage(ph,Complete,sort);
		pgno = ph.GetPageNum();
		fh.UnpinPage(pgno);
	} while(pgno!=lastpg);
	cout<<(sort?"SORTED":"NOT SORTED")<<endl;
} 

void createInput(FileManager& fm,const char* filename,int page,int lastoff){
	int total_num = (page-1)*1023+lastoff;
	int arr[total_num];
	for(int i=0;i<total_num;i++) {
		arr[i]=i+1;
	}
	int k=0;
	random_shuffle(arr,arr+total_num);
	FileHandler fh = fm.CreateFile(filename);
	for(int j=0;j<page;j++){
		PageHandler ph = fh.NewPage();
		char* data = ph.GetData();
		for(int i=0,num;i<PAGE_CONTENT_SIZE/4;i++){
			if(j==page-1 && i==lastoff) {
				int t =INT_MIN;
				memcpy(&data[i*4],&t,sizeof(int));	
				break;
			} else {
				memcpy(&data[i*4],&arr[k],sizeof(int));
				k++;	
			}	
			
		}
		fh.UnpinPage(ph.GetPageNum());
	}
	fh.FlushPages();
	fm.CloseFile(fh);
	cout<<"Input File created with pages,lastpos: "<<page<<","<<lastoff<<endl;
}

pair<int,int> boundMegaBinarySearch(FileHandler& fh, int t,char LU){
	PageHandler pgi = fh.FirstPage();
	PageHandler pgf = fh.LastPage();
	int first_pgn = pgi.GetPageNum();
	int last_pgn = pgf.GetPageNum();
	cout<<"The file has "<<last_pgn-first_pgn+1<<" pages"<<endl;
	//checking the first page

	//check if first page = last page
	if (first_pgn==last_pgn){
		fh.UnpinPage(last_pgn);
		BSResult bsr = SearchLastPage(pgi,t,LU);
		if (bsr.type=='L'){
			return make_pair(last_pgn,0);
		}
		else if(bsr.type=='R'){
			return make_pair(last_pgn+1,0);
		}
		else {
			return bsr.result;
		}
	}

	
	BSResult bsr = BoundBinarySearch(pgi,t,LU);
	fh.UnpinPage(pgi.GetPageNum());
	BSResult& bsref = bsr;
	if (bsref.type=='F'){
		return bsr.result;
	}
	else if(bsref.type=='L'){
		return make_pair(first_pgn,0);
	}
	else if(bsref.type=='R'){
		//checking the last page
		BSResult bsr1 = SearchLastPage(pgf,t,LU);
		fh.UnpinPage(pgf.GetPageNum());
		bsref = bsr1;
		if (bsref.type=='F'){
			return bsref.result;
		}
		else if (bsref.type=='R'){
			return make_pair(last_pgn+1,0);
		}
		else if (bsref.type=='L'){
			//proceed
		}
	}
	// somewhere in the middle
	int low_pg = first_pgn+1;
	int hi_pg = last_pgn -1;
	int mid_pg;
	int page_no;
	int no_pages;
	PageHandler *pha = new PageHandler[BUFFER_SIZE];
	while(true){
		if (hi_pg<low_pg){
			if (bsref.type=='L')
				return make_pair(bsref.result.first,0);
			else if (bsref.type=='R')
				return make_pair(bsref.result.first+1,0);
		}
		else{
			no_pages=0;
			mid_pg = (hi_pg+low_pg)/2;
			page_no = mid_pg;
			try{
				//loading pages into buffer
				while(true){
					if (no_pages==BUFFER_SIZE || no_pages>hi_pg){
						throw exception();
					}
					pha[no_pages]=(fh.PageAt(page_no));
					page_no++;
					no_pages++;	
				}
			}
			catch(...){
				cout<<"No more space in buffer, initiating binary search in pinned pages"<<endl;
				cout<<no_pages<<" pages loaded in buffer"<<endl;
				int hi = no_pages-1;
				int lo = 0;
				int mid;
				pair<int,int> ans;
				//check in first page 
				BSResult bsr1 = BoundBinarySearch(pha[0],t,LU);
				bsref = bsr1;
				if (bsref.type=='F'){
					ans = bsref.result;
				}
				else if(bsref.type=='L'){
					hi_pg=mid_pg-1;
					for(int i=0;i<no_pages;i++){
						fh.UnpinPage(pha[i].GetPageNum());
					}
					continue;
				}
				else if(bsref.type=='R'){
					if (hi==lo){
						ans= make_pair(bsref.result.first+1,0);
					}
					//cheking in last page in buffer
					BSResult bsr1 = BoundBinarySearch(pha[hi],t,LU);
					bsref = bsr1;
					if (bsref.type=='F'){
						ans = bsref.result;
					}
					else if (bsref.type=='R'){
						low_pg = mid_pg+hi+1;
						for(int i=0;i<no_pages;i++){
							fh.UnpinPage(pha[i].GetPageNum());
						}
						continue;
					}
					else{ //bsref.type = 'L'
						lo++;
						hi--;
						while(true){
							if (hi<lo) break;
							mid = (hi+lo)/2;
							BSResult bsr1 = BoundBinarySearch(pha[mid],t,LU);
							bsref = bsr1;
							if (bsref.type=='L'){
								hi = mid-1;
							}
							else if(bsref.type=='R'){
								lo = mid+1;
							}
							else{
								break;
							}
						}
						if (bsref.type=='L'){
							ans = make_pair(bsref.result.first,0);
						}
						else if (bsref.type=='R'){
							ans = make_pair(bsref.result.first+1,0);
						}
						else{//type='F'
							ans = bsref.result;
						}
					}
					
				}
				for(int i=0;i<no_pages;i++){
					fh.UnpinPage(pha[i].GetPageNum());
				}
				return ans;
			}
		}
	}
}
MBSResult megaBinarySearch(FileHandler& fh,int t){
	MBSResult mbsr;
	mbsr.lower_bound = boundMegaBinarySearch(fh,t,'L');
	mbsr.lower_bound.first++;
	mbsr.upper_bound = boundMegaBinarySearch(fh,t,'U');
	mbsr.upper_bound.first++;
	return mbsr;
}
BSResult BoundBinarySearch(PageHandler& ph,int t,char type){
	BSResult bsr;
	char* data = ph.GetData();
	int lo = 0;
	int hi = PAGE_CONTENT_SIZE-4;
	int num;
	int mid;
	
	int bound;
	if (type=='L'){
		memcpy(&num,&data[0],sizeof(int));
		if (num<t){
			bound = 1;
			memcpy(&num,&data[hi],sizeof(int));
			if (num<t){
				bsr.type='R';
				bsr.result = make_pair(ph.GetPageNum(),PAGE_CONTENT_SIZE/4+1);
				return bsr;
			}
			else {
				lo+=4;
				hi-=4;
				while(true){
					if(hi<lo){
						break;
					}
					mid = ((hi/4+lo/4)/2)*4;
					memcpy(&num,&data[mid],sizeof(int));
					if (num<t){
						bound = mid/4+1;
						lo = mid+4;
						continue;
					}
					else{
						hi = mid-4;
						continue;
					}

				}
				bsr.type='F';
				bsr.result = make_pair(ph.GetPageNum(),bound);
				return bsr;
			}	
		}
		else {
			bsr.type='L';
			bsr.result = make_pair(ph.GetPageNum(),0);
			return bsr;
		}
	}
	else {
		memcpy(&num,&data[hi],sizeof(int));
		if (num>t){
		
			bound = hi/4+1;
			memcpy(&num,&data[lo],sizeof(int));
			if (num>t){
				bsr.type='L';
				bsr.result = make_pair(ph.GetPageNum(),0);
				return bsr;
			}
			else {
				lo+=4;
				hi-=4;
				while(true){
					if(hi<lo){
						break;
					}
					mid = ((hi/4+lo/4)/2)*4;
					memcpy(&num,&data[mid],sizeof(int));
					if (num>t){
						bound = mid/4+1;
						hi = mid-4;
						continue;
					}
					else{
						lo = mid+4;
						continue;
					}

				}
				bsr.type='F';
				bsr.result = make_pair(ph.GetPageNum(),bound);
				return bsr;
			}	
		}
		else {
			bsr.type='R';
			bsr.result = make_pair(ph.GetPageNum(),PAGE_CONTENT_SIZE/4+1);
			return bsr;
		}
	}
}

BSResult SearchLastPage (PageHandler& ph, int t,char type) {
	char*data = ph.GetData();
	int pg = ph.GetPageNum();
	int value;
	if(type =='L') {
		for(int i=0;i<PAGE_CONTENT_SIZE/4;i++) {
			memcpy(&value, &data[i*4], sizeof(int));
			if(value==-2147483648) {
				if(i==0) return BSResult('L',pg,i+1);
				else return BSResult('F',pg,i);
			}
			if(t<=value) {
				if(i==0) return BSResult('L',pg,1);
				return BSResult('F',pg,i);

			}
		}
		return BSResult('F',pg,PAGE_CONTENT_SIZE/4);
	} else {
		for(int i=0;i<PAGE_CONTENT_SIZE/4;i++) {
			memcpy(&value, &data[i*4], sizeof(int));
			if(value==-2147483648) {
				if(i==0) return BSResult('L',pg,i+1);
				else return BSResult('F',pg,i+1);
			} 
			if(t<value) {
				if(i==0) return BSResult('L',pg,1);
				else return BSResult('F',pg,i+1);	
			}
		}		
		return BSResult('F',pg,PAGE_CONTENT_SIZE/4+1);
	}
}

void Insertion(FileHandler& fhi, int t,FileHandler& fho){
	pair<int,int> mbsr = boundMegaBinarySearch(fhi,t,'U');
	int pg = mbsr.first;
	int pos = mbsr.second;
	PageHandler ph = fhi.LastPage();
	int last_pgn = ph.GetPageNum();
	PageHandler phi = fhi.FirstPage();
	PageHandler pho;
	fhi.UnpinPage(last_pgn);
	for(int i=0;i<pg;i++){
		pho = fho.NewPage();
		PageCopy(phi,pho);
		fho.DisposePage(pho.GetPageNum());
		fhi.UnpinPage(phi.GetPageNum());
		if (i!=last_pgn)
			phi = fhi.NextPage(i);
	}
	pho = fho.NewPage();
	int val = ShiftPage(phi,pho,pos-1,t);
	fho.DisposePage(pho.GetPageNum());
	fhi.UnpinPage(phi.GetPageNum());
	
	while(true){
		if (++pg>last_pgn){
			if (val == INT_MIN){
				pho = fho.NewPage();

				char* dat = pho.GetData();
				int int_min = INT_MIN;
				memcpy(dat,&int_min,sizeof(int));
				memset(dat+4,0,PAGE_CONTENT_SIZE-4);

				fho.DisposePage(pho.GetPageNum());
				fho.FlushPages();
				return;
			}
			else {
				fho.FlushPages();
				return;
			}
		}
		phi = fhi.PageAt(pg);
		pho = fho.NewPage();
		val = ShiftPage(phi,pho,0,val);
		fhi.UnpinPage(pg);
		fho.DisposePage(pho.GetPageNum());
	}
}

// Copy Page Content 
void PageCopy (PageHandler &source,PageHandler &target) {
	char* source_data = source.GetData();
	char* target_data = target.GetData();
	memcpy(target_data,source_data,PAGE_CONTENT_SIZE);
}

// Get Txt file from number
string getFilename (int i,int mr) {
	return to_string(mr)+"_"+to_string(i)+".txt";
}

// Destroy Txt file from number
void DestroyRun (FileManager& fm, int i) {
	string filename;
	filename = to_string(i)+".txt";
	fm.DestroyFile(filename.c_str());
}

void pushValuetoPage(PageHandler& oph,int index,int value) {
	char* data = oph.GetData();
	memcpy(&data[4*index],&value,sizeof(int));
}

// Tried and tested
class heap_nway {
    private:
        pair<int,int> node[65];
        int next = 0;
    public:
    	int total_insert=0;
        void printheap() {
            cout<<"Print with Nodes:"<<next<<endl;
            for(int i=1;i<=next;i++) {
                cout<<node[i].second<<" ";
            }
            cout<<endl;
        }
        void bubbleup (int i) {
            if(i==1) return;
            if(node[i].second<node[i/2].second) swap(i,i/2);
            bubbleup(i/2);
        }
        void insert(pair<int,int> n) {
            next++;
            node[next] = n;
            total_insert++;
            bubbleup(next);
        }
        void swap(int x,int y) {
            pair<int,int> t = node[x];
            node[x] = node[y];
            node[y] = t;
        }
        // deletes the min from the heap
        pair<int,int> pop_min () {
            pair<int,int> t = node[1];
            swap(1,next);
            next--;
            bubbledown(1);
            return t;
        }
        bool isEmpty () {
		    return next==0;
	    }
       
        void bubbledown (int i) {
            int lc = 2*i;
            if(next<lc) return;
            if(next==lc) {
                if(node[lc].second<node[i].second) swap(lc,i);
            } else {
                int lr = 2*i+1;
                int index = node[lr].second<node[lc].second?lr:lc;
                swap(index,i);
                bubbledown(index);
            }
        }
};

// Contruct heap node from page and index
pair<int,int> heapnode (PageHandler& ph,int page_index,int index) {
	pair<int,int> t;
	char* data = ph.GetData();
	t.first=page_index;
	memcpy(&t.second, &data[sizeof(int)*index],sizeof(int));
	return t;
}


// Merge N way [L,R) runs to a single run
void NwayMerge (FileManager& fm,int L, int R, const char * merge_output, int mr) {
	// File handlers and pages of input 
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
	heap_nway X;
	for(int i=0;i<runs;i++) {
		run_index[i]=0;
		char * data = page[i].GetData();
		int t = valueAt(data,0);
		if(t!=INT_MIN) X.insert(make_pair(i,t));
	}

	cout<<"Sorting through heap";
	// Sorting through heap
	pair<int,int> min_node;
	while(!X.isEmpty()) {
		output_eof++;
		if(output_eof==PAGE_CONTENT_SIZE/4) {
			outputFile.UnpinPage(outputPage.GetPageNum());
			outputPage = outputFile.NewPage();
			output_eof = 0;
		}
		min_node = X.pop_min();
		pushValuetoPage(outputPage,output_eof,min_node.second);   

		// Now take another value from the pages
		int i = min_node.first;
		run_index[i]++;
		if(run_index[i]==PAGE_CONTENT_SIZE/4) {
			int pg = page[i].GetPageNum();
			runfiles[i].UnpinPage(pg);
			// cout<<"Page request"<<i<<pg<<endl;
			try {
				page[i] = runfiles[i].NextPage(pg);
				run_index[i]=0;
				int t = valueAt(page[i],run_index[i]);
				if(t==INT_MIN) {
					int pg = page[i].GetPageNum();
					runfiles[i].UnpinPage(pg);
				} else
					X.insert(make_pair(i,t));					
			} catch (exception e) {
				// Dont do anything
			}
			
		} else {
			int t = valueAt(page[i],run_index[i]);
			if(t!=INT_MIN) {
				X.insert(make_pair(i,t));
			} else {
				int pg = page[i].GetPageNum();
				runfiles[i].UnpinPage(pg);
			}
		}
	}

	cout<<"Total Entries"<<X.total_insert<<endl;

	// put INT_MIN AT THE END OF THE FILE
	if(output_eof!=PAGE_CONTENT_SIZE/4) {
		pushValuetoPage(outputPage,output_eof+1,INT_MIN);
	}
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
	int total_new_runs = ceil(total_runs/39);
	*max_run_size = (BUFFER_SIZE-1)*(*max_run_size);
	int mrs = *max_run_size;
	int Nway = BUFFER_SIZE-1;
	for(int i=1;i<=total_new_runs;i++) {
		int l = Nway*(i-1);
		int r = min(Nway*i,total_runs);
		string file = to_string(mr)+"_"+to_string(i)+".txt";
		NwayMerge(fm,l,r,file.c_str(),mr-1);   // N-way sort [L,R)
	}

	for(int i=total_new_runs+1;i<=total_runs;i++)
		DestroyRun(fm,i);
	return total_new_runs;
}

void MakeRunI (int L, int R, FileHandler& ifh, FileHandler& ofh) {
	int run_size = R-L;
	heap_nway X;
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
		if(t!=INT_MIN) X.insert(make_pair(i-L,t));
	}

	pair<int,int> min_node;
	PageHandler outputPage = ofh.NewPage();

	// X.printheap();
	while(!X.isEmpty()) {
		eof++;
		if(eof==PAGE_CONTENT_SIZE/4) {
			ofh.UnpinPage(outputPage.GetPageNum());
			outputPage = ofh.NewPage();
			eof = 0;
		}
		min_node = X.pop_min();
		pushValuetoPage(outputPage,eof,min_node.second);  

		int i = min_node.first;
		page_index[i]++;
		if(page_index[i]==PAGE_CONTENT_SIZE/4) {
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
				X.insert(make_pair(i,t));
			}
		}
	}
	if(eof!=PAGE_CONTENT_SIZE/4) {
		pushValuetoPage(outputPage,eof+1,INT_MIN);
	}
	ofh.FlushPages();
}

// Sort Complete Page  -- check if works
void SortPage (PageHandler &page) {
	char* data = page.GetData();
	int N = PAGE_CONTENT_SIZE/4;
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

	cout<<"Printing Per Page Sorted File"<<endl;
	printFile(sortedpage);
	int max_Run_Size = BUFFER_SIZE-1;
	int total_runs = ceil(((float)total_pages)/max_Run_Size);
	cout<<"Total Initial Runs "<<total_runs<<endl;
	for(int i=1;i<=total_runs;i++) {
		int l = max_Run_Size*(i-1);
		int r = min(max_Run_Size*i,total_pages);
		cout<<"Run "<<i<<", L,R -> "<<l<<","<<r<<endl;
		FileHandler ofh;
		ofh = fm.CreateFile(getFilename(i,0).c_str());
		cout<<"Created Initial Run Name:"<<getFilename(i,0)<<endl;
		MakeRunI(l,r,sortedpage,ofh);   // merge sort [L,R)
		cout<<"Print Run "<<i<<endl;
		printFile(ofh);
		fm.CloseFile(ofh);
		fm.ClearBuffer();
	}
	fm.CloseFile(sortedpage);
	fm.DestroyFile("sortedpage.txt");
	return total_runs;
}

void MergeSort(FileHandler& fh, FileManager& fm, const char * mergeFilename){
	PageHandler ph = fh.LastPage();
	int total_pages = ph.GetPageNum()+1;
	cout<<"Total Pages:"<<total_pages<<endl;
	fh.UnpinPage(total_pages-1);
	int total_runs = CreateInitialRuns(fh,fm,total_pages);
	int max_run_size = BUFFER_SIZE-1;
	int merge_round=0;
	while(total_runs>BUFFER_SIZE-1) {
		merge_round++;
		total_runs = MergePass(total_runs,&max_run_size,fm,merge_round);
	}
	cout<<"Final N-way merge"<<endl;
	NwayMerge(fm,1,total_runs+1,mergeFilename,merge_round);
}

int ShiftPage (PageHandler &ph,PageHandler &nph, int index, int value) { // indexing starting with 0 // index is upper bound
	char *source_data = ph.GetData();
	char *target_data = nph.GetData();
	int t,r;
	memcpy(&t,&source_data[PAGE_CONTENT_SIZE-4],sizeof(int));
	for(int i=PAGE_CONTENT_SIZE-4;i>index*4;i-=4) {
		memcpy(&target_data[i],&source_data[i-4],sizeof(int));
		memcpy(&r,&target_data[i],sizeof(int));
		if(r==INT_MIN) t = -1;
	}
	memcpy(&target_data[index*4],&value,sizeof(int));
	for(int i = (index-1)*4;i>=0;i=i-4) {
		memcpy(&target_data[i],&source_data[i],sizeof(int));
	}
	return t;
}

int ShiftPage (PageHandler &ph, int index, int value) { // indexing starting with 0 // index is upper bound
	char *data = ph.GetData();
	int t,r;
	bool flag = false;
	memcpy(&t,&data[(PAGE_CONTENT_SIZE-1)*4],sizeof(int));
	for(int i=PAGE_CONTENT_SIZE-1;i>index;i--) {
		memcpy(&data[i*4],&data[(i-1)*4],sizeof(int));
		memcpy(&r,&data[i*4],sizeof(int));
		if(r==INT_MIN) flag = true;
	}
	memcpy(&data[index*4],&value,sizeof(int));
	if(flag==true) return INT_MIN;
	else return t;
} 
void Insertion(FileHandler& fh, int t){
	pair<int,int> mbsr = boundMegaBinarySearch(fh,t,'U');
	int pg = mbsr.first;
	int pos = mbsr.second;
	PageHandler ph = fh.LastPage();
	int last_pgn = ph.GetPageNum();
	fh.UnpinPage(last_pgn);
	if (last_pgn<pg){
		PageHandler ph1 = fh.NewPage();
		ShiftPage(ph1,pos-1,t);
		fh.DisposePage(ph1.GetPageNum());
	}
	else{
		PageHandler ph1 = fh.PageAt(pg);
		int val = ShiftPage(ph1,pos-1,t);
		fh.DisposePage(pg++);
		if (val == INT_MIN){
			fh.FlushPage(pg-1);
			return;
		}
		while(true){
			if (pg>last_pgn){
				break;
			}
			ph1 = fh.PageAt(pg);
			val = ShiftPage(ph1,0,val);
			fh.DisposePage(pg++);
		}
		if (val!=INT_MIN){
			ph1 = fh.NewPage();
			ShiftPage(ph1,0,val);
			fh.DisposePage(ph1.GetPageNum());
		}
		fh.FlushPages();
	}
}

int main() {
	FileManager fm;
	createInput(fm,"test_input1.txt",1000,7);
	// FileHandler fh = fm.OpenFile("test_input1.txt");
	// FileHandler fh = fm.OpenFile("insert_testcase1.txt");
	// PageHandler ph = fh.FirstPage();
	// MBSResult bsr = megaBinarySearch(fh,2083069270);
	// cout<<"lower bound: "<<bsr.lower_bound.first<<" "<<bsr.lower_bound.second<<endl;
	// cout<<"upper bound: "<<bsr.upper_bound.first<<" "<<bsr.upper_bound.second<<endl;
	// cout<<bsr.type<<endl;
	// cout<<bsr.result.first<<' '<<bsr.result.second<<endl;
	// printFile(fh);
	// FileHandler fh1 = fm.OpenFile("insert_output1.txt");
	// printFile(fh1);
	// MBSResult mbsr = megaBinarySearch(fh,10000);
	// cout<<"lower bound: "<<mbsr.lower_bound.first<<" "<<mbsr.lower_bound.second<<endl;
	// cout<<"upper bound: "<<mbsr.upper_bound.first<<" "<<mbsr.upper_bound.second<<endl;
	// fm.CloseFile(fh);
	// fm.DestroyFile("test_input1");

	// MERGE SORT
	FileHandler fh_input = fm.OpenFile("test_input1.txt");
	// printFile(fh_input);
	MergeSort(fh_input,fm,"my_sort_output.txt");
	// printFile(fh_input);
	fm.CloseFile(fh_input);
	// FileHandler fh_output = fm.OpenFile("sort_output2.txt");
	
	// cout<<"Sorted Page"<<endl;
	// FileHandler fh_sorted = fm.OpenFile("sortedpage.txt");
	// printFile(fh_sorted);
	// cout<<"Given Output"<<endl;
	// printFile(fh_output);
	cout<<"My Output"<<endl;
	FileHandler fh_my_output = fm.OpenFile("my_sort_output.txt");
	printFile(fh_my_output);
	fm.DestroyFile("my_sort_output.txt");
	fm.DestroyFile("test_input1.txt");
	return 0;
}
