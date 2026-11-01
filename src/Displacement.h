#pragma once
#include "KeyValues.h"

// Displacement struct
struct Displacement {
    KvNode* side = nullptr; // The brush side that this displacement belongs to, we never want to own this, as we will only work with Displacements directly
    KvNode* dispinfo = nullptr;
    int power = 0;
    std::vector<float> distances; // How far along the normal they travel - (2^power + 1)^2 values, row by row
    std::vector<float> normals; // The direction the displacement points travels in - 3 per vertex, xyz

};

bool collectDisplacements(KvNode& node, std::vector<Displacement>& out);