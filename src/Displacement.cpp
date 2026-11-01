#include "Displacement.h"
#include <sstream>

static bool readInt(const std::string& text, int& out){
    std::istringstream in(text);
    in >> out;
    return !in.fail();
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
            out.push_back(disp);
        }
    }
    for (KvNode& child : node.children) {
        if (!collectDisplacements(child, out)) {
            return false;
        }
        
    }
    return true;
}