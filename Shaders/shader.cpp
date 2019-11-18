#include "innpch.h"
#include "shader.h"

//#include "GL/glew.h" - using QOpenGLFunctions instead

#include "camerasystem.h"
#include "matrix4x4.h"

#include <QFile>

Shader::Shader(const std::string shaderName, ShaderType type)
    :   mRenderingType(type)
{
    initializeOpenGLFunctions();    //must do this to get access to OpenGL functions in QOpenGLFunctions

    std::string vertexName = shaderName + ".vert";
    std::string fragmentName = shaderName + ".frag";

    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    std::string vertexWithPath = gsl::shaderFilePath + vertexName;
    std::string fragmentWithPath = gsl::shaderFilePath + fragmentName;

    // Open files and check for errors
    vShaderFile.open( vertexWithPath );
    if(!vShaderFile)
        std::cout << "ERROR SHADER FILE " << vertexWithPath << " NOT SUCCESFULLY READ" << std::endl;
    fShaderFile.open( fragmentWithPath );
    if(!fShaderFile)
        std::cout << "ERROR SHADER FILE " << vertexWithPath << " NOT SUCCESFULLY READ" << std::endl;
    std::stringstream vShaderStream, fShaderStream;
    // Read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf( );
    fShaderStream << fShaderFile.rdbuf( );
    // close file handlers
    vShaderFile.close( );
    fShaderFile.close( );
    // Convert stream into string
    vertexCode = vShaderStream.str( );
    fragmentCode = fShaderStream.str( );

    const GLchar *vShaderCode = vertexCode.c_str( );
    const GLchar *fShaderCode = fragmentCode.c_str( );


    // 2. Compile shaders
    GLuint vertex{0}, fragment{0};
    GLint success{0};
    GLchar infoLog[512]{};

    // Vertex Shader
    vertex = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertex, 1, &vShaderCode, nullptr );
    glCompileShader( vertex );
    // Print compile errors if any
    glGetShaderiv( vertex, GL_COMPILE_STATUS, &success );
    if ( !success )
    {
        glGetShaderInfoLog( vertex, 512, nullptr, infoLog );
        std::cout << "ERROR SHADER VERTEX " << shaderName << " COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Fragment Shader
    fragment = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragment, 1, &fShaderCode, nullptr );
    glCompileShader( fragment );
    // Print compile errors if any
    glGetShaderiv( fragment, GL_COMPILE_STATUS, &success );
    if ( !success )
    {
        glGetShaderInfoLog( fragment, 512, nullptr, infoLog );
        std::cout << "ERROR SHADER FRAGMENT " << shaderName << " COMPILATION_FAILED\n" << infoLog << std::endl;
    }


    // Shader Program
    this->program = glCreateProgram( );
    glAttachShader( this->program, vertex );
    glAttachShader( this->program, fragment );
    glLinkProgram( this->program );
    // Print linking errors if any
    glGetProgramiv( this->program, GL_LINK_STATUS, &success );
    if (!success)
    {
        glGetProgramInfoLog( this->program, 512, nullptr, infoLog );
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    // Delete the shaders as they're linked into our program now and no longer needed
    glDeleteShader( vertex );
    glDeleteShader( fragment );

    mMatrixUniform = glGetUniformLocation( program, "mMatrix" );
    vMatrixUniform = glGetUniformLocation( program, "vMatrix" );
    pMatrixUniform = glGetUniformLocation( program, "pMatrix" );

    updateParams(vertexWithPath);
    updateParams(fragmentWithPath);
}

