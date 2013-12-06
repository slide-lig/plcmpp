// Generated from /jLCM/src/main/java/fr/liglab/mining/io/FileCollector.java

#pragma once

#include <io/PatternsCollector.hpp>

#include <cstdint>
#include <fstream>
#include <string>
using namespace std;

namespace io {

class FileCollector : public virtual PatternsCollector
{

protected:
    int64_t collected;
    int64_t collectedLength;
    ofstream* stream;	// note: ofstream is internally buffered

public:
    void collect(int32_t support, vector<uint32_t>* pattern) override;
    int64_t close() override;
    int32_t getAveragePatternLength() override;
    virtual int64_t getCollected();
    virtual int64_t getCollectedLength();

    FileCollector(string &path);
    ~FileCollector();
};

}
