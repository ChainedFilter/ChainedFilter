#include "include/CuckooHash.h"
#include "include/ChainedFilter.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
using namespace std;

const int N=20000100;
int ff[100];
int main(int argc,char *argv[]){
    int n=1<<19;
    double load=0.4;
    Cuckoo *HT=new Cuckoo(n,3);
    n=n+n;
    for(int i=0;i<load*n;i++){
        int re=HT->insert(make_pair(i,i));
        if(re==-1)puts("-1");
    }
    pair<int,int>re=HT->check();
    printf("%d %d\n",re.first,re.second);
    delete HT;
    HT=new Cuckoo(n/2,3);
    ChainedFilter*filter=new ChainedFilter(re.first,re.second);
    clock_t st;
    st=clock();
    for(int i=0;i<load*n;i++){
        int re=HT->insert(make_pair(i,i));
    }
    for(int i=0;i<load*n;i++){
        auto re=HT->query(i);
    }
    for(int i=0;i<load*n;i++){
        int re=HT->getpos(i);
        filter->insert(i,re);
    }
    double ti=1.0*(clock()-st)/CLOCKS_PER_SEC;
    printf("total item=%d,build time=%lf\n",int(load*n),ti);
    for(int tt=0;tt<=10;tt++){
        int sum=0;
        st=clock();
        for(int i=0;i<load*n;i++){
            int tr=filter->query(i);
            if(HT->query_with_pre(i,tr)){
                sum++;
                filter->change(i,!tr);
            }
        }
        ti=1.0*(clock()-st)/CLOCKS_PER_SEC;
        printf("the %d round,number of mistake=%d,query time=%lf\n",sum,ti);
    }
    return 0;
}