Shader::Shader(const std::string vertexPath, const std::string fragmentPath, ShaderType type)
    : mRenderingType(type)
{
    initializeOpenGLFunctions();    //must do this to get access to OpenGL functions in QOpenGLFunctions

    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    std::string vertexWithPath = gsl::shaderFilePath + vertexPath;
    std::string fragmentWithPath = gsl::shaderFilePath + fragmentPath;

    // Open files and check for errors
    vShaderFile.open( vertexWithPath );
    if(!vShaderFile)
        std::cout << "ERROR SHADER FILE " << vertexWithPath << " NOT SUCCESFULLY READ" << std::endl;
    fShaderFile.open( fragmentWithPath );
    if(!fShaderFile)
        std::cout << "ERROR SHADER FILE " << fragmentWithPath << " NOT SUCCESFULLY READ" << std::endl;
    std::stringstream vShaderStream, fShaderStream;
    // Read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf( );
    fShaderStream << fShaderFile.rdbuf( );
    // close file handlers
    vShaderFile.close( );
    fShaderFile.close( );
    // Convert stream into string
    vertexCode = vShaderStream.str( );
    fragmentCode = fShaderStream.str( );

    const GLchar *vShaderCode = vertexCode.c_str( );
    const GLchar *fShaderCode = fragmentCode.c_str( );

    // 2. Compile shaders
    GLuint vertex{0}, fragment{0};
    GLint success{0};
    GLchar infoLog[512]{};

    // Vertex Shader
    vertex = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertex, 1, &vShaderCode, nullptr );
    glCompileShader( vertex );
    // Print compile errors if any
    glGetShaderiv( vertex, GL_COMPILE_STATUS, &success );
    if ( !success )
    {
        glGetShaderInfoLog( vertex, 512, nullptr, infoLog );
        std::cout << "ERROR SHADER VERTEX " << vertexPath << " COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Fragment Shader
    fragment = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragment, 1, &fShaderCode, nullptr );
    glCompileShader( fragment );
    // Print compile errors if any
    glGetShaderiv( fragment, GL_COMPILE_STATUS, &success );
    if ( !success )
    {
        glGetShaderInfoLog( fragment, 512, nullptr, infoLog );
        std::cout << "ERROR SHADER FRAGMENT " << fragmentPath << " COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shader Program
    this->program = glCreateProgram( );
    glAttachShader( this->program, vertex );
    glAttachShader( this->program, fragment );
    glLinkProgram( this->program );
    // Print linking errors if any
    glGetProgramiv( this->program, GL_LINK_STATUS, &success );
    if (!success)
    {
        glGetProgramInfoLog( this->program, 512, nullptr, infoLog );
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete the shaders as they're linked into our program now and no longer needed
    glDeleteShader( vertex );
    glDeleteShader( fragment );

    updateParams(vertexWithPath);
    updateParams(fragmentWithPath);
}

Shader::Shader(const std::string vertexPath, const std::string fragmentPath, const std::string geometryPath, ShaderType type)
    : mRenderingType(type)
{
    initializeOpenGLFunctions();    //must do this to get access to OpenGL functions in QOpenGLFunctions

    // 1. Retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    std::string vertexWithPath = gsl::shaderFilePath + vertexPath;
    std::string fragmentWithPath = gsl::shaderFilePath + fragmentPath;

    // Open files and check for errors
    vShaderFile.open( vertexWithPath );
    if(!vShaderFile)
        std::cout << "ERROR SHADER FILE " << vertexWithPath << " NOT SUCCESFULLY READ" << std::endl;
    fShaderFile.open( fragmentWithPath );
    if(!fShaderFile)
        std::cout << "ERROR SHADER FILE " << fragmentWithPath << " NOT SUCCESFULLY READ" << std::endl;
    std::stringstream vShaderStream, fShaderStream;
    // Read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf( );
    fShaderStream << fShaderFile.rdbuf( );
    // close file handlers
    vShaderFile.close( );
    fShaderFile.close( );
    // Convert stream into string
    vertexCode = vShaderStream.str( );
    fragmentCode = fShaderStream.str( );

    const GLchar *vShaderCode = vertexCode.c_str( );
    const GLchar *fShaderCode = fragmentCode.c_str( );

    //Do the same thing with the geometry shader if it exists:
    std::string geometryCode;
    const GLchar *gShaderCode = nullptr;
    std::ifstream gShaderFile;

    auto geometryWithPath = gsl::shaderFilePath + geometryPath;
    // Open files and check for errors
    gShaderFile.open( geometryWithPath );
    if(!gShaderFile)
        std::cout << "ERROR SHADER FILE " << geometryWithPath << " NOT SUCCESFULLY READ" << std::endl;

    std::stringstream gShaderStream;
    // Read file's buffer contents into streams
    gShaderStream << gShaderFile.rdbuf( );
    // close file handlers
    gShaderFile.close( );
    // Convert stream into string
    geometryCode = gShaderStream.str( );

    gShaderCode = geometryCode.c_str( );


    // 2. Compile shaders
    GLuint vertex{0}, fragment{0}, geometry{0};
    GLint success{0};
    GLchar infoLog[512]{};

    // Vertex Shader
    vertex = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertex, 1, &vShaderCode, nullptr );
    glCompileShader( vertex );
    // Print compile errors if any
    glGetShaderiv( vertex, GL_COMPILE_STATUS, &success );
    if ( !success )
    {
        glGetShaderInfoLog( vertex, 512, nullptr, infoLog );
        std::cout << "ERROR SHADER VERTEX " << vertexPath << " COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Fragment Shader
    fragment = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragment, 1, &fShaderCode, nullptr );
    glCompileShader( fragment );
    // Print compile errors if any
    glGetShaderiv( fragment, GL_COMPILE_STATUS, &success );
    if ( !success )
    {
        glGetShaderInfoLog( fragment, 512, nullptr, infoLog );
        std::cout << "ERROR SHADER FRAGMENT " << fragmentPath << " COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // Geometry Shader
    geometry = glCreateShader( GL_GEOMETRY_SHADER );
    glShaderSource( geometry, 1, &gShaderCode, nullptr );
    glCompileShader( geometry );
    // Print compile errors if any
    glGetShaderiv( geometry, GL_COMPILE_STATUS, &success );
    if ( !success )
    {
        glGetShaderInfoLog( geometry, 512, nullptr, infoLog );
        std::cout << "ERROR SHADER GEOMETRY " << geometryPath << " COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shader Program
    this->program = glCreateProgram( );
    glAttachShader( this->program, vertex );
    glAttachShader( this->program, fragment );
    glAttachShader( this->program, geometry );
    glLinkProgram( this->program );
    // Print linking errors if any
    glGetProgramiv( this->program, GL_LINK_STATUS, &success );
    if (!success)
    {
        glGetProgramInfoLog( this->program, 512, nullptr, infoLog );
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }

    // Delete the shaders as they're linked into our program now and no longer needed
    glDeleteShader( vertex );
    glDeleteShader( fragment );
    glDeleteShader(geometry);

    updateParams(vertexWithPath);
    updateParams(fragmentWithPath);
    updateParams(geometryWithPath);
}

Shader::~Shader()
{
    glDeleteProgram(program);
}

void Shader::use()
{
    glUseProgram( this->program );
}

GLuint Shader::getProgram() const
{
    return program;
}

void Shader::updateParams(const std::string& path)
{
    QFile file(QString::fromStdString(path));
    if(!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open file for parameter generation. (File:" << path.c_str() << ")";
        return;
    }

    QTextStream stream(&file);
    while(!stream.atEnd())
    {
        auto line = stream.readLine();
        if(line.size() && line.startsWith('u'))
        {
            auto split = line.split(" ");
            if(split[0] == "uniform")
            {
                auto name = split[2];
                if(name.startsWith("p_"))
                {
                    name = name.replace(';', "");

                    QString value = "";
                    if(line.contains('='))
                    {
                        auto right = line.right(line.size() - line.indexOf('=') - 1);
                        right = right.simplified();
                        right = right.replace(" ", "");
                        right = right.replace(';', "");
                        value = right;
                    }
                    auto type = split[1];
                    addParam(name.toStdString(), type.toStdString(), value.toStdString());
                }
            }
        }
    }
}

void Shader::addParam(const std::string& name, const std::string& type, const std::string& value)
{
    if(!name.size() || !type.size())
    {
        return;
    }

    if(type == "int")
    {
        params[name] = value.size() ? std::stoi(value) : 0;
    }
    else if(type == "bool")
    {
        params[name] = value.size() ? value == "true" ? true : false : false;
    }
    else if(type == "float")
    {
        params[name] = value.size() ? std::stof(value) : 0.f;
    }
    else if(type == "vec2")
    {
        if(value.size())
        {
            QString string = value.c_str();
            auto right = string.right(string.size() - string.indexOf('(') - 1);
            right = right.replace(')', "");
            auto split = right.split(',');

            params[name] = gsl::vec2{
                            static_cast<float>(split[0].toDouble()),
                            static_cast<float>(split[1].toDouble())};
        }
        else
        {
            params[name] = gsl::vec2{};
        }
    }
    else if(type == "vec3")
    {
        if(value.size())
        {
            QString string = value.c_str();
            auto right = string.right(string.size() - string.indexOf('(') - 1);
            right = right.replace(')', "");
            auto split = right.split(',');

            params[name] = gsl::vec3{
                            static_cast<float>(split[0].toDouble()),
                            static_cast<float>(split[1].toDouble()),
                            static_cast<float>(split[2].toDouble())};
        }
        else
        {
            params[name] = gsl::vec3{};
        }
    }
    else if(type == "vec4")
    {
        if(value.size())
        {
            QString string = value.c_str();
            auto right = string.right(string.size() - string.indexOf('(') - 1);
            right = right.replace(')', "");
            auto split = right.split(',');

            params[name] = gsl::vec4{
                            static_cast<float>(split[0].toDouble()),
                            static_cast<float>(split[1].toDouble()),
                            static_cast<float>(split[2].toDouble()),
                            static_cast<float>(split[3].toDouble())
                        };
        }
        else
        {
            params[name] = gsl::vec4{};
        }
    }
}

