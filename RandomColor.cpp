#include "RandomColor.h"

#include <stdlib.h> // for rand()
#include <time.h>

/*-----------------------------------------------------------------------------------------------
Description:
    Generates three random floats on the range 0.0f - 1.0f and stores them in a glm::vec3 for
    use in RGB or other red-green-blue color combinations (texture format and internal format can
    be RGB, GBR, GRB, etc.).
Parameters: None
Returns:
    A glm::vec3 with three random floats stuffed inside on the range 0.0f - 1.0f;
Exception:  Safe
Creator:    John Cox (6-12-2016)
-----------------------------------------------------------------------------------------------*/
glm::vec3 RandomColor()
{
    static bool firstTime = true;
    if (firstTime)
    {
        srand((unsigned int)time(0));
        firstTime = false;
    }

    glm::vec3 ret;
    ret.x = rand() / (float)(RAND_MAX);
    ret.y = rand() / (float)(RAND_MAX);
    ret.z = rand() / (float)(RAND_MAX);
    return ret;
}
