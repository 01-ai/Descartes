package com.lingyiwanwu.descartes;

import java.util.ArrayList;
import java.util.List;

public class SearchContext {
    public int topk = 10;
    public int searchResCnt = 20;
    public float exploreFactor = 1.0f;
    public List<Entity> results = new ArrayList<>();
}
