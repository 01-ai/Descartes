package com.lingyiwanwu.descartes.example;

import com.lingyiwanwu.descartes.Entity;
import com.lingyiwanwu.descartes.GraphIndex;
import com.lingyiwanwu.descartes.SearchContext;

import java.util.Random;

public class Sample {
    static {
        // 指定so文件路径
        System.load(System.getProperty("user.dir") + "../lib/libdescartes.so");

        // 将so文件放入系统库
        // System.loadLibrary("libdescartes.so");
    }

    public static void main(String[] args) {

        GraphIndex graphIndex = new GraphIndex();
        // init
        int res = graphIndex.init(System.getProperty("user.dir") +"/src/com/lingyiwanwu/descartes/example/Sample.config");
        System.out.println("init result is: " + res);

        // add vector
        long bytes = 128 * 4;
        long key = 0;
        float[] vector = new float[(int) bytes];
        Random random = new Random();
        for (int i = 0; i < 5000; i++) {
            res = graphIndex.addVector(vector, bytes, random.nextInt());
        }
        System.out.println("addVector result is: " + res);

        // search
        SearchContext context = new SearchContext();
        res = graphIndex.search(vector, bytes, context);
        System.out.println("search result is: " + res);
        System.out.println("size of search result is: " + context.results.size());
        for (int i = 0; i < context.results.size(); i++) {
            Entity entity = context.results.get(i);
            System.out.printf("%d: (%d,%f)%n", i, entity.getKey(), entity.getScore());
        }

        graphIndex.refineIndex(true);
        graphIndex.dump();
        graphIndex.close();
    }
}
