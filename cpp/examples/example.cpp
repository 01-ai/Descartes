#include <gflags/gflags.h>
#include <H5Cpp.h>
#include <assert.h>
#include <memory>
#include <chrono>
#include <stdio.h>
#include <string>
#include <set>
#include <algorithm>

#include "descartes_index.h"

using namespace descartes;

static bool Validate(const char* flagname, const std::string &value) 
{
    if (!value.empty()) {
        return true;
    }
    fprintf(stderr, "--%s should not be empty\n", flagname);
    return false;
}

DEFINE_string(dataset_file, "", "dataset file, mandatory");
DEFINE_string(config_file, "", "graph configure file, mandatory");
DEFINE_uint32(search_res_cnt, 40, "search result cnt. default 40");

DEFINE_validator(config_file, &Validate);
DEFINE_validator(dataset_file, &Validate);

class Timer {
public:
    Timer() {
        _start = std::chrono::steady_clock::now();
    }
    
    uint64_t ElapsedSeconds() const {
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<float> duration = end - _start;
        return duration.count();
    }

    uint64_t ElapsedMilliseconds() const {
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<float, std::milli> duration = end - _start;
        return duration.count();
    }
    
    uint64_t ElapsedMicroseconds() const {
        auto end = std::chrono::steady_clock::now();
        std::chrono::duration<float, std::micro> duration = end - _start;
        return duration.count();
    }

private:
    std::chrono::time_point<std::chrono::steady_clock> _start;
};

int main(int argc, char *argv[])
{
    std::string usage = "--dataset_file=sift.hdf5 --config_file=./fng.cfg ";
    gflags::SetUsageMessage(usage);
    gflags::ParseCommandLineFlags(&argc, &argv, true);
    
    int dimension = 0;
    // read train data from hdh5 file
    H5::H5File file(FLAGS_dataset_file, H5F_ACC_RDONLY);
    H5::DataSet trainDataSet = file.openDataSet("train");
    H5::DataSpace space = trainDataSet.getSpace();
    hsize_t shape[2];
    int dim = space.getSimpleExtentDims(shape);
    assert(dim = 2);
    std::unique_ptr<float[]> vectors(new float[shape[0] * shape[1]]);
    trainDataSet.read(vectors.get(), H5::PredType::NATIVE_FLOAT, space);
    fprintf(stderr, "Loaded train vector from file[%s]. count[%lu] dimension[%lu]\n", 
        FLAGS_dataset_file.c_str(), shape[0], shape[1]);
    dimension = shape[1];

    // build index
    GraphIndexPtr indexPtr = CreateGraphIndex();
    assert(indexPtr != nullptr);
    assert(indexPtr->Init(FLAGS_config_file) == 0);

    if (indexPtr->GetCurrentDocCnt() == 0) {
#pragma omp parallel for
        for (hsize_t i = 0; i < shape[0]; ++i) {
            int ret = indexPtr->AddVector(vectors.get() + i * shape[1], sizeof(float) * shape[1], i);
            assert(ret == 0);
        }
    }
    assert(indexPtr->RefineIndex(true) == 0);

    // read test data from hdh5 file
    H5::DataSet testDataSet = file.openDataSet("test");
    space = testDataSet.getSpace();
    dim = space.getSimpleExtentDims(shape);
    assert(dim = 2);
    std::unique_ptr<float[]> query(new float[shape[0] * shape[1]]);
    testDataSet.read(query.get(), H5::PredType::NATIVE_FLOAT, space);
    fprintf(stderr, "Loaded test vector from file[%s]. count[%lu] dimension[%lu]\n", 
        FLAGS_dataset_file.c_str(), shape[0], shape[1]);
    assert(dimension == shape[1]);

    // read neighbors 
     H5::DataSet neighborsDataSet = file.openDataSet("neighbors");
    space = neighborsDataSet.getSpace();
    dim = space.getSimpleExtentDims(shape);
    assert(dim = 2);
    std::unique_ptr<int32_t[]> neighbors(new int32_t[shape[0] * shape[1]]);
    neighborsDataSet.read(neighbors.get(), H5::PredType::NATIVE_INT32, space);
    fprintf(stderr, "Loaded neighbors from file[%s]. count[%lu] dimension[%lu]\n", 
        FLAGS_dataset_file.c_str(), shape[0], shape[1]);

    // compute recall
    Timer timer;
    SearchContext ctx;
    ctx.topk = 10;
    ctx.searchResCnt = FLAGS_search_res_cnt;
    float recallSum = 0;
    for (hsize_t i = 0; i < shape[0]; ++i) {
        int ret = indexPtr->Search(query.get() + i * dimension, sizeof(float) * dimension, ctx);
        assert(ret == 0);
        std::set<uint64_t> fngSet;
        std::set<uint64_t> oriSet;

        for (size_t j = 0; j < ctx.results.size(); ++j) {
            fngSet.insert(ctx.results[j].first);
            oriSet.insert(*(neighbors.get() + i * shape[1] + j));
        }

        std::set<uint32_t> intersection;
        std::set_intersection(fngSet.begin(), fngSet.end(), 
                              oriSet.begin(), oriSet.end(),
                              std::inserter(intersection, intersection.begin()));

        recallSum += ((float)intersection.size()) / ctx.results.size();
        if ((i + 1) % 1000 == 0) {
            fprintf(stderr, "Processed %lu\n", i + 1);
        }
    }
    fprintf(stdout, "query count %lu. topk %d. recall %f\n", shape[0], ctx.topk, recallSum / shape[0]);
    assert(indexPtr->Dump() == 0);
}
