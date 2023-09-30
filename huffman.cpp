#include <iostream>
#include <queue>
#include <unordered_map>
#include <string>
#include <bitset>
#include <fstream>
#include <QMessageBox>
#include <direct.h>   // For _mkdir on Windows systems

using namespace std;

class HuffmanNode
{
public:
    char symbol;
    int frequency;
    HuffmanNode *left;
    HuffmanNode *right;

    HuffmanNode(char symbol, int frequency) : symbol(symbol), frequency(frequency), left(nullptr), right(nullptr) {}
};

struct CompareNodes
{
    bool operator()(HuffmanNode *a, HuffmanNode *b)
    {
        return a->frequency > b->frequency;
    }
};

unordered_map<char, int> buildFrequencyTable(const string &data)
{
    unordered_map<char, int> freqTable;
    for (char c : data)
    {
        freqTable[c]++;
    }
    return freqTable;
}

HuffmanNode *buildHuffmanTree(const unordered_map<char, int> &freqTable)
{
    priority_queue<HuffmanNode *, vector<HuffmanNode *>, CompareNodes> minHeap;
    for (auto &entry : freqTable)
    {
        minHeap.push(new HuffmanNode(entry.first, entry.second));
    }

    while (minHeap.size() > 1)
    {
        HuffmanNode *leftChild = minHeap.top();
        minHeap.pop();
        HuffmanNode *rightChild = minHeap.top();
        minHeap.pop();

        HuffmanNode *parent = new HuffmanNode('\0', leftChild->frequency + rightChild->frequency);
        parent->left = leftChild;
        parent->right = rightChild;

        minHeap.push(parent);
    }

    return minHeap.top();
}

void buildCodeTable(HuffmanNode *node, const string &code, unordered_map<char, string> &codeTable)
{
    if (node == nullptr)
    {
        return;
    }

    if (node->symbol != '\0')
    {
        codeTable[node->symbol] = code;
    }

    buildCodeTable(node->left, code + "0", codeTable);
    buildCodeTable(node->right, code + "1", codeTable);
}

string huffmanCompress(const string &data, const unordered_map<char, string> &codeTable)
{
    string compressedData;
    for (char c : data)
    {
        compressedData += codeTable.at(c);
    }

    // Add padding to make the length of compressed data a multiple of 8
    int paddingBits = 8 - compressedData.length() % 8;
    compressedData += string(paddingBits, '0');

    // Convert compressed data to bytes
    string byteData;
    for (size_t i = 0; i < compressedData.length(); i += 8)
    {
        string byteStr = compressedData.substr(i, 8);
        byteData += static_cast<char>(bitset<8>(byteStr).to_ulong());
    }

    return byteData;
}

void encodeHuffmanTree(HuffmanNode *node, ofstream &outFile)
{
    if (node == nullptr)
    {
        return;
    }

    if (node->symbol != '\0')
    {
        outFile.put(1); // Mark leaf nodes with 0x01
        outFile.put(node->symbol);
    }
    else
    {
        outFile.put(0); // Mark internal nodes with 0x00
        encodeHuffmanTree(node->left, outFile);
        encodeHuffmanTree(node->right, outFile);
    }
}

void decodeHuffmanTree(ifstream &inFile, HuffmanNode *node)
{
    char marker;
    inFile.get(marker);

    if (marker == 1)
    { // Leaf node
        char symbol;
        inFile.get(symbol);
        node->left = nullptr;
        node->right = nullptr;
        node->symbol = symbol;
    }
    else if (marker == 0)
    { // Internal node
        node->symbol = '\0';
        node->left = new HuffmanNode('\0', 0);
        node->right = new HuffmanNode('\0', 0);
        decodeHuffmanTree(inFile, node->left);
        decodeHuffmanTree(inFile, node->right);
    }
}
string huffmanDecompress(const string &compressedData, HuffmanNode *huffmanTree)
{
    string binaryData;
    for (char c : compressedData)
    {
        binaryData += bitset<8>(c).to_string();
    }

    string decompressedData;
    HuffmanNode *currentNode = huffmanTree;
    for (char bit : binaryData)
    {
        if (bit == '0')
        {
            currentNode = currentNode->left;
        }
        else
        {
            currentNode = currentNode->right;
        }

        if (currentNode->symbol != '\0')
        {
            decompressedData += currentNode->symbol;
            currentNode = huffmanTree;
        }
    }

    return decompressedData;
}

void printHelp()
{
    cout << "Usage: huffman_cli <compress|decompress> <input_file> <output_file>" << endl;
}
std::streampos getFileSize(const std::string &filename)
{
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file)
    {
        return 0;
    }
    return file.tellg();
}

