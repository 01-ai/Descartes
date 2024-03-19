#pragma once
#include <vector>
#include <memory>

namespace descartes {

struct SearchContext {
    int  topk{10};
    int  searchResCnt{20};
    float exploreFactor{1.0f};
    std::vector<std::pair<uint64_t, float>> results;
};
using SearchContextPtr = std::shared_ptr<SearchContext>;

class GraphIndex {
public:
    GraphIndex() = default;
    virtual ~GraphIndex() = default;

public:
    virtual int Init(const std::string &configFilePath) = 0;
    virtual int AddVector(const void *vector, size_t bytes, uint64_t key) = 0;
    virtual int Search(const void *vector, size_t bytes, SearchContext &context) = 0;
    virtual int RefineIndex(bool quantize = true) = 0;
    virtual int Dump() = 0;
    
    virtual uint32_t GetCurrentDocCnt() const = 0;
};
using GraphIndexPtr = std::shared_ptr<GraphIndex>;

GraphIndexPtr __attribute__ ((visibility("default")))  CreateGraphIndex();

};
