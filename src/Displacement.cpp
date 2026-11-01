#include "Displacement.h"
#include <sstream>
#include <utility>

static bool readInt(const std::string& text, int& out){
    std::istringstream in(text);
    in >> out;
    return !in.fail();
}

static bool readFloats(const std::string& text, std::vector<float>& out){
    std::istringstream in(text);
    float value;
    while(in >> value){
        out.push_back(value);
    }
    return in.eof();
}

static bool readRows(KvNode& dispinfo, const std::string& blockName, int rowCount, int valuesPerRow, std::vector<float>& out){
    KvNode* block = findChild(dispinfo, blockName);
    if (block == nullptr){
        return false;
    }
    for (int r = 0; r < rowCount; r++){
        KvNode* rowNode = findChild(*block, "row" + std::to_string(r));
        if (rowNode == nullptr){
            return false;
        }
        size_t sizeBefore = out.size();
        if (!readFloats(rowNode->value, out)){
            return false;
        }
        if (out.size()- sizeBefore != static_cast<size_t>(valuesPerRow)) {
            return false;
        }
    }
    return true;
}


bool collectDisplacements(KvNode& node, std::vector<Displacement>& out){
    if (node.isBlock && node.key == "side") {
        KvNode* dispinfo = findChild(node, "dispinfo");
        if (dispinfo != nullptr) {
            KvNode* powerNode = findChild(*dispinfo, "power");
            if (powerNode == nullptr) {
                return false;
            }
            Displacement disp;
            disp.side = &node;
            disp.dispinfo = dispinfo;
            if (!readInt(powerNode->value, disp.power)) {
                return false;
            }
            if (disp.power < 2 || disp.power > 4) {
                return false;
            }
            int n = (1 << disp.power) + 1;

            if (!readRows(*dispinfo, "distances", n, n, disp.distances)) {
                return false;
            }
            if (!readRows(*dispinfo, "normals", n, n * 3, disp.normals)) {
                return false;
            }

            out.push_back(std::move(disp));
        }
    }
    for (KvNode& child : node.children) {
        if (!collectDisplacements(child, out)) {
            return false;
        }
        
    }
    return true;
}