package com.lingyiwanwu.descartes;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

public class GraphIndex {

    private long ptr = 0;

    // index init from config file
    public native int init(String configFilePath);

    // add vector to index
    public int addVector(float[] vector, long key) {
        byte[] byteVector = floatsToBytes(vector);
        return addVectorInternal(byteVector, vector.length * Float.BYTES, key);
    }

    private native int addVectorInternal(byte[] vector, long bytes, long key);

    // search vector in index with context
    public int search(float[] vector, SearchContext context) {
        byte[] byteVector = floatsToBytes(vector);
        byte[] resBytes = this.searchInternal(byteVector, vector.length * Float.BYTES, context);
        ByteBuffer buffer = ByteBuffer.wrap(resBytes);
        // 切换为小端字节序
        buffer.order(ByteOrder.LITTLE_ENDIAN);
        int res = buffer.getInt();
        // convert byte[] to List<Entity>
        for (int i = Integer.BYTES; i < resBytes.length; i += Long.BYTES + Float.BYTES) {
            long key = buffer.getLong(i);
            float score = buffer.getFloat(i + Long.BYTES);
            context.results.add(new Entity(key, score));
        }
        return res;
    }

    private native byte[] searchInternal(byte[] vector, long bytes, SearchContext context);

    // refine the index. Will quantize the index if  quantize is true
    public native int refineIndex(boolean quantize);

    // close index
    public native int close();

    // dump index
    public native int dump();

    public native int getCurrentDocCnt();

    private static byte[] floatsToBytes(float[] vector) {
        ByteBuffer byteBuffer = ByteBuffer.allocate(vector.length * Float.BYTES);
        byteBuffer.order(ByteOrder.LITTLE_ENDIAN);
        for (float v : vector) {
            byteBuffer.putFloat(v);
        }
        return byteBuffer.array();
    }
}
