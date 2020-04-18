#pragma once

#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <string>
#include <array>
#include <list>
#include <deque>
#include "gibberishdetector_data.h"

class GibberishDetector
{
    public:
        typedef std::list<std::string> WordByLines;

    private:
        static const size_t DEFAULT_COUNTS_VALUE;

    private:
        typedef std::unordered_map<char, size_t> CharIndex;
        typedef std::pair<char, char> NGram;
        typedef std::list<NGram> NGramList;

    public:
        GibberishDetector() = delete;
        static bool train(const WordByLines& wordlines, const std::string& goodfile, const std::string& badfile);
        static bool isGibberish(const std::string& text);
        static void initialize();

    private:
        template<typename T, size_t N> static GibberishDetectorData::Matrix2D<T, N> fillMatrix(const T& val);
        template<typename Function> static bool lines(const std::string& filename, const Function& cb);
        static NGramList ngram(const std::string& s);
        static std::string normalize(const std::string& s);
        static double avgTransitionProb(const std::string& s, const GibberishDetectorData::MatrixCounts& counts);
        static void initializeCharIndex();
        static void initializeCounts();

    private:
        static GibberishDetectorData::MatrixCounts m_counts;
        static CharIndex m_charidx;
};

template<typename T, size_t N>
GibberishDetectorData::Matrix2D<T, N> GibberishDetector::fillMatrix(const T& val)
{
    GibberishDetectorData::Matrix2D<T, N> m;
    std::for_each(m.begin(), m.end(), [val](typename GibberishDetectorData::Matrix2D<T, N>::value_type& r) { r.fill(val); });
    return m;
}

template<typename Function>
bool GibberishDetector::lines(const std::string& filename, const Function& cb)
{
    std::ifstream ifs(filename);
    if(!ifs.is_open()) return false;
    std::string line;

    while(std::getline(ifs, line)) cb(line);
    return true;
}
