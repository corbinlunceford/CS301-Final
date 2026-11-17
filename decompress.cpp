#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
using namespace std;
struct Node {int symbol=-1;Node* child[2]={nullptr,nullptr};};
int symbolFor(const string& s){if(s=="space")return ' ';if(s=="newline")return '\n';if(s=="return")return '\r';if(s=="tab")return '\t';if(s.size()==1)return static_cast<unsigned char>(s[0]);return -1;}
int main(int argc,char** argv){
auto start = std::chrono::high_resolution_clock::now();
 if(argc!=2){cerr<<"Usage: decompress <file.zip301>\n";return 1;}
 string input=argv[1];if(input.size()<7||input.substr(input.size()-7)!=".zip301"){cerr<<"Input must end in .zip301\n";return 1;}
 ifstream in(input,ios::binary);if(!in){cerr<<"Cannot open input\n";return 1;}
 vector<unique_ptr<Node>> nodes;auto create=[&](){nodes.push_back(make_unique<Node>());return nodes.back().get();};Node* root=create();
 string line;bool separator=false;array<bool,256> seen{};
 while(getline(in,line)){if(!line.empty()&&line.back()=='\r')line.pop_back();if(line=="*****"){separator=true;break;}
  size_t pos=line.find(' ');if(pos==string::npos||pos==0){cerr<<"Malformed code table\n";return 1;}
  string code=line.substr(0,pos),name=line.substr(pos+1);int symbol=symbolFor(name);if(symbol<0||seen[symbol]){cerr<<"Invalid or duplicate symbol\n";return 1;}seen[symbol]=true;
  Node* cur=root;for(char b:code){if(b!='0'&&b!='1'){cerr<<"Invalid code\n";return 1;}if(cur->symbol>=0){cerr<<"Non-prefix-free codes\n";return 1;}int direction=b-'0';if(!cur->child[direction])cur->child[direction]=create();cur=cur->child[direction];}
  if(cur->symbol>=0||cur->child[0]||cur->child[1]){cerr<<"Duplicate or conflicting code\n";return 1;}cur->symbol=symbol;
 }
 if(!separator||!getline(in,line)){cerr<<"Missing separator or bit count\n";return 1;}if(!line.empty()&&line.back()=='\r')line.pop_back();
 uint64_t total=0;try{size_t consumed=0;total=stoull(line,&consumed);if(consumed!=line.size())throw 1;}catch(...){cerr<<"Invalid bit count\n";return 1;}
 string output=input.substr(0,input.size()-7)+"2.txt";ofstream out(output,ios::binary);if(!out){cerr<<"Cannot create output\n";return 1;}
 Node* cur=root;uint64_t processed=0;char ch;
 while(processed<total&&in.get(ch)){unsigned char byte=static_cast<unsigned char>(ch);for(int shift=7;shift>=0&&processed<total;--shift){int bit=(byte>>shift)&1;++processed;if(!cur->child[bit]){cerr<<"Invalid compressed data\n";return 1;}cur=cur->child[bit];if(cur->symbol>=0){out.put(char(cur->symbol));cur=root;}}}
 if(processed!=total||cur!=root||!out){cerr<<"Truncated or malformed encoded data\n";return 1;}
 cout<<"Restored "<<output<<"\n";
 auto end = std::chrono::high_resolution_clock::now();

std::chrono::duration<double, std::milli> elapsed = end - start;

std::cout << "Execution time: "
          << std::fixed << std::setprecision(3)
          << elapsed.count() << " ms" << std::endl;
}
