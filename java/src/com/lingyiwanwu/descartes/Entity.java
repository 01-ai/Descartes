package com.lingyiwanwu.descartes;

public class Entity {
    private long key;
    private float score;

    public Entity(long key, float score) {
        this.key = key;
        this.score = score;
    }

    public long getKey() {
        return key;
    }

    public void setKey(long key) {
        this.key = key;
    }

    public float getScore() {
        return score;
    }

    public void setScore(float score) {
        this.score = score;
    }
}
