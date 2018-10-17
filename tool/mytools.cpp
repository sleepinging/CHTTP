#include "mytools.h"

#include <malloc.h>
#include <string.h>
#include <sys/stat.h>

#include <sstream>

using namespace std;

void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
{
  std::string::size_type pos1, pos2;
  pos2 = s.find(c);
  pos1 = 0;
  while(std::string::npos != pos2)
  {
    v.emplace_back(s.substr(pos1, pos2-pos1));
 
    pos1 = pos2 + c.size();
    pos2 = s.find(c, pos1);
  }
  if(pos1 != s.length())
    v.emplace_back(s.substr(pos1));
}

std::string& trim(std::string &s)   
{  
    if (s.empty())   
    {  
        return s;  
    }  
  
    s.erase(0,s.find_first_not_of(" "));  
    s.erase(s.find_last_not_of(" ") + 1);  
    return s;  
}

void dump(const void *p, size_t len, const char *filename)
{
    FILE *fp = fopen(filename, "w+b");
    //fopen_s(&fp, filename, "w+b");
    fwrite(p,sizeof(char),len,fp);
    fclose(fp);
}

//读取文件
std::vector<unsigned char> readfileall(const char* const filename){
    vector<unsigned char> buf;
    FILE *fp = fopen(filename, "rb");
    if(fp==nullptr){
        return buf;
    }
    // fseek(fp,0L,SEEK_END);
    // int size=ftell(fp);
    auto size=getfilesize(filename);
    
    buf.resize(size);
    fread(&buf[0],1,size,fp);
    fclose(fp);
    return buf;
}

//获取文件大小
size_t getfilesize(const char* const filename){
    class stat st;
    stat(filename,&st);
    return st.st_size;
}

int big2small(unsigned short v){
    auto d=(unsigned char*)&v;
    unsigned short a=(unsigned short)d[1]+d[0]*0x100;
    return a;
}

std::string mactostring(const unsigned char mac[6]){
    string smac;
    string tmp;
    stringstream ss;

    ss<<hex<<(int)mac[0]<<endl;
    ss>>tmp;
    smac.append(tmp);

    for(int i=1;i<6;++i){
        ss<<hex<<(int)mac[i]<<endl;
        ss>>tmp;
        smac.append(":").append(tmp);
    }
    return smac;
}

//将字符串转化为MAC
bool stringtomac(const std::string& str,unsigned char mac[6]){
    if(str.length()<11)return false;
    sscanf(str.c_str(), "%x:%x:%x:%x:%x:%x",
     (unsigned int*)&mac[0],(unsigned int*)&mac[1],(unsigned int*)&mac[2],
     (unsigned int*)&mac[3],(unsigned int*)&mac[4],(unsigned int*)&mac[5]);
    return true;
}

std::string iptostring(const unsigned char ip[4]){
    string smac;
    string tmp;
    stringstream ss;

    ss<<(int)ip[0]<<endl;
    ss>>tmp;
    smac.append(tmp);
    
    for(int i=1;i<4;++i){
        ss<<(int)ip[i]<<endl;
        ss>>tmp;
        smac.append(".").append(tmp);
    }
    return smac;
}

//string转uint64
uint64_t stringtouint64(const std::string& str){
    stringstream ss;
    uint64_t tmp;
    ss<<str;
    ss>>tmp;
    return tmp;
}

void getNext(const char *P,int len, int next[])
{
    int N=len;
    next[0]=0;
    next[1]=0;
    int k=0;
    int i=0;
    for(i=2;i<N;i++)
    {
        k=next[i-1];
        while(1)
        {
            if(P[i-1]==P[k])
            {
                next[i]=k+1;
                break;
            }
            else
                k=next[k];
            if(k==0)
            {
                next[i]=k;
                break;
            }
        }
    }
}


//利用KMP算法，验证字符串P是否为T的子字符串，如果是返回P第一个字母对应T的字符的
//最大长度len
//位置，否则返回-1
int KmpCmp(const char *T,int Tlen,const char *P,int Plen)
{
    int len=Plen;
    int *next=(int *)malloc(len*sizeof(int));
    getNext(P,Plen,next);

    int i=0,j=0;
    while(i<Tlen && j<Plen)
    {
        if(T[i]==P[j])
        {
            i++;
            j++;
        }
        else
        {       
            if(j==0)
                i++;
            j=next[j];
        }
    }
    if(j==Plen)
    {
        free(next);
        return i-len;
    }
    else
    {
        free(next);
        return -1;
    }
}

int hex2dec(char c)
{
	if ('0' <= c && c <= '9') {
		return c - '0';
	} else if ('a' <= c && c <= 'f') {
		return c - 'a' + 10;
	} else if ('A' <= c && c <= 'F') {
		return c - 'A' + 10;
	} else {
		return -1;
	}
}
 
char dec2hex(short int c)
{
	if (0 <= c && c <= 9) {
		return c + '0';
	} else if (10 <= c && c <= 15) {
		return c + 'A' - 10;
	} else {
		return -1;
	}
}
 
 
/*
 * 编码一个url
 * 结果需要先分配输入的3倍内存
 */
void urlencode(const char url[],size_t inlen,char* strout,size_t* lenout)
{
	size_t i = 0;
	*lenout = 0;
	for (i = 0; i < inlen; ++i) {
		char c = url[i];
		if (('0' <= c && c <= '9') ||
				('a' <= c && c <= 'z') ||
				('A' <= c && c <= 'Z') || c == '/' || c == '.') {
			strout[(*lenout)++] = c;
		} else {
			int j = (short int)c;
			if (j < 0)
				j += 256;
			int i1, i0;
			i1 = j / 16;
			i0 = j - i1 * 16;
			strout[(*lenout)++] = '%';
			strout[(*lenout)++] = dec2hex(i1);
			strout[(*lenout)++] = dec2hex(i0);
		}
	}
}
 
/*
 * 解码url
 * 输出字符需要先分配同样大的内存
 */
void urldecode(const char* instr,size_t inlen,char outstr[],size_t *outlen){
    size_t i = 0;
    *outlen=0;
    for (i = 0; i < inlen; ++i) {
		char c = instr[i];
		if (c != '%') {
			outstr[(*outlen)++] = c;
		} else {
			char c1 = instr[++i];
			char c0 = instr[++i];
			int num = 0;
			num = hex2dec(c1) * 16 + hex2dec(c0);
			outstr[(*outlen)++] = num;
		}
	}
}
