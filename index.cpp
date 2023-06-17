#include<bits/stdc++.h>

using namespace std;

// Structure to represent a node in the Huffman tree
struct HuffmanNode {
    char data;
    unsigned freq;
    HuffmanNode* left, * right;

    HuffmanNode(char data, unsigned freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

// Custom comparator for the priority queue used in building the Huffman tree
struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

// Map to store the generated Huffman codes for each character
unordered_map<char, string> huffmanCodes;

// Function to calculate the frequency of characters in the given text
void calcFrequency(const string& text, unordered_map<char, unsigned>& freq) {
    for (char c : text) {
        freq[c]++;
    }
}

// Function to build the Huffman tree from the character frequencies
HuffmanNode* buildHuffmanTree(const unordered_map<char, unsigned>& freq) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;

    for (const auto& pair : freq) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();

        HuffmanNode* merged = new HuffmanNode('$', left->freq + right->freq);
        merged->left = left;
        merged->right = right;

        pq.push(merged);
    }

    return pq.top();
}

// Function to generate Huffman codes for each character in the Huffman tree
void generateHuffmanCodes(HuffmanNode* root, string code) {
    if (root == nullptr) {
        return;
    }

    if (root->data != '$') {
        huffmanCodes[root->data] = code;
    }

    generateHuffmanCodes(root->left, code + "0");
    generateHuffmanCodes(root->right, code + "1");
}

// Function to compress the input file using Huffman coding and store the result in the output file
void compress(const string& inputFile, const string& outputFile) {
    ifstream inFile(inputFile);
    ofstream outFile(outputFile, ios::binary);

    unordered_map<char, unsigned> freq;
    calcFrequency(string(istreambuf_iterator<char>(inFile), istreambuf_iterator<char>()), freq);//convert the input file to string and calculate its frequency

    HuffmanNode* root = buildHuffmanTree(freq);
    generateHuffmanCodes(root, "");

    inFile.clear();
    inFile.seekg(0, ios::beg);

    string bitString = "";
    char c;
    while (inFile.get(c)) {
        bitString += huffmanCodes[c];
    }

    while (bitString.size() % 8 != 0) {
        bitString += '0'; // Padding with zeros to ensure byte alignment
    }

    for (size_t i = 0; i < bitString.size(); i += 8) {
        bitset<8> bits(bitString.substr(i, 8));
        char byte = static_cast<char>(bits.to_ulong());
        outFile.put(byte);
    }

    inFile.close();
    outFile.close();
}

// Function to decompress the compressed file using the provided Huffman tree and store the result in the output file
void decompress(const string& compressedFile, const string& outputFile, HuffmanNode* root) {
    ifstream inFile(compressedFile, ios::binary);
    ofstream outFile(outputFile);

    char byte;
    inFile.get(byte);
    string bitString = "";
    while (!inFile.eof()) {
        bitString += bitset<8>(byte).to_string();
        inFile.get(byte);
    }

    HuffmanNode* currNode = root;
    for (char bit : bitString) {
        if (bit == '0') {
            currNode = currNode->left;
        }
        else {
            currNode = currNode->right;
        }

        if (currNode->left == nullptr && currNode->right == nullptr) {
            outFile.put(currNode->data);
            currNode = root;
        }
    }

    inFile.close();
    outFile.close();
}

// Function to free the memory allocated for the Huffman tree
void freeMemory(HuffmanNode* node) {
    if (node == nullptr) {
        return;
    }

    freeMemory(node->left);
    freeMemory(node->right);
    delete node;
}

int main() {
    // Store the file paths in strings
    const string inputFile = "./input.txt";
    const string compressedFile = "./compressed.bin";
    const string outputFile = "./output.txt";

    // Compress the input file and store the binary representation in the compressed file
    compress(inputFile, compressedFile);
    cout << "File compressed successfully!" << endl;

    // Calculate original and compressed file sizes
    ifstream originalFile(inputFile, ios::ate | ios::binary);
    ifstream compressed(compressedFile, ios::ate | ios::binary);

    size_t originalSize = originalFile.tellg();
    size_t compressedSize = compressed.tellg();

    // Calculate compression ratio
    double compressionRatio = (1 - static_cast<double>(compressedSize) / originalSize) * 100;
    cout << "Original size: " << originalSize << " bytes" << endl;
    cout << "Compressed size: " << compressedSize << " bytes" << endl;
    cout << "Compression ratio: " << compressionRatio << "%" << endl;

    // Decompress the compressed file using the Huffman tree and store the result in the output file
    ifstream compressedFile1(inputFile);
    unordered_map<char, unsigned> freq1;
    calcFrequency(string(istreambuf_iterator<char>(compressedFile1), {}), freq1);
    HuffmanNode* root = buildHuffmanTree(freq1);
    decompress(compressedFile, outputFile, root);
    cout << "File decompressed successfully!" << endl;

    // Free memory allocated for the Huffman tree
    freeMemory(root);
    originalFile.close();
    compressed.close();
    compressedFile1.close();
    return 0;
}
