#include "ShaderCompiler.h"

ShaderCompiler::ShaderCompiler()
{
    shaderPath = (char*)malloc(512 * sizeof(char));
#if _DEBUG
    char defaultPath[] = "../resources/shaders/\0";
    memcpy(this->shaderPath, defaultPath, sizeof(defaultPath));
#else
    this->shaderPath = "../resources/shaders/\0";
    //memcpy(this->shaderPath, shaderPath, sizeof(shaderPath));
#endif
}

ShaderCompiler::ShaderCompiler(const char * shaderPath)
{
    memcpy((char*)(this->shaderPath), shaderPath, strlen(shaderPath) + 1);
}

void ShaderCompiler::combileShaderPathAndFile(
    const char*     path,
    const char*     file, 
    OUT const char* filePath)
{
    //memset((char*)filePath, 0, strlen(filePath));
    strcpy((char*)filePath, path);
    strcat((char*)filePath, "/");
    strcat((char*)filePath, file);
    strcat((char*)filePath, "\0");
}

void ShaderCompiler::writeToShaderSource(
    FILE*       pFile,
    UINT        sourceSize,
    UINT*       writtenSize,
    OUT char*   shaderSource)
{
    // Assemption: character in one line no longer than 256 characters
    char line[256];

    UINT size = *writtenSize;

    while (fgets(line, sizeof(line), pFile) != NULL)
    {
        fputs(line, stdout);

        if (strstr(line, "#include") != NULL)
        {
            preprocessingShaderFile(line, sourceSize, &size, shaderSource);
        }
        else
        {
            UINT lineSize = strlen(line) * sizeof(char);

            if (size + lineSize < sourceSize)
            {
                strncat(shaderSource, line, strlen(line));
            }
            else
            {
                printf("Exceed the shader source size, reallocate memory position.\n");
                sourceSize *= 2;
                char* newShaderSource = (char*)realloc(shaderSource, sourceSize);

                if (newShaderSource != nullptr)
                {
                    memcpy(newShaderSource, shaderSource, size);
                    free(shaderSource);

                    shaderSource = newShaderSource;
                    strncat(shaderSource, line, strlen(line));
                }
                else
                {
                    free(shaderSource);
                    printf("Error when reallocate for shader source.\n");
                    assert(FALSE);
                    exit(1);
                }
            }

            size += lineSize;
        }
    }
    char endChar = '\0';
    strncat(shaderSource, &endChar, 1);

    *writtenSize = size;
}

void ShaderCompiler::preprocessingShaderFile(
    const char* line,
    UINT        sourceSize,
    UINT*       writtenSize,
    OUT char*   shaderSource)
{
    char includeFile[256];

    UINT i = 0;
    UINT counter = 0;

    while (line[i] != '\0')
    {
        char c = line[i];

        if (c == '<')
        {
            do
            {
                i += 1;
                c = line[i];

                if (c == '\0' || c == '>') { break; }

                includeFile[counter] = c;
                counter += 1;
            } while (1);

            includeFile[counter] = '\0';
            break; // Only support one include per line now
        }

        i += 1;
    }

    if (counter > 0)
    {
        char includeFilePath[256];
        combileShaderPathAndFile(shaderPath, includeFile, includeFilePath);

        FILE* pIncludeFile = NULL;
        fopen_s(&pIncludeFile, includeFilePath, "r");

        if (pIncludeFile != NULL)
        {
            writeToShaderSource(pIncludeFile, sourceSize, writtenSize, shaderSource);
            fclose(pIncludeFile);
        }
        else
        {
            printf("Can't find include shader file!\n");
            assert(FALSE);
        }
    }
}

void ShaderCompiler::parseShaderFile(
    const char*  file,
    UINT         sourceSize,
    OUT char*    shaderSource)
{
    const char* filePath = (const char*)malloc(256 * sizeof(char));
    combileShaderPathAndFile(shaderPath, file, filePath);

    memset(shaderSource, 0, sourceSize);

    FILE* pFile = NULL;
    fopen_s(&pFile, filePath, "r");

    if (pFile != NULL)
    {
        UINT size = 0;
        writeToShaderSource(pFile, sourceSize, &size, shaderSource);        

        fclose(pFile);
    }
    else
    {
        printf("Can't find shader file!\n");
        assert(FALSE);
    }

    //Release file path array
    SafeRelease((void*)filePath, MALLOC);
}

void ShaderCompiler::parseShaderFile(
    const char*  path,
    const char*  file,
    UINT         sourceSize,
    OUT char *   shaderSource)
{
    SafeRelease((char*)shaderPath, MALLOC);
    shaderPath = (char*)malloc(512 * sizeof(char));
    memcpy(this->shaderPath, path, sizeof(path));

    parseShaderFile(file, sourceSize, shaderSource);
}

