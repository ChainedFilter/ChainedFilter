#include "include/ChainedFilter.h"
#include <cstdio>
using namespace std;
const int seed[]={3,5,13,17,53,67,97,193,257,7681,12289,40961,65537,786433,5767169,7340033,23068673,104857601,167772161,469762049,1004535809};
int id0[1ll<<27],id1[1ll<<27],value[1ll<<27],n,n0,n1,k;
int main(int argc,char *argv[]){
    ChainedFilter *ours,*tr;
    n=1<<20;
    for(k=2;k<=16;k++){
        n1=n;
        n0=n*k;
        for(int i=0;i<n0;i++)
        id0[i]=i+1;
        for(int i=0;i<n1;i++)
        id1[i]=i+n0+1;
        ours=new ChainedFilter();
        int len=ours->build(id1,n1,id0,n0);
        ours->~ChainedFilter();
        printf("when k=%d,ChainedFilter use memory=%d\n",k,len);
        if(len<0)continue;
        int down=len*0.95,up=len*1.02;
        for(int len1=down;len1<=up;len1+=(up-down)/8){
            int sum=0,win=0;
            for(int k=0;k<10;k++){
                tr=new ChainedFilter(seed[k]);
                int re=tr->build(id1,n1,id0,n0,len1);
                if(re>0)win++;
                sum++;
                tr->~ChainedFilter();
            }
            printf("when memory=%d,probability of construction success=%d/10\n",len1,win);
        }
    }
    return 0;
}