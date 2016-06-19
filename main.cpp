// the OpenGL version include also includes all previous versions
// Build note: Due to a minefield of preprocessor build flags, the gl_load.hpp must come after 
// the version include.
// Build note: Do NOT mistakenly include _int_gl_4_4.h.  That one doesn't define OpenGL stuff 
// first.
// Build note: Also need to link opengl32.lib (unknown directory, but VS seems to know where it 
// is, so don't put in an "Additional Library Directories" entry for it).
// Build note: Also need to link glload/lib/glloadD.lib.
#include "glload/include/glload/gl_4_4.h"
#include "glload/include/glload/gl_load.hpp"

// Build note: Must be included after OpenGL code (in this case, glload).
// Build note: Also need to link freeglut/lib/freeglutD.lib.  However, the linker will try to 
// find "freeglut.lib" (note the lack of "D") instead unless the following preprocessor 
// directives are set either here or in the source-building command line (VS has a
// "Preprocessor" section under "C/C++" for preprocessor definitions).
// Build note: Also need to link winmm.lib (VS seems to know where it is, so don't put in an 
// "Additional Library Directories" entry).
#define FREEGLUT_STATIC
#define _LIB
#define FREEGLUT_LIB_PRAGMAS 0
#include "freeglut/include/GL/freeglut.h"

// this linking approach is very useful for portable, crude, barebones demo code, but it is 
// better to link through the project building properties
#pragma comment(lib, "glload/lib/glloadD.lib")
#pragma comment(lib, "opengl32.lib")            // needed for glload::LoadFunctions()
#pragma comment(lib, "freeglut/lib/freeglutD.lib")
#ifdef WIN32
#pragma comment(lib, "winmm.lib")               // Windows-specific; freeglut needs it
#endif

// for printf(...)
#include <stdio.h>

#include "OpenGlErrorHandling.h"
#include "GenerateShader.h"
#include "GeometryData.h"
#include "PrimitiveGeneration.h"
#include "Texture.h"

// for moving the shapes around in window space
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

GLuint gProgramId;
GeometryData gTriangle;
GeometryData gBox;
GeometryData gCircle;
GLuint gTextureId;
GLint gUnifMatrixTransform;
GLint gUnifTextureSampler;


void Init()
{
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);
    glDepthRange(0.0f, 1.0f);

    gProgramId = GenerateShaderProgram();
    gUnifMatrixTransform = glGetUniformLocation(gProgramId, "translateMatrixWindowSpace");
    gUnifTextureSampler = glGetUniformLocation(gProgramId, "tex");

    GenerateTriangle(&gTriangle);

    GenerateBox(&gBox);
    GenerateCircle(&gCircle);
    InitializeGeometry(gProgramId, &gTriangle);
    //InitializeGeometry(gProgramId, &gBox);
    //InitializeGeometry(gProgramId, &gCircle);

    gTextureId = CreateTexture();
}

//Called to update the display.
//You should call glutSwapBuffers after all of your rendering to display what you rendered.
//If you need continuous updates of the screen, call glutPostRedisplay() at the end of the function.
void Display()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClearDepth(1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 translateMatrix;

    glUseProgram(gProgramId);

    // put the triangle up and to the right
    // Note: Remember that this program is "barebones", so translation must be in window space 
    // ([-1,+1] on X and Y).
    translateMatrix = glm::translate(glm::mat4(), glm::vec3(+0.3f, +0.3f, 0.0f));
    glUniformMatrix4fv(gUnifMatrixTransform, 1, GL_FALSE, glm::value_ptr(translateMatrix));
    glUniform1i(gUnifTextureSampler, 0);
    glBindVertexArray(gTriangle._vaoId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, gTextureId);
    glDrawElements(gTriangle._drawStyle, gTriangle._indices.size(), GL_UNSIGNED_SHORT, 0);

    //// put the box up and to the left
    //translateMatrix = glm::translate(glm::mat4(), glm::vec3(-0.3f, +0.3f, 0.0f));
    //glUniformMatrix4fv(gUniformLocation, 1, GL_FALSE, glm::value_ptr(translateMatrix));
    //glBindVertexArray(gBox._vaoId);
    //glDrawElements(gBox._drawStyle, gBox._indices.size(), GL_UNSIGNED_SHORT, 0);

    //// put the circle down and center
    //translateMatrix = glm::translate(glm::mat4(), glm::vec3(0.0f, -0.3f, 0.0f));
    //glUniformMatrix4fv(gUniformLocation, 1, GL_FALSE, glm::value_ptr(translateMatrix));
    //glBindVertexArray(gCircle._vaoId);
    //glDrawElements(gCircle._drawStyle, gCircle._indices.size(), GL_UNSIGNED_SHORT, 0);

    glUseProgram(0);

    glutSwapBuffers();
    glutPostRedisplay();
}

//Called whenever the window is resized. The new window size is given, in pixels.
//This is an opportunity to call glViewport or glScissor to keep up with the change in size.
void Reshape(int w, int h)
{
    glViewport(0, 0, w, h);
}

//Called whenever a key on the keyboard was pressed.
//The key is given by the ''key'' parameter, which is in ASCII.
//It's often a good idea to have the escape key (ASCII value 27) call glutLeaveMainLoop() to 
//exit the program.
void Keyboard(unsigned char key, int x, int y)
{
    // this statement is mostly to get ride of an "unreferenced parameter" warning
    printf("keyboard: x = %d, y = %d\n", x, y);
    switch (key)
    {
    case 27:
    {
        // ESC key
        glutLeaveMainLoop();
        return;
    }
    default:
        break;
    }
}

// I don't know what this does, but I've kept it around since early times, and this was the
// comment given with it:
//Called before FreeGLUT is initialized. It should return the FreeGLUT
//display mode flags that you want to use. The initial value are the standard ones
//used by the framework. You can modify it or just return you own set.
//This function can also set the width/height of the window. The initial
//value of these variables is the default, but you can change it.
unsigned int Defaults(unsigned int displayMode, int &width, int &height) 
{
    // this statement is mostly to get ride of an "unreferenced parameter" warning
    printf("Defaults: width = %d, height = %d\n", width, height);
    return displayMode; 
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);

    int width = 500;
    int height = 500;
    unsigned int displayMode = GLUT_DOUBLE | GLUT_ALPHA | GLUT_DEPTH | GLUT_STENCIL;
    displayMode = Defaults(displayMode, width, height);

    glutInitDisplayMode(displayMode);
    glutInitContextVersion(4, 4);
    glutInitContextProfile(GLUT_CORE_PROFILE);

    // enable this for automatic message reporting (see OpenGlErrorHandling.cpp)
//#define DEBUG
#ifdef DEBUG
    glutInitContextFlags(GLUT_DEBUG);
#endif

    glutInitWindowSize(width, height);
    glutInitWindowPosition(300, 200);
    int window = glutCreateWindow(argv[0]);

    glload::LoadTest glLoadGood = glload::LoadFunctions();
    // ??check return value??

    glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_CONTINUE_EXECUTION);

    if (!glload::IsVersionGEQ(3, 3))
    {
        printf("Your OpenGL version is %i, %i. You must have at least OpenGL 3.3 to run this tutorial.\n",
            glload::GetMajorVersion(), glload::GetMinorVersion());
        glutDestroyWindow(window);
        return 0;
    }

    if (glext_ARB_debug_output)
    {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        glDebugMessageCallbackARB(DebugFunc, (void*)15);
    }

    Init();

    glutDisplayFunc(Display);
    glutReshapeFunc(Reshape);
    glutKeyboardFunc(Keyboard);
    glutMainLoop();

    return 0;
}