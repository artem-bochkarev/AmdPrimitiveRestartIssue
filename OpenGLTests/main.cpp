#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#include <vector>

static const char* vertex_shader_text =
"#version 330\n"
"uniform mat4 MVP;\n"
"in vec3 vPos;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"}\n";

static const char* fragment_shader_text =
"#version 330\n"
"out vec4 resultColor;\n"
"void main()\n"
"{\n"
"    resultColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
"}\n";

#define USE_SHORT_INDEXES 1

#ifndef USE_SHORT_INDEXES
using INDEX_TYPE = unsigned int;
static const INDEX_TYPE RESTART_INDEX = 0;
static const GLenum INDEX_TYPE_GL = GL_UNSIGNED_INT;
#else
using INDEX_TYPE = unsigned short;
static const INDEX_TYPE RESTART_INDEX = UINT16_MAX;
static const GLenum INDEX_TYPE_GL = GL_UNSIGNED_SHORT;
#endif

int main( void )
{
    GLFWwindow* window;

    /* Initialize the library */
    if ( !glfwInit() )
        return -1;

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow( 640, 480, "Hello World", NULL, NULL );
    if ( !window )
    {
        glfwTerminate();
        return -1;
    }

    std::vector<glm::vec3> vertexes = { glm::vec3{ -6.0, 5.0, 0.0}, glm::vec3{ 0.0, 5.0, 0.0},
                                        glm::vec3{ 6.0, 5.0, 0.0}, glm::vec3{ 6.0, -5.0, 0.0}
                                      };

    std::vector<INDEX_TYPE> indexes = { 0, 1, RESTART_INDEX, 2, 3, RESTART_INDEX };

    glfwMakeContextCurrent( window );
    gladLoadGL();
    glfwSwapInterval( 1 );

    GLuint vao;
    glGenVertexArrays( 1, &vao );
    glBindVertexArray( vao );

    GLuint vertex_shader, fragment_shader, program;

    vertex_shader = glCreateShader( GL_VERTEX_SHADER );
    glShaderSource( vertex_shader, 1, &vertex_shader_text, NULL );
    glCompileShader( vertex_shader );
    GLint success = 0;
    glGetShaderiv( vertex_shader, GL_COMPILE_STATUS, &success );

    fragment_shader = glCreateShader( GL_FRAGMENT_SHADER );
    glShaderSource( fragment_shader, 1, &fragment_shader_text, NULL );
    glCompileShader( fragment_shader );

    glGetShaderiv( fragment_shader, GL_COMPILE_STATUS, &success );

    program = glCreateProgram();
    glAttachShader( program, vertex_shader );
    glAttachShader( program, fragment_shader );
    glLinkProgram( program );

    GLint isLinked = 0;
    glGetProgramiv( program, GL_LINK_STATUS, &isLinked );

    GLint mvp_location, vpos_location;
    mvp_location = glGetUniformLocation( program, "MVP" );
    vpos_location = glGetAttribLocation( program, "vPos" );

    GLuint vertex_buffer;
    glGenBuffers( 1, &vertex_buffer );
    glBindBuffer( GL_ARRAY_BUFFER, vertex_buffer );
    glBufferData( GL_ARRAY_BUFFER, vertexes.size() * sizeof( glm::vec3 ), vertexes.data(), GL_STATIC_DRAW );

    glEnableVertexAttribArray( vpos_location );
    glVertexAttribPointer( vpos_location, 3, GL_FLOAT, GL_FALSE, 0, (void*)0 );
    glBindBuffer( GL_ARRAY_BUFFER, 0 );


    while ( !glfwWindowShouldClose( window ) )
    {

        float ratio;
        int width, height;
        glfwGetFramebufferSize( window, &width, &height );
        ratio = width / (float)height;

        constexpr double k = 0.1f;
        glm::mat4 m = glm::scale( glm::mat4{ 1.0 }, glm::vec3{ k, k, k } );
        glm::mat4 p = glm::ortho( -ratio, ratio, -1.f, 1.f, 1.f, -1.f );
        glm::mat4 mvp = p * m;

        glDisable( GL_DEPTH_TEST );

        glViewport( 0, 0, width, height );
        glClearColor(0.0f, 0.3f, 0.0f, 1.0f );
        glClear( GL_COLOR_BUFFER_BIT );

        glUseProgram( program );
        glUniformMatrix4fv( mvp_location, 1, GL_FALSE, glm::value_ptr( mvp ) );

        glEnable( GL_PRIMITIVE_RESTART_FIXED_INDEX );
        //glPrimitiveRestartIndex( RESTART_INDEX );

        //Not ok:
        //glDrawElements( GL_LINE_STRIP, 3, INDEX_TYPE_GL, (void*)( indexes.data() ) );
        //glDrawElements( GL_LINE_STRIP, 3, INDEX_TYPE_GL, (void*)(indexes.data() + 2) );
        //Ok:
        glDrawElements( GL_LINE_STRIP, 6, INDEX_TYPE_GL, (void*)(indexes.data()) );

        glDisable( GL_PRIMITIVE_RESTART );

        glDrawArrays( GL_LINES, 0, 2 );

        glfwSwapBuffers( window );

        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
