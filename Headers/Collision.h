// COLLISION SYSTEM 
// These 4 functions return a bit cryptic numbers, so here's explanation of outputs:
// This collision system is designed for the PacMan, and he's one of the only characters that move, so I focus on PacMan and check if it collides
// In the code I use x0, y0, width0 and height0 as inputs for the PacMan, and x1, y1, width1 and height1 as the wall, or anything else that collides with the PacMan really
/* 
         1
         ↓
     ----------
     |        |
 4 → | PacMan | ← 2
     |        |
     ----------
         ↑
         3

*/
// If there's no collision, the funciton will return 0 
// This code should be improved for efficiency, for now it is what it is
// #include <endian.h> // Don't know why this was added
int RCollisionDetection(float x0, float y0, float width0, float height0, float x1, float y1, float width1, float height1) // Detects collision from the right
{
    if(x0+width0 >= x1-width1)
    {
        if(x0+width0 <= x1+width1)
        {
            if(y0+height0 >= y1-height1)
            {
                if(y0-height0 <= y1+height1)
                {
                    return 2;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
int LCollisionDetection(float x0, float y0, float width0, float height0, float x1, float y1, float width1, float height1) // Detects collision from the left
{
    if(x0-width0 <= x1+width1)
    {
        if(x0-width0 >= x1-width1)
        {
            if(y0+height0 >= y1-height1)
            {
                if(y0-height0 <= y1+height1)
                {
                    return 4;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
int TCollisionDetection(float x0, float y0, float width0, float height0, float x1, float y1, float width1, float height1) // Detects collision from the top
{
    if(y0+height0 >= y1-height1)
    {
        if(y0+height0 <= y1+height1)
        {
            if(x0-width0 <= x1+width1)
            {
                if(x0+width0 >= x1-width1)
                {
                    return 1;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
int BCollisionDetection(float x0, float y0, float width0, float height0, float x1, float y1, float width1, float height1) // Detects collision from the bottom
{
    if(y0-height0 <= y1+height1)
    {
        if(y0-height0 >= y1-height1)
        {
            if(x0-width0 <= x1+width1)
            {
                if(x0+width0 >= x1-width1)
                {
                    return 3;
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}
int MouseDetection(float x0, float y0, float x1, float y1, float width1, float height1) // returns 0 if the collision is not detected, and 1 if there is collision
{
    if(x0 >= x1-width1 && x0 <= x1+width1)
    {
        if(y0 >= y1-height1 && y0 <= y1+height1)
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return 0;
    }
}