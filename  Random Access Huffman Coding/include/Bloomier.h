#include "MurmurHash.h"
#include <cstdio>
#include <ctime>
using namespace std;
const double z = 120.0;
const double C[10]={0,0,0,0.91,0.96,0,0,0.98};
class Bloomier{
	int d,hashseed,fingerprintlen,FingerSeed,mask;
    long long len,offset;
    public:
    int *Encoder;
    Bloomier(int _fingerprintlen,int _d=3,int _hashseed=1e9+7,int FingerSeed=19260817){
        fingerprintlen=_fingerprintlen;
        mask=(1<<_fingerprintlen)-1;
        d=_d;
        hashseed=_hashseed;
    }
    void changeseed(int _hashseed=1e9+7){
        hashseed=_hashseed;
    }
    int build(int*id,int*value,int n,int fix_mem=-1){
        len=n*1.13;
        offset=len/z;
        len+=offset;
        if(fix_mem>=0)len=fix_mem/fingerprintlen;
        clock_t st,st2;
		st=clock();
        Encoder=new int[len]();
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
        double ti=1.0*(clock()-st)/CLOCKS_PER_SEC;
        st=clock();
        int temp;
        for(int i=0;i<n;i++){
            temp=query(id[i]);
            if(temp!=value[i])return -4;
        }
        ti=1.0*(clock()-st)/CLOCKS_PER_SEC;
        delete []cnt;
        delete []queue;
        delete []pos;
        return len*fingerprintlen;
    }
    int query(int id){
        int fingerprint=((unsigned)MurmurHash32(&id,4, FingerSeed))&mask;
        int base=((unsigned)MurmurHash32(&id,4, hashseed*13))%(len-offset);
        for(int j=d;j--;){
            int index=base+((unsigned)MurmurHash32(&id,4, hashseed*j))%offset;
            fingerprint^=Encoder[index];
        }
        return fingerprint;
    }
    ~Bloomier(){
        delete[]Encoder;
    }
};