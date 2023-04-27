#include "MurmurHash.h"
#include <cmath>
using namespace std;
double getp(int m,int n,int d){
    double re;
    re=1-pow(1-1.0/m,d*n);
    re=pow(re,d);
    return re;
}
class ChainedFilter{
    public:
    class Layer{
        public:
        bool*bloom;
	    int hashseed,len,sum1;
        double lambda,K;
        int n0,n1;
        Layer*upper;
        int level,d;
        Layer(int _n0,int _n1,double _lambda,int _level,int _hashseed=1e9+7){
            lambda=_lambda;
            n0=_n0;
            n1=_n1;
            K=1.0*n0/n1;
            len=0;
            double p=lambda/K;
            d=ceil(log(1.0/p)/log(2));
            d=max(d,1);
            d=min(d,6);
            for(int i=1<<28;i;i>>=1)
            if(getp(len+i,n1,d)>p)len+=i;
            len+=500;
            bloom=new bool[len]();
            level=_level;
            hashseed=_hashseed;
        }
        void insert(int id,bool k){
            if(k){
                for(int j=d;j--;){
                    int index=((unsigned)MurmurHash32(&id,4, hashseed*j))%len;
                    if(bloom[index]==0)sum1++;
                    bloom[index]=1;
                }
            }else{
                if(query(id)==k)return;
                if(upper==NULL)upper=new Layer(n1,n0*lambda/K,lambda,level+1);
                upper->insert(id,!k);
            }
        }
        void doit(int id,bool k){
            if(level>20)return;
            if(k){
                for(int j=d;j--;){
                    int index=((unsigned)MurmurHash32(&id,4, hashseed*j))%len;
                    if(bloom[index]==0)sum1++;
                    bloom[index]=1;
                }
                if(upper!=NULL)
                upper->doit(id,!k);
            }else{
                bool re=1;
                for(int j=d;j--;){
                    int index=((unsigned)MurmurHash32(&id,4, hashseed*j))%len;
                    if(bloom[index]==0)re=0;
                }
                if(!re)return;
                if(upper==NULL)upper=new Layer(n1,n0*lambda/K,lambda,level+1);
                upper->doit(id,!k);
            }
        }
        bool query(int id){
            bool re=1;
            for(int j=d;j--;){
                int index=((unsigned)MurmurHash32(&id,4, hashseed*j))%len;
                if(bloom[index]==0)re=0;
            }
            if(!re)return 0;
            if(upper!=NULL)return !upper->query(id);
            return 1;
        }
        int size(){
            int re=len;
            if(upper!=NULL)re+=upper->size();
            return re;
        }
    };
    Layer*F1;
    ChainedFilter(int n0,int n1){
        F1=new Layer(n0,n1,1.0/2,1);
    }
    void insert(int id,bool k){
        F1->insert(id,k);
    }
    void change(int id,bool k){
        F1->doit(id,k);
    }
    bool query(int id){
        return F1->query(id);
    }
    int size(){
        return F1->size();
    }
};