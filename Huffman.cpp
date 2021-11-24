#include "Huffman.h"
#include <algorithm>

void Huffman::buildHuffmanTree(char* characters, int* freq, int size)
{
    for(int i = 0; i < size; i++)
    {
        Node leaf = {characters[i], freq[i], NULL, NULL};
        heap.push(leaf);
    }

    while(heap.size() != 1)
    {

    }
}

void printCodes()
{

}

void decodeText(const char* file)
{

}