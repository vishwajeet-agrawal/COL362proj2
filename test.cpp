#include<iostream>
using namespace std;
class heap_nway {
    private:
        pair<int,int> node[65];
        int next = 0;
    public:
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

int main() {
    int l;
    cin>>l;
    heap_nway x;
    for(int i=1;i<=l;i++) {
        x.insert(make_pair(5,rand()%60));
        x.printheap();
        
    }
    for(int i=1;i<=l;i++) {
        if(x.isEmpty()) break;
        pair<int,int> t = x.pop_min(); 
        cout<<t.second<<" ";
    }
}