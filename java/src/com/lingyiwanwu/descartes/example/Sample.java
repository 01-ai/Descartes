package com.lingyiwanwu.descartes.example;

import com.lingyiwanwu.descartes.Entity;
import com.lingyiwanwu.descartes.GraphIndex;
import com.lingyiwanwu.descartes.SearchContext;

public class Sample {
    static {
        // 指定so文件路径
        System.load(System.getProperty("user.dir") + "/../lib/libdescartes.so");

        // 将so文件放入系统库
        // System.loadLibrary("libdescartes.so");
    }

    public static void main(String[] args) {

        GraphIndex graphIndex = new GraphIndex();
        String configPath = System.getProperty("user.dir") +"/src/com/lingyiwanwu/descartes/example/Sample.config";
        // init; if index has dumped before, then will load the existing data
        int res = graphIndex.init(configPath);
        System.out.println("init result is: " + res); // 0 for sucess
        if (res != 0) {
            System.err.println("init failed and exit");
            System.exit(1);
        }

        // same as `vector.global.dimension` in Sample.config
        int dim = 128;
        float[] vector = new float[dim];
        // add vector
        for (int i = 0; i < 5000; i++) {
            vector[0] = (float) i; // meaningless, just for demo
            long key = i;
            res = graphIndex.addVector(vector, key);
        }
        System.out.println("addVector result is: " + res); // 0 for sucess

        // search
        SearchContext context = new SearchContext(); // new context for each search
        context.topk = 10;
        context.searchResCnt = 50;
        context.exploreFactor = 1.0f;
        res = graphIndex.search(vector, context); // 0 for sucess
        System.out.println("search result is: " + res);
        System.out.println("size of search result is: " + context.results.size());
        for (int i = 0; i < context.results.size(); i++) {
            Entity entity = context.results.get(i);
            System.out.printf("%d: (%d,%f)%n", i, entity.getKey(), entity.getScore());
        }

        // optimize the index
        graphIndex.refineIndex(true);
        // index dump path: `vector.global.index_dir` in Sample.config
        graphIndex.dump();
        // IMPORTANT: release the resource when no longer use
        graphIndex.close();
    }
}
