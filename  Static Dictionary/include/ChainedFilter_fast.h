#include "MurmurHash.h"
#include "Bloomier.h"
#include <cstdio>
#include <cmath>
#include <ctime>
#include <cstring>
using namespace std;
const int chooseseed[11]={7,13,17,23,47,51,53,1000007,97,93};
class ChainedFilter{
    int deb=0;
	int d=3,hashseed;
    int query_layer(int*Encoder,int id,int len,int offset,int FingerSeed,int mask){
        int fingerprint=((unsigned)MurmurHash32(&id,4, FingerSeed))&mask;
        int base=((unsigned)MurmurHash32(&id,4, hashseed*13))%(len-offset);
        for(int j=d;j--;){
            int index=base+((unsigned)MurmurHash32(&id,4, hashseed*j))%offset;
            fingerprint^=Encoder[index];
        }
        return fingerprint;
    }
    int buildEncoder(int*Encoder,int*id,int*value,int n,int len,int offset,int fingerprintlen,int FingerSeed,int mask){
        int *cnt=new int[len]();
        int *queue=new int[len+n]();
        int *pos=new int[n]();
        register int i,l=0,r=0,top=len;
        for(i=0;i<n;i++){
            pos[i]=-1;
            int base=((unsigned)MurmurHash32(&id[i],4, hashseed*13))%(len-offset);
            for(int j=d;j--;){
                int index=base+((unsigned)MurmurHash32(&id[i],4, hashseed*j))%offset;
                cnt[index]++;Encoder[index]^=i;
            }
        }
        for(i=0;i<len;i++)
        if(cnt[i]==1)queue[r++]=i;
        for(;l<r;l++){
            i=queue[l];
            if(cnt[i]!=1)continue;
            int k=Encoder[i];
            pos[k]=i;
            queue[top++]=k;
            int base=((unsigned)MurmurHash32(&id[k],4, hashseed*13))%(len-offset);
            for(int j=d;j--;){
                int index=base+((unsigned)MurmurHash32(&id[k],4, hashseed*j))%offset;
                cnt[index]--;Encoder[index]^=k;
                if(cnt[index]==1)queue[r++]=index;
            }
        }
        if(top!=len+n)return -1;
        for(i=top-1;i>=len;i--){
            int k=queue[i];
            int fingerprint=((unsigned)MurmurHash32(&id[k],4, FingerSeed))&mask;
            int base=((unsigned)MurmurHash32(&id[k],4, hashseed*13))%(len-offset);
            for(int j=d;j--;){
                int index=base+((unsigned)MurmurHash32(&id[k],4, hashseed*j))%offset;
                fingerprint^=Encoder[index];
            }
            if(Encoder[pos[k]]!=0)return -2;
            Encoder[pos[k]]=fingerprint^value[k];
        }
        int temp;
        for(int i=0;i<n;i++){
            temp=query_layer(Encoder,id[i],len,offset,FingerSeed,mask);
            if(temp!=value[i]){
                return -4;
            }
        }
        delete []cnt;
        delete []queue;
        delete []pos;
        return len*fingerprintlen;
    }
    int FingerSeed,mask;
    long long len,offset;
    int*Counter;
    public:
    ChainedFilter(int _hashseed=23068673){
        hashseed=_hashseed;
    }
    void changeseed(int _hashseed=23068673){
        hashseed=_hashseed;
    }
    int build(int*id1,int n1,int*id0,int n0,int fix_mem=-1){
        double lambda=1.0*n0/n1;
        int fingerprintlen=ceil(log(lambda)/log(2)-0.1);
        len=n1*1.13;
        offset=2*len/z;
        len+=offset;
        if(fix_mem>=0)len=fix_mem/fingerprintlen;
        int *id3=new int[n0+n1];
        int *value=new int[n0+n1];
        for(int i=0;i<n1;i++)value[i]=1;
        clock_t st,st2;
		st=clock();
        int*Encoder1=new int[len]();
        int mem1=buildEncoder(Encoder1,id1,value,n1,len,offset,fingerprintlen,FingerSeed=19260817,mask=(1<<fingerprintlen)-1);
        if(mem1<0){
            return -1;
        }
        double ti1=1.0*(clock()-st)/CLOCKS_PER_SEC;
        int n3=0;
        for(int i=0;i<n1;i++){
            int k=query_layer(Encoder1,id1[i],len,offset,FingerSeed,mask);
            if(k!=1)return -2;
            id3[n3]=id1[i];
            value[n3]=1;
            n3++;
        }
        for(int i=0;i<n0;i++){
            int k=query_layer(Encoder1,id0[i],len,offset,FingerSeed,mask);
            if(k!=1)continue;
            id3[n3]=id0[i];
            value[n3]=0;
            n3++;
        }
		st2=clock();
        int*Encoder2=new int[len*2-offset]();;
        int mem2=buildEncoder(Encoder2,id3,value,n3,len*2-offset,offset,1,FingerSeed,1);
        if(mem2<0){
            return -3;
        }
        double ti2=1.0*(clock()-st2)/CLOCKS_PER_SEC;
        double ti=1.0*(clock()-st)/CLOCKS_PER_SEC;
        // fprintf(stderr,"n=,%d,n0=,%d,n1=,%d,n3=,%d,mem1=,%d,mem2=,%d,total mem=,%d,",n0+n1,n0,n1,n3,mem1,mem2,mem1+mem2);
        // fprintf(stderr,"build approximate time=,%.10lf,build exact time=,%.10lf,total build time=,%.10lf,",ti1,ti2,ti);
        Counter=new int[len]();
        for(int i=0;i<len;i++){
            Counter[i]=Encoder1[i]<<1;
            Counter[i]+=Encoder2[i];
            Counter[i]<<=1;
            Counter[i]+=Encoder2[i+len-offset];
        }
		st=clock();
        int temp;
        for(int i=0;i<n1;i++){
            temp=query(id1[i]);
            if(temp!=1){
                temp=query(id1[i]);
                temp=(query_layer(Encoder1,id1[i],len,offset,FingerSeed,mask)==1)&query_layer(Encoder2,id1[i],len*2-offset,offset,FingerSeed,1);
                return -5;
            }
        }
        for(int i=0;i<n0;i++){
            temp=query(id0[i]);
            if(temp!=0)return -6;
        }

        ti=1.0*(clock()-st)/CLOCKS_PER_SEC;;
        // fprintf(stderr,"query time=,%.10lf\n",ti);
        delete []Encoder1;
        delete []Encoder2;
        return mem1+mem2;
    }
    bool query(int id){
        int fingerprint=((unsigned)MurmurHash32(&id,4, FingerSeed))&mask;
        fingerprint=(fingerprint<<1)+(fingerprint&1);
        fingerprint=(fingerprint<<1)+(fingerprint&1);
        int h=((unsigned)MurmurHash32(&id,4, hashseed*13))%(2*len-2*offset);
        int base=h%(len-offset);
        for(int j=d;j--;){
            int index=base+((unsigned)MurmurHash32(&id,4, hashseed*j))%offset;
            fingerprint^=Counter[index];
        }
        if(h==base){
            return ((fingerprint>>2)==1)&&((fingerprint&2)==2);
        }else{
            return ((fingerprint>>2)==1)&&((fingerprint&1)==1);
        }
    }
};