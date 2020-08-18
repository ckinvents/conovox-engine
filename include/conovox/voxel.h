#ifndef CONOVOX_VOXEL_H
#define CONOVOX_VOXEL_H

#include <conovox/core.h>
#include <conovox/shader.h>
#include <vector>

namespace Conovox {
    typedef struct {
        char matID;
        unsigned char sides;
        int instances[6];
    } Voxel;

    typedef struct {
        unsigned int offset;
    } RenderVoxel;

    typedef struct {
        glm::vec3 color;
        float density;
        float bindForce;
        float hardness;
    } Material;

    class VoxChunk {
        private:
            // Data members
            std::vector<Voxel> voxels;
            std::vector<Material> materials;
            std::vector<RenderVoxel> voxelInstances[6];
            std::vector<glm::vec3> colorBuffer;
            unsigned int width;
            unsigned int depth;
            unsigned int height;
            float scale;
            glm::vec3 offset;
            glm::mat4 model;
            // Render members
            unsigned int voxTex;
            unsigned int voxelVBOs[6];
            unsigned int staticVBO;
            unsigned int instanceVBOs[6];
            unsigned int voxelVAOs[6];
            void bufferData();
            void generateGLData();
            // Voxel update functions
            Voxel& getVoxelRef(unsigned int x, unsigned int y, unsigned int z);
            void setVoxColor(unsigned int x, unsigned int y, unsigned int z, glm::vec3 color);
            unsigned int getOffset(unsigned int x, unsigned int y, unsigned int z);
            void changeVoxel(unsigned int x, unsigned int y, unsigned int z, char matID);
            void updateVoxel(unsigned int x, unsigned int y, unsigned int z, char matID);
            void insertVoxel(unsigned int x, unsigned int y, unsigned int z, char matID);
            void removeVoxel(unsigned int x, unsigned int y, unsigned int z);
            void showVoxel(unsigned int x, unsigned int y, unsigned int z);
            void hideVoxel(unsigned int x, unsigned int y, unsigned int z);
            // Side check functions
            void checkSides(unsigned int x, unsigned int y, unsigned int z);
            void addSide(unsigned int x, unsigned int y, unsigned int z, unsigned char side);
            void removeSide(unsigned int x, unsigned int y, unsigned int z, unsigned char side);
            void showSide(unsigned int x, unsigned int y, unsigned int z, unsigned char side);
            void hideSide(unsigned int x, unsigned int y, unsigned int z, unsigned char sice);
            glm::vec3 getMatColor(char matID);
        public:
            // Constructors
            VoxChunk(std::vector<Material> materials, unsigned int width, unsigned int depth, unsigned int height, float scale, glm::vec3 offset);
            VoxChunk(std::vector<char> matData, std::vector<Material> materials, unsigned int width, unsigned int depth, unsigned int height, float scale, glm::vec3 offset);
            
            // Accessors & mutators
            char getVoxel(unsigned int x, unsigned int y, unsigned int z);
            void setVoxel(unsigned int x, unsigned int y, unsigned int z, char matID);
            void fillVoxels(char type, unsigned int startX, unsigned int startY, unsigned int startZ, unsigned int offX, unsigned int offY, unsigned int offZ);

            // Coordinate conversions
            glm::vec3 getVoxelWorldCoords(glm::vec3 voxelCoords);
            glm::vec3 getWorldVoxelCoords(glm::vec3 worldCoords);

            // Voxel modification functions
            void setSphere(int x, int y, int z, int radius, char matID);

            // Renderer
            void draw(Shader shader);
    };
};

#endif //CONOVOX_VOXEL_H