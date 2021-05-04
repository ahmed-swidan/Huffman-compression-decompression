#include <bits/stdc++.h>
using namespace std;

struct MinHeapNode
{

    // One of the input characters
    char data;

    // Frequency of the character
    unsigned freq;

    // Left and right child
    MinHeapNode *left, *right;

    MinHeapNode(char data, unsigned freq)

    {

        left = right = NULL;
        this->data = data;
        this->freq = freq;
    }
};

struct compare
{

    bool operator()(MinHeapNode *l, MinHeapNode *r)

    {
        return (l->freq > r->freq);
    }
};

void calculateCompressionRatio(char *fileName)
{
    std::streampos fsize = 0;
    std::ifstream myfile(fileName, ios::in);  // File is of type const char*
    fsize = myfile.tellg();         // The file pointer is currently at the beginning
    myfile.seekg(0, ios::end);      // Place the file pointer at the end of file
    fsize = myfile.tellg() - fsize;
    myfile.close();

    static_assert(sizeof(fsize) >= sizeof(long long), "Oops.");

    cout << "size uncompressed is: " << fsize / 1000 << " Kbytes.\n";


    std::streampos fsize2 = 0;
    std::ifstream myfile2("output.txt", ios::in);  // File is of type const char*
    fsize2 = myfile2.tellg();         // The file pointer is currently at the beginning
    myfile2.seekg(0, ios::end);      // Place the file pointer at the end of file
    fsize2 = myfile2.tellg() - fsize2;
    myfile2.close();

    static_assert(sizeof(fsize2) >= sizeof(long long), "Oops.");
    cout << "size compressed is: " << fsize2 / 1000 << " kbytes.\n";
    cout << "Compression ratio is: " << (double)fsize / fsize2 * 1.0 << endl;
}

void printCodes(struct MinHeapNode *root, string str, map<char, string> &charCode)
{

    if (!root)
        return;

    if (root->data != '*')
        charCode[root->data] = str;

    printCodes(root->left, str + "0", charCode);
    printCodes(root->right, str + "1", charCode);
}

map<char, string> HuffmanCodes(map<char, int> chars)
{
    map<char, string> charCode;
    struct MinHeapNode *left, *right, *top;

    priority_queue<MinHeapNode *, vector<MinHeapNode *>, compare> minHeap;

    for (auto const &item : chars)
    {
        minHeap.push(new MinHeapNode(item.first, item.second));
    }

    while (minHeap.size() != 1)
    {

        left = minHeap.top();
        minHeap.pop();

        right = minHeap.top();
        minHeap.pop();
        top = new MinHeapNode('*', left->freq + right->freq);

        top->left = left;
        top->right = right;

        minHeap.push(top);
    }

    printCodes(minHeap.top(), "", charCode);
    return charCode;
}

map<char, int> readInput(char *fileName)
{
    map<char, int> freq;
    ifstream f;
    f.open(fileName);
    char temp;
    while (f.get(temp))
    {
        if (freq.find(temp) == freq.end())
        {
            freq[temp] = 1;
        }
        else
        {
            freq[temp]++;
        }
    }
    f.close();
    return freq;
}

string charToCode(map<char, string> charCode, char *fileName)
{
    string code = "";
    ifstream f(fileName);
    char temp;
    while (f.get(temp))
    {
        charCode.find(temp);
        code += charCode[temp];
    }
    f.close();
    return code;
}

char str_to_char(string binaryString)
{
    stringstream sstream(binaryString);
    bitset<8> bits;
    sstream >> bits;
    return char(bits.to_ulong());
}

void storeHuffmanMap(ofstream &out, map<char, string> huffmanMap)
{
    for (auto const &item : huffmanMap)
    {
        if (item.first == '\n')
        {
            out << "\\n:" << item.second << endl;
        }
        else
        {
            out << item.first << ":" << item.second << endl;
        }
    }
    out << "**" << endl;
}

