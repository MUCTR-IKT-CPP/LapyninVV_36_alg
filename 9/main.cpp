#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <chrono>
#include <iomanip>
#include <algorithm>
#include <unordered_map>
#include <fstream>

class SHA1 {
private:
    uint32_t h0, h1, h2, h3, h4;
    
    static uint32_t leftRotate(uint32_t n, uint32_t d) {
        return (n << d) | (n >> (32 - d));
    }

    void processBlock(const uint8_t* block) {
        uint32_t w[80];
        
        for(int i = 0; i < 16; i++) {
            w[i] = (block[i*4] << 24) | (block[i*4+1] << 16) | 
                   (block[i*4+2] << 8) | (block[i*4+3]);
        }

        for(int i = 16; i < 80; i++) {
            w[i] = leftRotate(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
        }

        uint32_t a = h0;
        uint32_t b = h1;
        uint32_t c = h2;
        uint32_t d = h3;
        uint32_t e = h4;

        for(int i = 0; i < 80; i++) {
            uint32_t f, k;
            
            if(i < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999;
            } else if(i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1;
            } else if(i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDC;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6;
            }

            uint32_t temp = leftRotate(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = leftRotate(b, 30);
            b = a;
            a = temp;
        }

        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
        h4 += e;
    }

public:
    SHA1() {
        reset();
    }

    void reset() {
        h0 = 0x67452301;
        h1 = 0xEFCDAB89;
        h2 = 0x98BADCFE;
        h3 = 0x10325476;
        h4 = 0xC3D2E1F0;
    }

    std::string hash(const std::string& message) {
        reset();
        
        uint64_t ml = message.length() * 8;
        std::vector<uint8_t> padded(message.begin(), message.end());
        
        padded.push_back(0x80);
        
        while((padded.size() * 8) % 512 != 448) {
            padded.push_back(0x00);
        }
        
        for(int i = 7; i >= 0; i--) {
            padded.push_back((ml >> (8 * i)) & 0xFF);
        }

        for(size_t i = 0; i < padded.size(); i += 64) {
            processBlock(&padded[i]);
        }

        std::stringstream ss;
        ss << std::hex << std::setfill('0')
           << std::setw(8) << h0
           << std::setw(8) << h1
           << std::setw(8) << h2
           << std::setw(8) << h3
           << std::setw(8) << h4;
        return ss.str();
    }
};

std::string generateRandomString(int length) {
    static const char charset[] = "0123456789"
                                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                "abcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, sizeof(charset) - 2);
    
    std::string str;
    str.reserve(length);
    for(int i = 0; i < length; ++i) {
        str += charset[dis(gen)];
    }
    return str;
}

size_t findLongestCommonSubstring(const std::string& str1, const std::string& str2) {
    size_t maxLen = 0;
    for(size_t i = 0; i < str1.length(); i++) {
        for(size_t j = 0; j < str2.length(); j++) {
            size_t len = 0;
            while(i + len < str1.length() && 
                  j + len < str2.length() && 
                  str1[i + len] == str2[j + len]) {
                len++;
            }
            maxLen = std::max(maxLen, len);
        }
    }
    return maxLen;
}

int main() {
    SHA1 sha1;
    std::ofstream results("results.txt");
    
    std::cout << "Running Test 1..." << std::endl;
    results << "Test 1 Results:\n";
    std::vector<int> differences = {1, 2, 4, 8, 16};
    
    for(int diff : differences) {
        double avgMaxCommon = 0;
        for(int i = 0; i < 1000; i++) {
            std::string str1 = generateRandomString(128);
            std::string str2 = str1;
            
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<> dis(0, 127);
            for(int j = 0; j < diff; j++) {
                int pos = dis(gen);
                str2[pos] = str2[pos] == 'A' ? 'B' : 'A';
            }
            
            std::string hash1 = sha1.hash(str1);
            std::string hash2 = sha1.hash(str2);
            
            avgMaxCommon += findLongestCommonSubstring(hash1, hash2);
        }
        avgMaxCommon /= 1000;
        results << "Differences: " << diff << ", Avg max common length: " << avgMaxCommon << "\n";
    }
    
    std::cout << "Running Test 2..." << std::endl;
    results << "\nTest 2 Results:\n";
    for(int i = 2; i <= 6; i++) {
        int N = std::pow(10, i);
        std::unordered_map<std::string, int> hashCount;
        
        for(int j = 0; j < N; j++) {
            std::string str = generateRandomString(256);
            hashCount[sha1.hash(str)]++;
        }
        
        int collisions = 0;
        for(const auto& pair : hashCount) {
            if(pair.second > 1) {
                collisions += pair.second - 1;
            }
        }
        
        results << "N = " << N << ", Collisions: " << collisions << "\n";
    }
    
    std::cout << "Running Test 3..." << std::endl;
    results << "\nTest 3 Results:\n";
    std::vector<int> sizes = {64, 128, 256, 512, 1024, 2048, 4096, 8192};
    
    for(int size : sizes) {
        auto start = std::chrono::high_resolution_clock::now();
        
        for(int i = 0; i < 1000; i++) {
            std::string str = generateRandomString(size);
            sha1.hash(str);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        double avgTime = duration.count() / 1000.0;
        
        results << "Size: " << size << ", Average time (microseconds): " << avgTime << "\n";
    }
    
    results.close();
    std::cout << "All tests completed. Results written to results.txt" << std::endl;
    return 0;
}
