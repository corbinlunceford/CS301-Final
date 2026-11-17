#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <memory>
#include <queue>
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>
using namespace std;
struct Node { uint64_t freq; int symbol; int minimum; Node *left,*right; };
struct Compare { bool operator()(const Node* a,const Node* b) const { if(a->freq!=b->freq) return a->freq>b->freq; return a->minimum>b->minimum; } };
void codesFor(Node* node,string path,array<string,256>& codes){if(!node)return;if(node->symbol>=0){codes[node->symbol]=path.empty()?"0":path;return;}codesFor(node->left,path+'0',codes);codesFor(node->right,path+'1',codes);}
string nameFor(unsigned char c){if(c==' ')return "space";if(c=='\n')return "newline";if(c=='\r')return "return";if(c=='\t')return "tab";return string(1,char(c));}
int main(int argc,char** argv){
auto start = std::chrono::high_resolution_clock::now();
 if(argc!=2){cerr<<"Usage: compress <file.txt>\n";return 1;}
 string input=argv[1];if(input.size()<4||input.substr(input.size()-4)!=".txt"){cerr<<"Input must end in .txt\n";return 1;}
 ifstream in(input,ios::binary);if(!in){cerr<<"Cannot open input\n";return 1;}
 array<uint64_t,256> freq{};vector<unsigned char> data;char ch;while(in.get(ch)){auto b=static_cast<unsigned char>(ch);++freq[b];data.push_back(b);}if(!in.eof()){cerr<<"Input read failed\n";return 1;}
 vector<unique_ptr<Node>> storage;priority_queue<Node*,vector<Node*>,Compare> pq;
 auto makeNode=[&](uint64_t f,int s,int m,Node*l,Node*r){storage.push_back(make_unique<Node>(Node{f,s,m,l,r}));return storage.back().get();};
 for(int i=0;i<256;++i)if(freq[i])pq.push(makeNode(freq[i],i,i,nullptr,nullptr));
 while(pq.size()>1){Node*a=pq.top();pq.pop();Node*b=pq.top();pq.pop();pq.push(makeNode(a->freq+b->freq,-1,min(a->minimum,b->minimum),a,b));}
 array<string,256> codes{};if(!pq.empty())codesFor(pq.top(),"",codes);
 string output=input.substr(0,input.size()-4)+".zip301";ofstream out(output,ios::binary);if(!out){cerr<<"Cannot create output\n";return 1;}
 // The ZIP301 specification reserves whitespace labels and a literal character otherwise.
 // This format cannot unambiguously represent LF/CR/TAB/SPACE aliases as ordinary text;
 // we emit their prescribed surrogate labels.
 for(int i=0;i<256;++i)if(freq[i])out<<codes[i]<<' '<<nameFor(static_cast<unsigned char>(i))<<'\n';
 out<<"*****\n";uint64_t bits=0;for(int i=0;i<256;++i)bits+=freq[i]*codes[i].size();out<<bits<<'\n';
 unsigned char byte=0;int used=0;for(auto b:data)for(char bit:codes[b]){byte=static_cast<unsigned char>((byte<<1)|(bit=='1'));if(++used==8){out.put(char(byte));byte=0;used=0;}}
 if(used){byte=static_cast<unsigned char>(byte<<(8-used));out.put(char(byte));}
 if(!out){cerr<<"Output write failed\n";return 1;}cout<<"Created "<<output<<" ("<<bits<<" encoded bits)\n";
 auto end = std::chrono::high_resolution_clock::now();

std::chrono::duration<double, std::milli> elapsed = end - start;

std::cout << "Execution time: "
          << std::fixed << std::setprecision(3)
          << elapsed.count() << " ms" << std::endl;
}