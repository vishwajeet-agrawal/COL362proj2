//Sample file for students to get their code running

#include<iostream>
#include "file_manager.h"
#include "errors.h"
#include<cstring>
#include <random>
#include <vector>
#include <math.h>
#include <list>
#define INT_MIN -2147483648

using namespace std;
void printPage(FileHandler* fh);
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

int main() {
	FileManager fm;
	createInput(fm,"test_input1");
	FileHandler fh = fm.OpenFile("insert_testcase1.txt");
	PageHandler ph = fh.FirstPage();
	MBSResult bsr = megaBinarySearch(fh,2083069270);
	cout<<"lower bound: "<<bsr.lower_bound.first<<" "<<bsr.lower_bound.second<<endl;
	cout<<"upper bound: "<<bsr.upper_bound.first<<" "<<bsr.upper_bound.second<<endl;
	// cout<<bsr.type<<endl;
	// cout<<bsr.result.first<<' '<<bsr.result.second<<endl;
	printPage(&fh);
	FileHandler fh1 = fm.OpenFile("insert_output1.txt");
	printPage(&fh1);
	MBSResult mbsr = megaBinarySearch(fh,10000);
	cout<<"lower bound: "<<mbsr.lower_bound.first<<" "<<mbsr.lower_bound.second<<endl;
	cout<<"upper bound: "<<mbsr.upper_bound.first<<" "<<mbsr.upper_bound.second<<endl;
	fm.CloseFile(fh);
	fm.DestroyFile("test_input1");
	return 0;
}

void printPage(FileHandler* fh){
	PageHandler ph = fh->FirstPage();
	char* data = ph.GetData();
	int num;
	for(int i=0;i<PAGE_CONTENT_SIZE/5;i=i+4){
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
			num = i+1+j*PAGE_CONTENT_SIZE;
			// if (i==60){num=-2147483648;}
			memcpy(&data[i],&num,sizeof(int));
		}
		// fh.FlushPage(ph.GetPageNum());
	}
	fm.CloseFile(fh);
	cout<<"Input created"<<endl;
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


void MergeSort(FileHandler& fh){

}
void PageCopy (PageHandler &source,PageHandler &target) {
	char* source_data = source.GetData();
	char* target_data = target.GetData();
	memcpy(target_data,source_data,PAGE_CONTENT_SIZE);
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