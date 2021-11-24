#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <queue>
#include <functional>

class Huffman
{
    public:
        void buildHuffmanTree(char* characters, int* freq, int size);
        void printCodes();
        void decodeText(const char* file);
    
    private:
        struct Node {
            char data;
            int freq;
            struct Node *left, *right;
        };

        std::priority_queue<Node, std::vector<int>, std::greater<int>> heap;
};

#endif