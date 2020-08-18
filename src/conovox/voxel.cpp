#include <conovox/voxel.h>
#include <random>
#include <iostream>

namespace Conovox {

    typedef enum {
        VS_N = 0b00000001,
        VS_E = 0b00000010,
        VS_S = 0b00000100,
        VS_W = 0b00001000,
        VS_U = 0b00010000,
        VS_D = 0b00100000,
        VS_A = 0b00111111
    } VoxSide;

    const unsigned char FACE_SIZE = 36;
    const float VOX_CUBEVERT[] = {
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,

         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f    
    };

    const int RAND_SEED = 69420;

    // Buffer new data to instanceVBO
    void VoxChunk::bufferData()
    {
        for (int i = 0; i < 6; i++)
        {
            if (voxelInstances[i].size() > 0)
            {
                glBindBuffer(GL_ARRAY_BUFFER, instanceVBOs[i]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(RenderVoxel) * voxelInstances[i].size(), &voxelInstances[i][0], GL_DYNAMIC_DRAW);
            }
        }
    }

    // Configure buffers and attributes
    void VoxChunk::generateGLData()
    {
        // Create 3D texture
        glGenTextures(1, &voxTex);
        glBindTexture(GL_TEXTURE_3D, voxTex);
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        // Initialize VAO and VBO
        glGenBuffers(6, voxelVBOs);
        glGenBuffers(6, instanceVBOs);
        glGenVertexArrays(6, voxelVAOs);
        for (int i = 0; i < 6; i++)
        {
            // Fill VBO & bind VAO
            glBindVertexArray(voxelVAOs[i]);
            // Configure cube VBO
            glBindBuffer(GL_ARRAY_BUFFER, voxelVBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * FACE_SIZE, &VOX_CUBEVERT[FACE_SIZE * i], GL_STATIC_DRAW);
            // Configure attributes for cube mesh
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)0);
            glEnableVertexAttribArray(0); // Vertex coords
            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 6, (void*)(sizeof(float) * 3));
            glEnableVertexAttribArray(1); // Normal coords
            // Configure instance VBO
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBOs[i]);
            glVertexAttribIPointer(2, 3, GL_UNSIGNED_INT, sizeof(RenderVoxel), (void*)offsetof(RenderVoxel, offset));
            glEnableVertexAttribArray(2);
            glVertexAttribDivisor(2, 1);
        }
        // Unbind VAO
        glBindVertexArray(0);
    }

    // Return reference to voxel in voxels
    Voxel& VoxChunk::getVoxelRef(unsigned int x, unsigned int y, unsigned int z)
    {
        return voxels[y * width * depth + z * width + x];
    }

    void VoxChunk::setVoxColor(unsigned int x, unsigned int y, unsigned int z, glm::vec3 color)
    {
        colorBuffer[y * width * depth + z * width + x] = glm::uvec3(color * 255.0f);
    }

    void VoxChunk::changeVoxel(unsigned int x, unsigned int y, unsigned int z, char matID)
    {
        char voxType = getVoxel(x, y, z);
        if (matID != voxType)
        {
            // Check that voxel isn't being deleted
            if (matID < 0)
            {
                removeVoxel(x, y, z);
            }
            // Voxel is being set to something
            else
            {
                // Voxel already present, simply update in place
                if (voxType >= 0)
                { 
                    updateVoxel(x, y, z, matID);
                }
                // Voxel does not exist, perform add & update arrays
                else
                {
                    insertVoxel(x, y, z, matID);
                }
            }
        }
    }

    // Internal voxel update functions
    void VoxChunk::updateVoxel(unsigned int x, unsigned int y, unsigned int z, char matID)
    {
        Voxel& voxel = getVoxelRef(x, y, z);
        voxel.matID = matID;
        // If voxel is visible, update its instance too
        for (int i = 0; i < 6; i++)
        {
            if (voxel.instances[i] >= 0)
            {
                setVoxColor(x, y, z, getMatColor(matID));
            }
        }
    }

    void VoxChunk::insertVoxel(unsigned int x, unsigned int y, unsigned int z, char matID)
    {
        Voxel& voxel = getVoxelRef(x, y, z);
        voxel.matID = matID;
        // Check if voxel visible
        checkSides(x, y, z);
        // Update bordering voxels
        if (z > 0)
        {
            addSide(x, y, z-1, VS_N);
        }
        if (x > 0)
        {
            addSide(x-1, y, z, VS_E);
        }
        if (z < depth-1)
        {
            addSide(x, y, z+1, VS_S);
        }
        if (x < width-1)
        {
            addSide(x+1, y, z, VS_W);
        }
        if (y > 0)
        {
            addSide(x, y-1, z, VS_U);
        }
        if (y < height-1)
        {
            addSide(x, y+1, z, VS_D);
        }
    }

    void VoxChunk::removeVoxel(unsigned int x, unsigned int y, unsigned int z)
    {
        Voxel& voxel = getVoxelRef(x, y, z);
        voxel.matID = -1;
        voxel.sides = 0;
        // Clear render reference
        hideVoxel(x, y, z);
        // Update bordering voxels
        if (z > 0)
        {
            removeSide(x, y, z-1, VS_N);
        }
        if (x > 0)
        {
            removeSide(x-1, y, z, VS_E);
        }
        if (z < depth-1)
        {
            removeSide(x, y, z+1, VS_S);
        }
        if (x < width-1)
        {
            removeSide(x+1, y, z, VS_W);
        }
        if (y > 0)
        {
            removeSide(x, y-1, z, VS_U);
        }
        if (y < height-1)
        {
            removeSide(x, y+1, z, VS_D);
        }
    }

    void VoxChunk::showVoxel(unsigned int x, unsigned int y, unsigned int z)
    {
        Voxel& voxel = getVoxelRef(x, y, z);
        for (int i = 0; i < 6; i++)
        {
            RenderVoxel instance;
            instance.offset = getOffset(x, y, z);
            // Add new instances to end
            voxelInstances[i].push_back(instance);
            voxel.instances[i] = voxelInstances[i].size() - 1;
        }
    }

    void VoxChunk::hideVoxel(unsigned int x, unsigned int y, unsigned int z)
    {
        Voxel& voxel = getVoxelRef(x, y, z);
        for (int i = 0; i < 6; i++)
        {
            if (!(voxel.instances[i] == -1))
            {
                // Swap deleted instance with end of array and shrink
                unsigned int endVoxOffset = voxelInstances[i][voxelInstances[i].size() - 1].offset;
                Voxel& endVoxel = voxels[endVoxOffset];
                endVoxel.instances[i] = voxel.instances[i];
                voxelInstances[i][voxel.instances[i]] = voxelInstances[i][voxelInstances[i].size() - 1];
                voxelInstances[i].pop_back();
                voxel.instances[i] = -1;
            }
        }
    }

    // Side check functions (ASSUMES NEW (THUS HIDDEN) VOXEL, otherwise use individual side update functions)
    void VoxChunk::checkSides(unsigned int x, unsigned int y, unsigned int z)
    {
        Voxel& voxel = getVoxelRef(x, y, z);
        if (voxel.matID >= 0)
        {
            voxel.sides = (VS_N * (getVoxel(x, y, z+1) >= 0)) |
                        (VS_E * (getVoxel(x+1, y, z) >= 0)) |
                        (VS_S * (getVoxel(x, y, z-1) >= 0)) |
                        (VS_W * (getVoxel(x-1, y, z) >= 0)) |
                        (VS_U * (getVoxel(x, y+1, z) >= 0)) |
                        (VS_D * (getVoxel(x, y-1, z) >= 0));
            // All sides are obscured
            if (voxel.sides == VS_A)
            {
                hideVoxel(x, y, z);
            }
            // All sides are visible
            else if (voxel.sides == 0)
            {
                showVoxel(x, y, z);
            }
            // Reveal individual sides
            else
            {
                if (!(voxel.sides & VS_N))
                {
                    showSide(x, y, z, VS_N);
                }
                if (!(voxel.sides & VS_E))
                {
                    showSide(x, y, z, VS_E);
                }
                if (!(voxel.sides & VS_S))
                {
                    showSide(x, y, z, VS_S);
                }
                if (!(voxel.sides & VS_W))
                {
                    showSide(x, y, z, VS_W);
                }
                if (!(voxel.sides & VS_U))
                {
                    showSide(x, y, z, VS_U);
                }
                if (!(voxel.sides & VS_D))
                {
                    showSide(x, y, z, VS_D);
                }
            }
        }
    }

    void VoxChunk::addSide(unsigned int x, unsigned int y, unsigned int z, unsigned char side)
    {
        Voxel& voxel = getVoxelRef(x, y, z);
        if (voxel.matID >= 0)
        {
            voxel.sides |= side;
            hideSide(x, y, z, side);
        }
    }

    void VoxChunk::removeSide(unsigned int x, unsigned int y, unsigned int z, unsigned char side)
    {
        Voxel& voxel = getVoxelRef(x, y, z);
        if (voxel.matID >= 0)
        {
            voxel.sides &= !(side);
            // Check if voxel was hidden, show if true
            showSide(x, y, z, side);
        }
    }

    void VoxChunk::showSide(unsigned int x, unsigned int y, unsigned int z, unsigned char side)
    {
        int sideNum = __builtin_ctz(side);
        Voxel& voxel = getVoxelRef(x, y, z);
        RenderVoxel instance;
        setVoxColor(x, y, z, getMatColor(voxel.matID));
        instance.offset = getOffset(x, y, z);
        // Add new instances to end
        voxelInstances[sideNum].push_back(instance);
        voxel.instances[sideNum] = voxelInstances[sideNum].size() - 1;
    }

    void VoxChunk::hideSide(unsigned int x, unsigned int y, unsigned int z, unsigned char side)
    {
        int sideNum = __builtin_ctz(side);
        Voxel& voxel = getVoxelRef(x, y, z);
        unsigned int endVoxOffset = voxelInstances[sideNum][voxelInstances[sideNum].size() - 1].offset;
        Voxel& endVoxel = voxels[endVoxOffset];
        endVoxel.instances[sideNum] = voxel.instances[sideNum];
        voxelInstances[sideNum][voxel.instances[sideNum]] = voxelInstances[sideNum][voxelInstances[sideNum].size() - 1];
        voxelInstances[sideNum].pop_back();
        voxel.instances[sideNum] = -1;
    }

    glm::vec3 VoxChunk::getMatColor(char matID)
    {
        return materials[matID].color + 0.1f * (float)rand()/RAND_MAX;
    }

    // Constructors
    VoxChunk::VoxChunk(std::vector<Material> materials, unsigned int width, unsigned int depth, unsigned int height, float scale, glm::vec3 offset)
    {
        // Allocate empty vector of voxels
        for (unsigned long i = 0; i < width * depth * height; i++)
        {
            Voxel voxel;
            voxel.matID = -1;
            for (int i = 0; i < 6; i++)
            {
                voxel.instances[i] = -1;
            }
            voxel.sides = 0;
            voxels.push_back(voxel);
            colorBuffer.push_back(glm::vec3(0.0f));
        }
        this->materials = materials;
        this->width = width;
        this->depth = depth;
        this->height = height;
        this->scale = scale;
        this->offset = offset;
        model = glm::scale(glm::translate(glm::mat4(1.0f), offset), glm::vec3(scale));
        generateGLData();
        srand(RAND_SEED);
    }

    VoxChunk::VoxChunk(std::vector<char> matData, std::vector<Material> materials, unsigned int width, unsigned int depth, unsigned int height, float scale, glm::vec3 offset)
    {
        // Allocate empty vector of voxels
        for (unsigned int i = 0; i < width * depth * height; i++)
        {
            Voxel voxel;
            voxel.matID = -1;
            for (int i = 0; i < 6; i++)
            {
                voxel.instances[i] = -1;
            }
            voxel.sides = 0;
            voxels.push_back(voxel);
        }
        this->materials = materials;
        this->width = width;
        this->depth = depth;
        this->height = height;
        this->scale = scale;
        this->offset = offset;
        model = glm::scale(glm::translate(glm::mat4(1.0f), offset), glm::vec3(scale));
        generateGLData();
        srand(RAND_SEED);
        // Fill voxel array (TEMPORARY, VERY INEFFICIENT BC OF UPDATE FUNCTIONS)
        unsigned int total = width * depth * height;
        unsigned int progress = 0;
        std::cout << "Generating terrain..." << std::endl;
        for (unsigned int y = 0; y < height; y++)
        {
            for (unsigned int z = 0; z < depth; z++)
            {
                for (unsigned int x = 0; x < width; x++)
                {
                    changeVoxel(x, y, z, matData[y * width * height + z * width + x]);
                    progress++;
                }
            }
            std::cout << (100.0f * progress) / total << "\% complete" << std::endl;
        }
        std::cout << "Done!" << std::endl;
        bufferData();
    }

    // Accessors & Mutators
    char VoxChunk::getVoxel(unsigned int x, unsigned int y, unsigned int z)
    {
        if (x >= 0 && x < width && y >= 0 && y < height && z >= 0 && z < depth)
        {
            return voxels[y * width * depth + z * width + x].matID;
        }
        else
        {
            return 0;
        }
    }

    void VoxChunk::setVoxel(unsigned int x, unsigned int y, unsigned int z, char matID)
    {
        changeVoxel(x, y, z, matID);
        bufferData();
    }

    // Coordinate conversions
    glm::vec3 VoxChunk::getVoxelWorldCoords(glm::vec3 voxelCoords)
    {
        return scale * voxelCoords + offset;
    }

    glm::vec3 VoxChunk::getWorldVoxelCoords(glm::vec3 worldCoords)
    {
        return (worldCoords - offset) / scale + 0.5f;
    }

    void VoxChunk::setSphere(int x, int y, int z, int radius, char matID)
    {
        int boundStartX;
        int boundStartY;
        int boundStartZ;
        int boundEndX;
        int boundEndY;
        int boundEndZ;
        // Check bounds
        if (x - radius < 0)
        {
            boundStartX = 0;
        }
        else
        {
            boundStartX = x - radius;
        }
        if (x + radius > width)
        {
            boundEndX = width;
        }
        else
        {
            boundEndX = x + radius;
        }
        if (y - radius < 0)
        {
            boundStartY = 0;
        }
        else
        {
            boundStartY = y - radius;
        }
        if (y + radius > height)
        {
            boundEndY = height;
        }
        else
        {
            boundEndY = y + radius;
        }
        if (z - radius < 0)
        {
            boundStartZ = 0;
        }
        else
        {
            boundStartZ = z - radius;
        }
        if (z + radius > depth)
        {
            boundEndZ = depth;
        }
        else
        {
            boundEndZ = z + radius;
        }
        // Get radius squared
        unsigned int dist = radius * radius;
        for (int bY = boundStartY; bY < boundEndY; bY++)
        {
            for (int bX = boundStartX; bX < boundEndX; bX++)
            {
                for (int bZ = boundStartZ; bZ < boundEndZ; bZ++)
                {
                    int dX = bX - x;
                    int dY = bY - y;
                    int dZ = bZ - z;
                    if ((dX*dX + dY*dY + dZ*dZ) <= dist)
                    {
                        changeVoxel(bX, bY, bZ, matID);
                    }
                }
            }
        }
        bufferData();
    }


    // Renderer
    void VoxChunk::draw(Shader shader)
    {
        shader.use();
        shader.setMatrix("model", glm::value_ptr(model));
        for (int i = 0; i < 6; i++)
        {
            if (voxelInstances[i].size() > 0)
            {
                glBindVertexArray(voxelVAOs[i]);
                glDrawArraysInstanced(GL_TRIANGLES, 0, 6, voxelInstances[i].size());
            }
        }
        // Cleanup and reset
        glBindVertexArray(0);
    }
};