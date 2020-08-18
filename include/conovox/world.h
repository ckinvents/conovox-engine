#ifndef CONOVOX_WORLD_H
#define CONOVOX_WORLD_H

#include <conovox/core.h>
#include <conovox/voxel.h>

namespace Conovox {
    class World {
        private:
            unsigned int width;
            unsigned int depth;
            std::vector<VoxChunk> voxels;
            unsigned int voxelVBO;
            unsigned int voxelVAO;
    };
};

#endif //CONOVOX_WORLD_H