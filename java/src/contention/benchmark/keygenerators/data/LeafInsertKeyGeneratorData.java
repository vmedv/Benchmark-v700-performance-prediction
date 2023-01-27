package contention.benchmark.keygenerators.data;

import contention.abstractions.KeyGeneratorData;
import contention.abstractions.Parameters;
import contention.benchmark.tools.Range;
import contention.benchmark.ThreadLoops.workloads.DeleteLeafsWorkload;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;

public class LeafInsertKeyGeneratorData extends KeyGeneratorData {
    private static List<int[]> layers;

    public LeafInsertKeyGeneratorData(Parameters parameters) {
        int size = parameters.range;
        layers = new ArrayList<>();

        List<Range> verts = new ArrayList<>();
        verts.add(new Range(1, size));

        while (!verts.isEmpty()) {
            layers.add(new int[verts.size()]);
            List<Range> nextLayerVerts = new ArrayList<>();

            for (int i=0; i<verts.size(); i++) {
                Range next = verts.get(i);

                if (next.left > next.right) {
                    continue;
                }

                int nextVert = (next.left + next.right) / 2;

                layers.get(layers.size()-1)[i] = nextVert;

                if (next.left == next.right) {
                    continue;
                }

                nextLayerVerts.add(new Range(next.left, nextVert - 1));
                nextLayerVerts.add(new Range(nextVert + 1, next.right));
            }

            verts = nextLayerVerts;
        }
    }

    @Override
    public int get(int index) {
        return -1;
    }

    public int get(int layer, int index) {
        return layers.get(layer)[index];
    }

    public int getLayerSize(int layer){
        return layers.get(layer).length;
    }

    public int getLayersCount(){
        return layers.size();
    }

}