void createDirectory(const std::string& directoryPath) {
#ifdef _WIN32
    _mkdir(directoryPath.c_str());
#else
    mkdir(directoryPath.c_str(), 0777); // Note: You might want to use more restrictive permissions
#endif
}

void compressFile(const QString &inputFileName, const QString &outputFileName)
{
    // Read input data from the provided file
    std::ifstream inFile(inputFileName.toStdString());
    if (!inFile)
    {
        QMessageBox::critical(nullptr, "Error", "Error opening input file: " + inputFileName);
        return;
    }

    std::string originalData;
    char c;
    while (inFile.get(c))
    {
        originalData += c;
    }
    inFile.close();

    // Build the Huffman tree and compress the data
    std::unordered_map<char, int> freqTable = buildFrequencyTable(originalData);
    HuffmanNode *huffmanTree = buildHuffmanTree(freqTable);

    std::unordered_map<char, std::string> codeTable;
    buildCodeTable(huffmanTree, "", codeTable);

    std::string compressedData = huffmanCompress(originalData, codeTable);

    // Define the folder name
    std::string compressFolder = "compress";

    // Create the folder if it doesn't exist
    createDirectory(compressFolder);

    // Modify the output file name to include the folder
    std::string outputFileNameWithPath = compressFolder + "/" + outputFileName.toStdString();

    // Save the Huffman tree and compressed data to a file within the "compress" folder
    std::ofstream outFile(outputFileNameWithPath, std::ios::binary);
    outFile.put(huffmanTree->symbol);
    encodeHuffmanTree(huffmanTree, outFile);
    outFile << compressedData;
    outFile.close();

    // Calculate original file size
    std::streampos originalFileSize = getFileSize(inputFileName.toStdString());
    // Calculate compressed file size
    std::streampos compressedFileSize = getFileSize(outputFileNameWithPath);
    double compressionRatio = static_cast<double>(compressedFileSize) / originalFileSize;
    QString stringCompressionRatio = QString::number(compressionRatio);
    cout << "Compression successful. Compressed data saved in '" << outputFileNameWithPath << "'." << endl;
    cout << "Compression Ratio: " << compressionRatio << endl;

    QMessageBox::information(nullptr, "Compress", "Compression successful. Compressed data saved in " + QString::fromStdString(outputFileNameWithPath) + " with a compression ratio of " + stringCompressionRatio);

}

// Function to decompress the input compressed file using Huffman coding
void decompressFile(const QString &inputFileName, const QString &outputFileName)
{
    // Read compressed data and Huffman tree from the file
    std::ifstream inFile(inputFileName.toStdString(), std::ios::binary);
    if (!inFile)
    {
        QMessageBox::critical(nullptr, "Error", "Error opening input file: " + inputFileName);
        return;
    }

    char rootSymbol;
    inFile.get(rootSymbol);
    HuffmanNode *decodedHuffmanTree = new HuffmanNode(rootSymbol, 0);
    decodeHuffmanTree(inFile, decodedHuffmanTree);

    // Read the remaining compressed data
    std::string compressedData;
    char c;
    while (inFile.get(c))
    {
        compressedData += c;
    }
    inFile.close();

    // Decompress the data
    std::string decompressedData = huffmanDecompress(compressedData, decodedHuffmanTree);

    // Define the folder name
    std::string decompressFolder = "decompress";

    // Create the folder if it doesn't exist
    createDirectory(decompressFolder);

    // Modify the output file name to include the folder
    std::string outputFileNameWithPath = decompressFolder + "/" + outputFileName.toStdString();

    // Save the decompressed data to a file
    std::ofstream outFile(outputFileNameWithPath, std::ios::binary);
    outFile << decompressedData;
    outFile.close();

    // Calculate original file size
    std::streampos originalFileSize = getFileSize(inputFileName.toStdString());
    // Calculate compressed file size
    std::streampos decompressedFileSize = getFileSize(outputFileName.toStdString());
    double compressionRatio = static_cast<double>(decompressedFileSize) / originalFileSize;
    QString stringDecompressionRatio = QString::number(compressionRatio);
    cout << "Decompression successful. Decompressed data saved in '" << outputFileName.toStdString() << "'." << endl;
    cout << "Decompression Ratio: " << compressionRatio << endl;


    QMessageBox::information(nullptr, "Decompress", "Decompression successful. Decompressed data saved in " + outputFileName + " with decompression ratio of " + stringDecompressionRatio );
}
