#include<iostream>
#include<string.h>
#include<arpa/inet.h>
#include<sstream>

using namespace std;

template<typename T>
string serialize(const T&a)
{
    return T::serialize(a);
}

template<typename T>
int deserialize(string str,T&a)
{
    return T::deserialize(str,a);
}

/** Special Version
* For...
*   int, double, float
*/
/// int
template<>
string serialize(const int& a)
{
    string ans;
    int c=htonl(a);
    ans.append((const char*)&c,sizeof(c));
    return ans;
}
template<>
int deserialize(string str,int& c)
{
    memcpy(&c,str.data(),sizeof(c));
    c=ntohl(c);
    return sizeof(c);
}
/// string
template<>
string serialize(const string& a)
{
    int len=a.size();
    string ans;
    ans.append(::serialize(len));
    ans.append(a);
    return ans;
}
template<>
int deserialize(string str,string& a)
{
    int len;
    ::deserialize(str,len);
    a=str.substr(sizeof(len),len);
    return sizeof(int)+len;
}
///double
/*template<>
string serialize(const double& a)
{
    string ans;
    double c=htonl(a);
    ans.append((const char*)&c,sizeof(c));
    return ans;
}
template<>
int deserialize(string str,double& c)
{
    memcpy(&c,str.data(),sizeof(c));
    c=ntohl(c);
    return sizeof(c);
}
*/

/// Marco definition
#define NORMAL_DATA_SERIALIZE(Type) template<> \
    string serialize(const Type& a)\
    {\
        string ans;\
        ans.append((const char*)&a,sizeof(a));\
        return ans;\
    }
#define NORMAL_DATA_DESERIALIZE(Type) template<> \
    int deserialize(string str,Type& a)\
    {\
        memcpy(&a,str.data(),sizeof(a));\
        return sizeof(a);\
    }

/// double
NORMAL_DATA_SERIALIZE(double);
NORMAL_DATA_DESERIALIZE(double);
NORMAL_DATA_SERIALIZE(float);
NORMAL_DATA_DESERIALIZE(float);
NORMAL_DATA_SERIALIZE(char);
NORMAL_DATA_DESERIALIZE(char);

template<typename SerializableType>
class Serializable
{
public:
    static SerializableType deserialize(string);
    static string serialize(const SerializableType& a);
};

class OutEngine
{
public:
    template<typename SerializableType>
    OutEngine& operator << (SerializableType& a)
    {
        string x=::serialize(a);
        os.write(x.data(),x.size());
        return *this;
    }
    string str()
    {
        return os.str();
    }
    void set_empty()
    {
        os.str("");
    }
    OutEngine():os(std::ios::binary){}
public:
    ostringstream os;
};

class InEngine
{
public:
    InEngine(string s) : is(s){n_size=leftsize();}
    template<typename SerializableType>
    InEngine& operator >> (SerializableType& a)
    {
        int ret=::deserialize(is,a);
        is=is.substr(ret);
        return *this;
    }
    void set_str(string s)
    {
        is=s;
        n_size=leftsize();
    }
 
    int leftsize()
    {
        return is.size();
    }
    int donesize()
    {
        return n_size-leftsize();
    }
 
protected:
    string is;
    int n_size;
};
class cbox : public Serializable<cbox>
{
public:
    int a;
    double b;
    string str;
 
    static string serialize(const cbox& inc)
    {
        OutEngine x;
        x<<inc.a<<inc.b<<inc.str;
        return x.str();
    }
    static int deserialize(string inc,cbox& box)
    {
        InEngine x(inc);
        x>>box.a>>box.b>>box.str;
        return x.donesize();
    }
};
int main()
{
    cbox box;
    box.a=11;
    box.b=6.6;
    box.str="Hello World";
 
    cbox box3;
    box3.a=33;
    box3.b=12.5;
    box3.str="Yummy Hamburger!";
 
    OutEngine oe;
    oe<<box<<box3;
 
    string b=oe.str();
    cout<<b<<endl;
 
    cbox box2;
    cbox box4;
    InEngine ie(b);
    ie>>box2>>box4;
 
    cout<<box2.a<<endl;
    cout<<box2.b<<endl;
    cout<<box2.str<<endl;
 
    cout<<box4.a<<endl;
    cout<<box4.b<<endl;
    cout<<box4.str<<endl;
    return 0;
}



