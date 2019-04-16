#include "binary_search.h"
#include <fstream>
#include <algorithm>
#include <iostream>
#define MAX_ARRAY_SIZE 100000
void InsertArray(int* input, int size, FileHandler& fh,FileManager& fm);
pair<int,bool> ShiftPage (PageHandler &ph, int index, int value);
void Insertion(FileHandler& fh, int t);
// int Insertion(FileHandler& fh, int t, int fv, int fpgn);
pair<int,int> boundMegaBinarySearch(FileHandler& fh, int t,char LU,int first_pgn);
using namespace std;
int main(int argc, char* argv[]){
    if (argc<3){
        return 1;
    }
    FileManager fm;
    FileHandler fh = fm.OpenFile(argv[1]);
    int counter=0;
    fstream fs;
    fs.open(argv[2]);
    int* input = new int[MAX_ARRAY_SIZE];
    while(true){
        while(counter<MAX_ARRAY_SIZE && !fs.eof()){
            fs>>*(input+counter);
            counter++;
        }
        InsertArray(input,counter,fh,fm);
        if(counter==MAX_ARRAY_SIZE){
            counter=0;
        }
        else{
            break;
        }
    }
}
void InsertArray(int* input, int size, FileHandler& fh,FileManager& fm){
    sort(input,input+size);
    for(int i=0;i<size;i++){
        Insertion(fh,input[i]);
        fm.ClearBuffer();
    }
}
pair<int,bool> ShiftPage (PageHandler &ph, int index, int value) { // indexing starting with 0 // index is upper bound
	char *data = ph.GetData();
	int t,r;
	bool flag = false;
	memcpy(&t,&data[(PAGE_CONTENT_SIZE-8)],sizeof(int));
	for(int i=(PAGE_CONTENT_SIZE-8)/4;i>index;i--) {
		memcpy(&data[i*4],&data[(i-1)*4],sizeof(int));
		memcpy(&r,&data[i*4],sizeof(int));
		if(r==INT_MIN) flag = true;
	}
	if (t==INT_MIN) {memcpy(&data[PAGE_CONTENT_SIZE-4],&t,sizeof(int));
					flag==true;}
	memcpy(&data[index*4],&value,sizeof(int));
	if(flag==true) return (make_pair(t,false));
	else return make_pair(t,true);
} 
// int Insertion(FileHandler& fh, int t, int fv, int fpgn){
//     pair<int,int> mbsr = boundMegaBinarySearch(fh,t,'U',fpgn);
//     int pg = mbsr.first;
// 	int pos = mbsr.second;
// 	PageHandler ph = fh.LastPage();

// 	int last_pgn = ph.GetPageNum();
// 	fh.UnpinPage(last_pgn);
// 	fh.FlushPages();
// 		PageHandler ph1 = fh.NextPage(pg-1);
// 		pair<int,bool> val = ShiftPage(ph1,pos-1,t);
// 		fh.MarkDirty(pg++);
//         fh.UnpinPage(pg-1);
// 		// fh.UnpinPage(pg++);
// 		// fh.UnpinPage(pg++);
// 		// fh.FlushPage(0);
		
// 		// ph1 = fh.FirstPage();

// 		while(true){
// 			if (pg>last_pgn){
// 				break;
// 			}
// 			ph1 = fh.PageAt(pg);
// 			val = ShiftPage(ph1,0,val.first);
// 			fh.MarkDirty(pg++);
//             fh.UnpinPage(pg-1);
// 			// fh.FlushPage(pg-1);
// 		}
// 		if (val.second==true){
// 			ph1 = fh.NewPage();
// 			ShiftPage(ph1,0,val.first);
// 			fh.MarkDirty(ph1.GetPageNum());
//             fh.UnpinPage(ph1.GetPageNum());
// 		}
// 		fh.FlushPages();
// }
void Insertion(FileHandler& fh, int t){
	pair<int,int> mbsr = boundMegaBinarySearch(fh,t,'U');
	int pg = mbsr.first;
	int pos = mbsr.second;
	PageHandler ph = fh.LastPage();

	int last_pgn = ph.GetPageNum();
	fh.UnpinPage(last_pgn);
	fh.FlushPages();
		PageHandler ph1 = fh.NextPage(pg-1);
		pair<int,bool> val = ShiftPage(ph1,pos-1,t);
		fh.MarkDirty(pg++);
        fh.UnpinPage(pg-1);
		// fh.UnpinPage(pg++);
		// fh.UnpinPage(pg++);
		// fh.FlushPage(0);
		
		// ph1 = fh.FirstPage();

		while(true){
			if (pg>last_pgn){
				break;
			}
			ph1 = fh.PageAt(pg);
			val = ShiftPage(ph1,0,val.first);
			fh.MarkDirty(pg++);
            fh.UnpinPage(pg-1);
			// fh.FlushPage(pg-1);
		}
		if (val.second==true){
			ph1 = fh.NewPage();
			ShiftPage(ph1,0,val.first);
			fh.MarkDirty(ph1.GetPageNum());
            fh.UnpinPage(ph1.GetPageNum());
		}
		fh.FlushPages();
	
}

pair<int,int> boundMegaBinarySearch(FileHandler& fh, int t,char LU){
	PageHandler pgi = fh.FirstPage();
	PageHandler pgf = fh.LastPage();
	int first_pgn = pgi.GetPageNum();
	int last_pgn = pgf.GetPageNum();
	// cout<<"The file has "<<last_pgn-first_pgn+1<<" pages"<<endl;
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
				return make_pair(bsref.result.first,PAGE_CONTENT_SIZE/4);
		}
		else{
			no_pages=0;
			mid_pg = (hi_pg+low_pg)/2;
			page_no = mid_pg;
			try{
				//loading pages into buffer
				while(true){
					if (no_pages==BUFFER_SIZE || page_no>hi_pg){
						throw exception();
					}
					pha[no_pages]=(fh.PageAt(page_no));
					page_no++;
					no_pages++;	
				}
			}
			catch(...){
				// cout<<"No more space in buffer, initiating binary search in pinned pages"<<endl;
				// cout<<no_pages<<" pages loaded in buffer"<<endl;
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
						ans= bsref.result;
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

BSResult BoundBinarySearch(PageHandler& ph,int t,char type){
	BSResult bsr;
	char* data = ph.GetData();
	int lo = 0;
	int hi = PAGE_CONTENT_SIZE-4-4;
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
			bsr.result = make_pair(ph.GetPageNum(),PAGE_CONTENT_SIZE/4);
			return bsr;
		}
	}
}

BSResult SearchLastPage (PageHandler& ph, int t,char type) {
	char*data = ph.GetData();
	int pg = ph.GetPageNum();
	int value;
	if(type =='L') {
		for(int i=0;i<PAGE_CONTENT_SIZE/4-1;i++) {
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
		for(int i=0;i<PAGE_CONTENT_SIZE/4-1;i++) {
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
		return BSResult('F',pg,PAGE_CONTENT_SIZE/4-1);
	}
}