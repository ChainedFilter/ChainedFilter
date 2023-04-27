#include "MurmurHash.h"
#include <algorithm>
#include <cstring>
#include <cstdio>
using namespace std;
struct Key{
    char a[36],c[36];
    Key(int x=0){
        sprintf(a,"%d",x);
    }
    bool operator==(int x){
        char b[36];
        sprintf(b,"%d",x);
        return !strcmp(a,b);
    }
};
struct Value{
    char a[64],d[64];
    Value(int x=0){
        sprintf(a,"%d",x);
    }
};
void swap(pair<Key,Value>&x,pair<int,int>&y){
    pair<int,int>z;
    sscanf(x.first.a,"%d",&z.first);
    sscanf(x.second.a,"%d",&z.second);
    x.first=y.first;
    x.second=y.second;
    y=z;
}
class Cuckoo{
	int hashseed,n,mask;
    pair<Key,Value>*table[2];
    bool*have[2];
    public:
    Cuckoo(int _size,int _hashseed=1e9+7){
        for(n=1;n<_size;n<<=1);
        mask=n-1;
        hashseed=_hashseed;
        have[0]=new bool[n]();
        have[1]=new bool[n]();
        table[0]=new pair<Key,Value>[n]();
        table[1]=new pair<Key,Value>[n]();
    }
    int insert(pair<int,int> key){
        retry:
        int index=((unsigned)MurmurHash32(&key.first,4, hashseed))&mask;
        if(have[0][index]==0){
            have[0][index]=1;
            table[0][index]=key;
            return 0;
        }
        swap(table[0][index],key);
        int now=(index^key.first)&mask,o=1;
        while(have[o][now]){
            swap(table[o][now],key);
            if(o==0&&now==index){
                return -1;
            }
            o^=1;
            now=(now^key.first)&mask;
        }
        have[o][now]=1;
        table[o][now]=key;
        return o;
    }
    int getpos(int key){
        int index=((unsigned)MurmurHash32(&key,4, hashseed))&mask;
        if(have[0][index]==1&&table[0][index].first==key){
            return 0;
        }
        index=(index^key)&mask;
        if(have[1][index]==1&&table[1][index].first==key){
            return 1;
        }
        return -1;
    }
    bool query_with_pre(int key,int o){
        Value re;
        int index=((unsigned)MurmurHash32(&key,4, hashseed))&mask;
        if(o==1)index=(index^key)&mask;
        if(have[o][index]==1&&table[o][index].first==key){
            return 1;
        }
        o^=1;
        index=(index^key)&mask;
        if(have[o][index]==1&&table[o][index].first==key){
            return 0;
        }
    }
    Value query(int key){
        Value re,re1;
        int k;
        int index=((unsigned)MurmurHash32(&key,4, hashseed))&mask;
        re=table[0][index].second;
        if(have[0][index]==1&&table[0][index].first==key){
            return re;
        }
        index=(index^key)&mask;
        re1=table[1][index].second;
        if(have[1][index]==1&&table[1][index].first==key){
            return re1;
        }
        return k?re1:re;
    }
    pair<int,int> check(){
        pair<int,int>re=make_pair(0,0);
        for(int i=0;i<n;i++){
            if(have[0][i])re.first++;
            if(have[1][i])re.second++;
        }
        return re;
    }
};