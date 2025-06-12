#include "Shader.hpp"
#include <fstream>
#include <sstream>

Shader::Shader(std::string vert, std::string frag) {
    shaderProg = loadProg(vert, frag);
}

/*
    Function to load shader program, takes the path of vert and frag shaders as parameters
*/
GLuint Shader::loadProg(std::string vert, std::string frag) {
    std::fstream vertSrc(vert);
    std::stringstream vertBuff;
    vertBuff << vertSrc.rdbuf();

    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    std::fstream fragSrc(frag);
    std::stringstream fragBuff;
    fragBuff << fragSrc.rdbuf();
    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v, NULL);
    glCompileShader(vertexShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragShader);

    return shaderProg;
}

GLuint Shader::getProg() {
    return shaderProg;
}