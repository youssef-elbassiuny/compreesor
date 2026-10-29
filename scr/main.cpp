#include<iostream>
#include<queue>
#include<unordered_map>
#include<fstream>
#include<filesystem>
#include "../include/bit_writer.h"
#include "../include/Arena.h"

using namespace std;

struct node
{
    node* m_left;
    node* m_right;
    char m_ch;
    size_t m_freq;


    node(char ch , size_t freq)
        :m_ch(ch) , m_freq(freq) , m_left(nullptr) , m_right(nullptr)
    {
    }

    node(node* left , node* right)
        :m_ch(0) , m_freq(left->m_freq+right->m_freq) ,m_right(right) , m_left(left)
    {
    }
    bool is_leaf()
    {
        return m_left==nullptr && m_right==nullptr;
    }
};
Arena_allocator arena(1024*20);
struct compare
{
    bool operator()(node* a, node* b)const
    {
        return a->m_freq > b->m_freq;
    }
};

void gen_code(node* node ,string code , unordered_map<char  ,string>& codes)
{
    if (node==nullptr)
    {
        return;
    }
    if (node->is_leaf())
    {
        codes[node->m_ch]=(code.empty())?"0" : code;
    }
    gen_code(node->m_left , code+"0" , codes);
    gen_code(node->m_right ,code+"1" , codes);
}


void write_tree(node* node , ofstream& out)
{
    if (node==nullptr)
    {
        return;
    }
    if (node->is_leaf())
    {
        out.put(1);
        out.put(node->m_ch);
        return;
    }
    out.put(0);
    write_tree(node->m_left , out);
    write_tree(node->m_right , out);
}





node* read_tree(ifstream& in)
{
    char marker;

    if (!in.get(marker))
    {
        return nullptr;
    }

    if (marker == 1)
    {
        char ch;

        if (!in.get(ch))
        {
            return nullptr;
        }
        return arena.alloc<node>(ch , 0);
    }

    if (marker == 0)
    {
        node* left = read_tree(in);
        node* right = read_tree(in);

        if (left == nullptr || right == nullptr)
        {
            return nullptr;
        }
        return arena.alloc<node>(left ,right);
    }

    return nullptr;
}




void decode(ifstream& in, ofstream& out, node* root, uint64_t size)
{
    if (root == nullptr)
    {
        return;
    }

    if (root->is_leaf())
    {
        for (uint64_t i = 0; i < size; ++i)
        {
            out.put(root->m_ch);
        }

        return;
    }

    node* current = root;
    uint64_t decoded = 0;
    char byte;

    while (decoded < size && in.get(byte))
    {
        for (int i = 7; i >= 0 && decoded < size; --i)
        {
            bool bit = (static_cast<unsigned char>(byte) >> i) & 1;

            if (bit == 0)
            {
                current = current->m_left;
            }
            else
            {
                current = current->m_right;
            }

            if (current->is_leaf())
            {
                out.put(current->m_ch);
                ++decoded;
                current = root;
            }
        }
    }
}
int main(int argc, char* argv[])
{

   if (string(argv[2])=="decode")
   {
       ifstream in(argv[1], ios::binary);
       if (!in) {
           cerr << "Failed to open input file"<<endl;
           return 1;
       }

       uint64_t size = 0;
       if (!in.read(reinterpret_cast<char*>(&size), sizeof(size))) {
           cerr << "failed to read original size"<<endl;
           return 1;
       }

       node* root = read_tree(in);
       if (!root) {
           cerr << "failed to read Huffman tree"<<endl;
           return 1;
       }

       ofstream out("decoded.txt", ios::binary);
       if (!out) {
           cerr << "failed to open decoded.txt"<<endl;
           return 1;
       }

       decode(in, out, root, size);

       out.close();
       in.close();

       cout << "decoding finished"<<endl;
       cout << "expected size: " << size << " bytes"<<endl;
       cout << "decoded file: decoded.txt"<<endl;
   }
    else if (string(argv[2])=="encode")
    {
        char* input=argv[1];
        ifstream file(input , ios::binary);
        if (!file)
        {
            cerr<<"failed to open file"<<endl;
            exit(EXIT_FAILURE);
        }
        size_t freq[256]{};
        char byte;
        while (file.get(byte))
        {
            unsigned char ch = static_cast<unsigned char>(byte);
            freq[ch]++;
        }
        vector<node*>nodes;
        for (int i=0 ; i<256  ; ++i)
        {
            if (freq[i]>0)
            {
                node* nodee=arena.alloc<node>(static_cast<char>(i) , freq[i]);
                nodes.push_back(nodee);
            }
        }
        priority_queue<node*  , vector<node*> , compare>leafs_tree;
        for (node* node : nodes)
        {
            leafs_tree.push(node);
        }
        while (leafs_tree.size()>1)
        {
            node* left=leafs_tree.top();
            leafs_tree.pop();
            node* right=leafs_tree.top();
            leafs_tree.pop();
            node* new_node=arena.alloc<node>(left ,right);
            leafs_tree.push(new_node);
        }
        node* root=nullptr;
        if (!leafs_tree.empty())
        {
            root=leafs_tree.top();
        }
        unordered_map<char , string>codes;
        gen_code(root , "" ,codes);
        file.clear();
        file.seekg(0);
        ofstream out("output.bin" , ios::binary);
        uint64_t size = filesystem::file_size(argv[1]);
        out.write(
        reinterpret_cast<const char*>(&size),
        sizeof(size)
    );
        write_tree(root , out);
        bit_writer writer(out);
        while (file.get(byte))
        {
            auto it =codes.find(byte);
            if (it !=codes.end())
            {
                writer.write_code(it->second);
            }
        }
        writer.finish();
        out.close();
    }
}