void ShaderCompiler::setDefaultShaderPath(const char* path)
{
    memset(this->shaderPath, 0, strlen(shaderPath));
    memcpy(this->shaderPath, path, strlen(path));
    strcat(this->shaderPath, "\0");
    //memcpy((char*)(this->shaderPath), path, strlen(path) + 1);
}

const char* ShaderCompiler::getDefaultPath()
{
    return shaderPath;
}

BOOL ShaderCompiler::compileShader(
    const char* const  vertexShaderPath,
    const char* const  vertexShaderFile,
    const char* const  fragmentShaderPath,
    const char* const  fragmentShaderFile,
    OUT GLuint* const  shaderProgram,
    const unsigned int vertexShaderSourceSize,
    const unsigned int fragmentShaderSourceSize)
{
    const char* vertexShaderFilePath = (const char*)malloc(256 * sizeof(char));
    combileShaderPathAndFile(vertexShaderPath, vertexShaderFile, vertexShaderFilePath);

    const char* fragmentShaderFilePath = (const char*)malloc(256 * sizeof(char));
    combileShaderPathAndFile(fragmentShaderPath, fragmentShaderFile, fragmentShaderFilePath);

    setDefaultShaderPath(vertexShaderPath);
    
    BOOL hs = compileShader(
        vertexShaderFile, fragmentShaderFile, NULL, NULL, NULL,
        shaderProgram,
        vertexShaderSourceSize, fragmentShaderSourceSize);

    return hs;
}

BOOL ShaderCompiler::compileShader(
    const char* const  vertexShaderFile,
    const char* const  fragmentShaderFile,
    const char* const  geometryShaderFile,
    const char* const  tcsShaderFile,
    const char* const  tesShaderFile,
    OUT GLuint*        shaderProgram,
    const unsigned int vertexShaderSourceSize,
    const unsigned int fragmentShaderSourceSize)
{
    BOOL result = TRUE;

    GLint success = true;// Indicator of compile result
    char compileLog[512];

    // Create shader program
    *shaderProgram = glCreateProgram();

    /// Compile vertex shader
    char* vertexShaderSource = (char*)malloc(vertexShaderSourceSize * sizeof(char));
    parseShaderFile(vertexShaderFile, vertexShaderSourceSize, vertexShaderSource);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (success)
    {
        printf("\nVertex shader is successly compiled.\n");
    }
    else
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, compileLog);
        printf("\nFail to compile vertex shader.\n%s\n", compileLog);
        result = FALSE;
    }

    glAttachShader(*shaderProgram, vertexShader);

    /// Compile fragment(pixal) shader
    char* fragmentShaderSource = (char*)malloc(vertexShaderSourceSize * sizeof(char));
    parseShaderFile(fragmentShaderFile, fragmentShaderSourceSize, fragmentShaderSource);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (success)
    {
        printf("\nFragment shader is successly compiled.\n");
    }
    else
    {
        glGetShaderInfoLog(fragShader, 512, NULL, compileLog);
        printf("\nFail to compile fragment shader.\n%s\n", compileLog);
        result = FALSE;
    }

    glAttachShader(*shaderProgram, fragShader);

    /// Compile geometry shader
    GLuint geometryShader;
    char* geometryShaderSource = NULL;
    if (geometryShaderFile != NULL)
    {
        geometryShaderSource = (char*)malloc(DEFAULT_SHADER_SIZE * sizeof(char));
        parseShaderFile(geometryShaderFile, DEFAULT_SHADER_SIZE, geometryShaderSource);

        geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);
        glCompileShader(geometryShader);

        glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);

        if (success)
        {
            printf("\nGeometry shader is successly compiled.\n");
        }
        else
        {
            glGetShaderInfoLog(fragShader, 512, NULL, compileLog);
            printf("\nFail to compile geometry shader.\n%s\n", compileLog);
            result = FALSE;
        }

        glAttachShader(*shaderProgram, geometryShader);
    }

    // TODO: Compiler tessellation Shader

    // Link shaders into program
    glLinkProgram(*shaderProgram);

    int InfoLogLength;
    glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
    glGetProgramiv(*shaderProgram, GL_INFO_LOG_LENGTH, &InfoLogLength);

    if (success)
    {
        printf("\nShader program is successly linked.\n");
    }
    else
    {
        glGetShaderInfoLog(*shaderProgram, InfoLogLength, NULL, &compileLog[0]);
        printf("\nFail to link shaders in the program.\n");

        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(*shaderProgram, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        printf("%s\n", &ProgramErrorMessage[0]);
        result = FALSE;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragShader);

    if (geometryShaderFile != NULL) { glDeleteShader(geometryShader); }

    SafeRelease(vertexShaderSource, MALLOC);
    SafeRelease(fragmentShaderSource, MALLOC);
    SafeRelease(geometryShaderSource, MALLOC);

    return result;
}

ShaderCompiler::~ShaderCompiler()
{
    SafeRelease((char*)shaderPath, MALLOC);
}
