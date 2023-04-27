#include "MurmurHash.h"
#include "Bloomier.h"
#include <cstdio>
#include <cmath>
#include <ctime>
using namespace std;
const int chooseseed[11]={7,13,17,23,47,51,53,1000007,97,93};
class ChainedFilter{
	int mem1,mem2,seed;
    Bloomier*approximate;
    Bloomier*exact;
    public:
    ChainedFilter(int _hashseed=1e9+7){
        seed=_hashseed;
    }
    void changeseed(int _hashseed=1e9+7){
        seed=_hashseed;
    }
    int build(int*id1,int n1,int*id0,int n0,int fix_mem=-1){
        double lambda=1.0*n0/n1;
        int fingerprintlen=floor(log(lambda)/log(2));
        approximate=new Bloomier(fingerprintlen,3,seed,seed^19260817);
        int *id3=new int[n0+n1];
        int *value=new int[n0+n1];
        for(int i=0;i<n1;i++)value[i]=1;
        clock_t st,st2;
		st=clock();
        mem1=approximate->build(id1,value,n1);
        for(int i=0;mem1<0&&i<5;i++){
            approximate->changeseed(chooseseed[i]);
            mem1=approximate->build(id1,value,n1);
        }
        if(mem1<0){
            return -1;
        }
        double ti1=1.0*(clock()-st)/CLOCKS_PER_SEC;
        int n3=0;
        for(int i=0;i<n1;i++){
            int k=approximate->query(id1[i]);
            if(k!=1)return -2;
            id3[n3]=id1[i];
            value[n3]=1;
            n3++;
        }
        for(int i=0;i<n0;i++){
            int k=approximate->query(id0[i]);
            if(k!=1)continue;
            id3[n3]=id0[i];
            value[n3]=0;
            n3++;
        }
        exact=new Bloomier(1,3,seed^1000000007,seed^998244353);
        if(fix_mem>0)
        mem2=fix_mem-mem1;
        else mem2=-1;
		st2=clock();
        int re=exact->build(id3,value,n3,mem2);
        for(int i=5;re<0&&i<10;i++){
            exact->changeseed(chooseseed[i]);
            re=exact->build(id3,value,n3,mem2);
        }
        if(re<0){
            return -3;
        }
        if(fix_mem>0&&mem1>=fix_mem)
            return -1;
        double ti2=1.0*(clock()-st2)/CLOCKS_PER_SEC;
        double ti=1.0*(clock()-st)/CLOCKS_PER_SEC;
        // fprintf(stderr,"n=,%d,n0=,%d,n1=,%d,n3=,%d,mem1=,%d,mem2=,%d,total mem=,%d,",n0+n1,n0,n1,n3,mem1,re,mem1+re);
        // fprintf(stderr,"build approximate time=,%.10lf,build exact time=,%.10lf,total build time=,%.10lf,",ti1,ti2,ti);
		st=clock();
        int temp;
        for(int i=0;i<n0;i++){
            temp=query(id0[i]);
            if(temp!=0)return -4;
        }
        for(int i=0;i<n1;i++){
            temp=query(id1[i]);
            if(temp!=1)return -4;
        }

        ti=1.0*(clock()-st)/CLOCKS_PER_SEC;;
        // fprintf(stderr,"query time=,%.10lf\n",ti);
        delete[] id3;
        delete[] value;
        return mem1+re;
    }
    bool query(int id){
        return (approximate->query(id)==1)&&(exact->query(id)==1);
    }
    ~ChainedFilter(){
        approximate->~Bloomier();
        exact->~Bloomier();
    }
};