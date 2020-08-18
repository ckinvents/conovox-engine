#ifndef CONOVOX_SHADER_H
#define CONOVOX_SHADER_H

#include <conovox/core.h>

// Represents a shader program with a vertex and fragment shader
namespace Conovox {
    class Shader
    {
        private:
            // gl ID for shader program
            unsigned int glID;
        public:
            // Methods
            Shader(const char* vertPath, const char* fragPath);
            // Enables shader via gl call
            void use();
            // Sets boolean uniform
            void setBool(const char* name, bool value);
            // Sets integer uniform
            void setInt(const char* name, int value);
            // Sets float uniform
            void setFloat(const char* name, float value);
            // Sets a vec3
            void setVec3(const char* name, float x, float y, float z);
            void setVec3(const char* name, float* value);
            // Sets a vec4
            void setVec4(const char* name, float x, float y, float z, float w);
            void setVec4(const char* name, float* value);
            // Sets 4x4 matrix
            void setMatrix(const char* name, float* value);
            // Get gl object ID
            unsigned int getID() const;
    };
};

#endif //CONOVOX_SHADER_H