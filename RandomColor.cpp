#include "RandomColor.h"

#include <stdlib.h> // for rand()
#include <time.h>

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
