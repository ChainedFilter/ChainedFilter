#include "include/ChainedFilter.h"
#include <cstdio>
#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
using namespace std;

vector<string> items;
unordered_map<string, int>freq;
priority_queue<pair<int,int>,vector<pair<int,int>>,greater<pair<int,int> > >hp;
const int N=30000100;
long long Huffman;
int rt,to[N][2],fa[N];
string name[N];
int code[N];
int id0[N],id1[N],n1,n0;
void getdeep(int x,int deep,int K){
    if(to[x][0]==0){
        Huffman+=1ll*deep*freq[name[x]];
        code[x]=K;
        int sum=freq[name[x]];
        while(K>1){
            if(K&1){
                for(int i=0;i<sum;i++){
                    id1[n1]=n1;
                    n1++;
                }
            }else{
                for(int i=0;i<sum;i++){
                    id0[n0]=n0;
                    n0++;
                }
            }
            K>>=1;
        }
        return;
    }
    getdeep(to[x][0],deep+1,K<<1);
    getdeep(to[x][1],deep+1,(K<<1)|1);
}
void make_log(int K,int MAX_ITEM = 1000000){
    int id=0;
    int fre=1;
    while(id<10){
        id++;
        fre*=K;
        freq[string((char*)(&id), 4)]=fre;
        MAX_ITEM-=min(fre,MAX_ITEM);
    }
}
int main(int argc,char *argv[]){
    int alpha=2;
    sscanf(argv[1],"%d",&alpha);
    make_log(alpha);
    rt=0;
    for(auto x:freq){
        rt++;
        name[rt]=x.first;
        hp.push(make_pair(x.second,rt));
    }
    while(hp.size()>1){
        auto x=hp.top();hp.pop();
        auto y=hp.top();hp.pop();
        rt++;
        to[rt][0]=x.second;
        to[rt][1]=y.second;
        fa[x.second]=fa[y.second]=rt;
        hp.push(make_pair(x.first+y.first,rt));
    }
    getdeep(rt,0,1);
    while(n0<1000000){
        int kk=1000000/n0+1,k=n0*kk;
        while(n0<k){
            id0[n0]=n0;
            n0++;
        }
        k=n1*kk;
        while(n1<k){
            id1[n1]=n1;
            n1++;
        }
    }
    for(int i=0;i<n1;i++)
    id1[i]+=n0+100;
    Huffman=n0+n1;
    printf("memory of Huffman=%lld\n",Huffman);
    printf("%d,%d\n",n0,n1);
    ChainedFilter *ours;
    ours=new ChainedFilter();
    int len=ours->build(id0,n0,id1,n1);
    ours->~ChainedFilter();
    printf("alpha=%d,ChainedFilter use memory=%d\n",alpha,len);
    return 0;
}