void compress(char *chr)
{
    clock_t t1, t2;
    t1 = clock();
    map<char, string> huffmanMap = HuffmanCodes(readInput(chr));
    string huffmanCode = charToCode(huffmanMap, chr);
    int padding;
    if (huffmanCode.length() % 8 != 0)
    {
        padding = 8 - huffmanCode.length() % 8;
        for (int x = 0; x < padding; x++)
            huffmanCode += "0";
    }
    string outfile = "output.txt";
    ofstream f(outfile.c_str(), std::ofstream::binary);
    f << padding << endl;
    int i = 0;
    storeHuffmanMap(f, huffmanMap);
    while (true)
    {
        char temp = str_to_char(huffmanCode.substr(i, 8));
        f << temp;
        i += 8;
        if (i >= huffmanCode.length())
            break;
    }
    f.close();
    t2 = clock();
    double time = ((double)t2 - (double)(t1)) / (double)CLOCKS_PER_SEC;
    cout << "time elapsed: " << time << endl;
    calculateCompressionRatio(chr);
}

void getHuffmanCodesFromFile(ifstream &f, int *padding, map<char, string> &huffmanCodes)
{
    char temp;
    if (f.is_open())
    {
        f.get(temp);
        f.get();
        *padding = temp - 48;
        bool flag = true;
        char temp1, temp2;
        while (f.get(temp1))
        {

            f.get(temp2);
            if (temp1 == '*' && temp2 == temp1)
            {
                f.get();
                break;
            }
            if (flag)
            {

                char colon = ':';
                if (colon - temp2)
                {
                    string temp3;
                    f.get();
                    getline(f, temp3);
                    huffmanCodes[(char)10] = temp3;
                    flag = false;
                }
                else
                {
                    string code;
                    getline(f, code);
                    huffmanCodes[temp1] = code;
                }
            }
            else
            {
                string code;
                getline(f, code);
                huffmanCodes[temp1] = code;
            }
        }
    }
}

string char_to_str(char ch)
{
    bitset<8> temp(ch);
    return temp.to_string();
}

void decompress(char *filename)
{
    clock_t t1, t2;
    t1 = clock();
    ifstream f(filename, std::ifstream::binary);
    int padding = 0;
    map<char, string> huffmanCodes;
    getHuffmanCodesFromFile(f, &padding, huffmanCodes);
    char temp;
    string huffmanCode = "";
    while (f.get(temp))
    {
        huffmanCode += char_to_str(temp);
    }
    huffmanCode = huffmanCode.substr(0, huffmanCode.length() - padding);
    cout << huffmanCode.length() << " " << huffmanCodes.size() << endl;
    string outfile = "decompressed.txt";
    ofstream decompressedOut(outfile.c_str(), std::ofstream::binary);
    int i = 0;
    while (i < huffmanCode.length())
    {
        for (auto const &item : huffmanCodes)
        {
            if (huffmanCode.substr(i, item.second.length()) == item.second)
            {
                decompressedOut << item.first;
                i += item.second.length();
                break;
            }
        }
    }
    f.close();
    for (auto const &item : huffmanCodes)
        cout << item.first << " : " << item.second << endl;
    t2 = clock();
    double time = ((double)t2 - (double)(t1)) / (double)CLOCKS_PER_SEC;
    cout << "time elapsed: " << time << endl;
}

int main()
{

    string fileName;
    cout << "please enter the desired file name: ";
    cin >> fileName;
    int mode;
    do
    {
        cout << "please choose the mode (1: compress, 2: decompress, 3: exit): ";
        cin >> mode;
    } while (!(mode > 0 && mode < 4));
    char *chr = const_cast<char *>(fileName.c_str());
    switch (mode)
    {
    case 1:
        compress(chr);
        break;
    case 2:
        decompress(chr);
        break;
    case 3:
        exit(0);
    default:
        break;
    }
    return 0;
}
