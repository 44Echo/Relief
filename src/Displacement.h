#pragma once
#include "KeyValues.h"

// Displacement struct
struct Displacement {
    KvNode* side = nullptr; // The brush side that this displacement belongs to, we never want to own this, as we will only work with Displacements directly
    KvNode* dispinfo = nullptr;
    int power = 0;
};

bool collectDisplacements(KvNode& node, std::vector<Displacement>& out);