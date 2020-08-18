#include <conovox/shader.h>
#include <iostream>
#include <fstream>

// Internal check function
int getShaderCompStat(unsigned int shader, const char* type);
int getProgramLinkStat(unsigned int program);

#define ERROR_BUF_SIZE 1024

namespace Conovox {
    // Create shader program
    Shader::Shader(const char* vertPath, const char* fragPath)
    {
        // Source pointers
        char* vertCode;
        char* fragCode;
        std::ifstream vertFile;
        std::ifstream fragFile;
        // Attempt to open files
        vertFile.open(vertPath, std::ios::ate);
        fragFile.open(fragPath, std::ios::ate);
        if (vertFile.is_open() && fragFile.is_open())
        {
            // Get file sizes
            std::streampos vertFileSize = vertFile.tellg();
            std::streampos fragFileSize = fragFile.tellg();
            // Allocate buffers for shaders
            vertCode = new (std::nothrow) char[(unsigned int)vertFileSize + 1];
            fragCode = new (std::nothrow) char[(unsigned int)fragFileSize + 1];
            if (!vertCode || !fragCode)
            {
                std::cerr << "ERROR: Could not allocate string buffer! Exiting...\n";
                glID = 0;
            }
            else
            {
                // Fill buffers from files
                vertFile.seekg(0);
                fragFile.seekg(0);
                vertFile.read(vertCode, vertFileSize);
                fragFile.read(fragCode, fragFileSize);
                vertCode[vertFileSize] = '\0';
                fragCode[fragFileSize] = '\0';
                // Close IO
                vertFile.close();
                fragFile.close();

                // Create shaders
                unsigned int vertShader;
                unsigned int fragShader;
                vertShader = glCreateShader(GL_VERTEX_SHADER);
                fragShader = glCreateShader(GL_FRAGMENT_SHADER);
                if (!vertShader || !fragShader)
                {
                    std::cerr << "ERROR: Failed to create shaders! Exiting\n";
                    glID = 0;
                }
                else
                {
                    // Attempt to create new shader program
                    glID = glCreateProgram();
                    if (!glID)
                    {
                        fprintf(stderr, "ERROR: Failed to create shader program! Exiting...\n");
                        glID = 0;
                    }
                    else
                    {
                        // Compile and link
                        glShaderSource(vertShader, 1, (const char**)&vertCode, NULL);
                        glShaderSource(fragShader, 1, (const char**)&fragCode, NULL);
                        glCompileShader(vertShader);
                        glCompileShader(fragShader);
                        if (!getShaderCompStat(vertShader, "Vertex") || !getShaderCompStat(fragShader, "Fragment"))
                        {
                            std::cerr << "ERROR: Failed to compile shaders! Exiting...\n";
                            glDeleteProgram(glID);
                            glID = 0;
                        }
                        else
                        {
                            glAttachShader(glID, vertShader);
                            glAttachShader(glID, fragShader);
                            glLinkProgram(glID);
                            if (!getProgramLinkStat(glID))
                            {
                                std::cerr << "ERROR: Could not link shader program! Exiting...\n";
                                glDeleteProgram(glID);
                                glID = 0;
                            }
                        }
                    }
                }
                // Delete shader objects
                glDeleteShader(vertShader);
                glDeleteShader(fragShader);
            }
            // Cleanup buffers
            delete[](vertCode);
            delete[](fragCode);
        }
        else
        {
            // File IO failed, do not set glID
            glID = 0;
        }
    }

    // Enable shader
    void Shader::use()
    {
        glUseProgram(glID);
    }

    // Set boolean uniform
    void Shader::setBool(const char* name, bool value)
    {
        glUniform1i(glGetUniformLocation(glID, name), (int)value);
    }
    // Set integer uniform
    void Shader::setInt(const char* name, int value)
    {
        glUniform1i(glGetUniformLocation(glID, name), value);
    }
    // Set float uniform
    void Shader::setFloat(const char* name, float value)
    {
        glUniform1f(glGetUniformLocation(glID, name), value);
    }
    // Set vec3 uniform
    void Shader::setVec3(const char* name, float x, float y, float z)
    {
        glUniform3f(glGetUniformLocation(glID, name), x, y, z);
    }
    void Shader::setVec3(const char* name, float* value)
    {
        glUniform3fv(glGetUniformLocation(glID, name), 1, value);
    }
    // Set vec4 uniform
    void Shader::setVec4(const char* name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(glID, name), x, y, z, w);
    }
    void Shader::setVec4(const char* name, float* value)
    {
        glUniform4fv(glGetUniformLocation(glID, name), 1, value);
    }
    // Set matrix uniform
    void Shader::setMatrix(const char* name, float* value)
    {
        glUniformMatrix4fv(glGetUniformLocation(glID, name), 1, GL_FALSE, value);
    }

    unsigned int Shader::getID() const
    {
        return glID;
    }
};

// Shader status message, retrieves result from compiler
int getShaderCompStat(unsigned int shader, const char* type)
{
    int success;
    char infoLog[ERROR_BUF_SIZE];
    // Was shader compilation successful?
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    // If not, return error
    if (!success)
    {
        glGetShaderInfoLog(shader, ERROR_BUF_SIZE, NULL, infoLog);
        std::cerr << "ERROR: " << type << " shader compilation failed:\n" 
                << infoLog << std::endl;
    }
    return success;
}

// Program status messsage, retrieves result from linker
int getProgramLinkStat(unsigned int program)
{
    int success;
    char infoLog[ERROR_BUF_SIZE];
    // Was program linking successful?
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    // If not, return error
    if (!success)
    {
        glGetProgramInfoLog(program, ERROR_BUF_SIZE, NULL, infoLog);
        std::cerr << "ERROR: Shader linking failed:\n" 
                << infoLog << std::endl;
    }
    return success;
}
