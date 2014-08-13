//
//  MarchingSquare.cpp
//  Titan
//
//  Created by zhang yunjie on 2014/08/13.
//
//

#include "MarchingSquare.h"

USING_NS_CC;

void MarchingSquare::marchingSquares(Image *image, std::vector<Vec2> &pointVector)
{
    pointVector.clear();
    
    mpImage = image;
    mTolerance = 1;
    
    // get the starting point
    cocos2d::Vec2* startPoint = getStartingPixel();
    if (startPoint != NULL)
    {
        // px and py are the coordinates of the starting points
        float px = startPoint->x;
        float py = startPoint->y;
        
        // stepx and stepy can be -1, 0 or 1 and represent the step in pixels to reach
        // next contour point
        float stepx, stepy;
        // we also neeed to save the previous step. that's why we use prevx and prevy
        float prevx, prevy;
        // closeLoop wille be true once we traced the full contour
        bool closedLoop = false;
        
        while (!closedLoop)
        {
            // the core of the script is getting the 2x2 suqare value of each pixel
            int squareValue = getSquareValue(px, py);
            switch (squareValue) {
                    /* going UP with these cases:
                     
                     +---+---+   +---+---+   +---+---+
                     | 1 |   |   | 1 |   |   | 1 |   |
                     +---+---+   +---+---+   +---+---+
                     |   |   |   | 4 |   |   | 4 | 8 |
                     +---+---+   +---+---+   +---+---+
                     
                     */
                case 1:
                case 5:
                case 13:
                    stepx = 0;
                    stepy = -1;
                    break;
                    
                    /* going DOWN with these cases:
                     
                     +---+---+   +---+---+   +---+---+
                     |   |   |   |   | 2 |   | 1 | 2 |
                     +---+---+   +---+---+   +---+---+
                     |   | 8 |   |   | 8 |   |   | 8 |
                     +---+---+   +---+---+  +---+---+
                     
                     */
                case 8:
                case 10:
                case 11:
                    stepx = 0;
                    stepy = 1;
                    break;
                    
                    /* going LEFT with these cases:
                     
                     +---+---+   +---+---+   +---+---+
                     |   |   |   |   |   |   |   | 2 |
                     +---+---+   +---+---+   +---+---+
                     | 4 |   |   | 4 | 8 |   | 4 | 8 |
                     +---+---+   +---+---+  +---+---+
                     
                     */
                case 4:
                case 12:
                case 14:
                    stepx = -1;
                    stepy = 0;
                    break;
                    
                    /* going RIGHT with these cases:
                     
                     +---+---+   +---+---+   +---+---+
                     |   | 2 |   | 1 | 2 |   | 1 | 2 |
                     +---+---+   +---+---+   +---+---+
                     |   |   |   |   |   |   | 4 |   |
                     +---+---+   +---+---+  +---+---+
                     
                     */
                case 2:
                case 3:
                case 7:
                    stepx = 1;
                    stepy = 0;
                    break;
                    
                    /* special saddle point case 1:
                     
                     +---+---+
                     |   | 2 |
                     +---+---+
                     | 4 |   |
                     +---+---+
                     
                     going LEFT if coming from UP
                     else going RIGHT
                     
                     */
                case 6:
                    if(prevx == 0 && prevy == -1)
                    {
                        stepx = -1;
                        stepy = 0;
                    }
                    else
                    {
                        stepx = 1;
                        stepy = 0;
                    }
                    break;
                    
                    /* special saddle point case 2:
                     
                     +---+---+
                     | 1 |   |
                     +---+---+
                     |   | 8 |
                     +---+---+
                     
                     going UP if coming from RIGHT
                     else going DOWN
                     
                     */
                case 9:
                    if(prevx == 1 && prevy == 0)
                    {
                        stepx = 0;
                        stepy = -1;
                    }
                    else
                    {
                        stepx = 0;
                        stepy = 1;
                    }
                    break;
                    
                default:
                    break;
            }
            // moving onto next point
            px += stepx;
            py += stepy;
            // saving contour point
            pointVector.push_back(Vec2(px, py));
            
            prevx = stepx;
            prevy = stepy;
            // if we returned to the first point visited, the loop has finished.
            if (px == startPoint->x && py == startPoint->y)
            {
                closedLoop = true;
            }
        }
    }
}

Vec2* MarchingSquare::getStartingPixel()
{
    Vec2 *offsetPoint = new Vec2(Vec2::ZERO);
    for (int i = 0; i<mpImage->getHeight(); i++)
    {
        for (int j = 0; j<mpImage->getWidth(); j++)
        {
            offsetPoint->x = j;
            offsetPoint->y = i;

            if (getAlphaValue(mpImage, j, i) >= mTolerance)
            {
                return offsetPoint;
            }
        }
    }
    return NULL;
}

int MarchingSquare::getSquareValue(int px, int py)
{
    /*
     
     checking the 2x2 pixel grid, assigning these values to each pixel, if not transparent
     
     +---+---+
     | 1 | 2 |
     +---+---+
     | 4 | 8 | <- current pixel (pX,pY)
     +---+---+
     
     */
    
    int squareValue = 0;
    // checking upper left pixel
    if (getAlphaValue(mpImage, px-1, py-1) >= mTolerance)
    {
        squareValue += 1;
    }
    // checking upper pixel
    if (getAlphaValue(mpImage, px, py-1) > mTolerance)
    {
        squareValue += 2;
    }
    // checking left pixel
    if (getAlphaValue(mpImage, px-1, py) > mTolerance)
    {
        squareValue += 4;
    }
    // checking the pixel itself
    if (getAlphaValue(mpImage, px, py) > mTolerance)
    {
        squareValue += 8;
    }
    
    return squareValue;
}

float MarchingSquare::getAlphaValue(Image* image, int x, int y)
{
    unsigned char * data = image->getData();
    int mPixelW = image->getWidth();
    int mPixelH = image->getHeight();
    
    if (x<0 || x >= mPixelW || y<0 || y >= mPixelH)
    {
        return 0;
    }
    return data[(y * mPixelW * 4 + x * 4) + 3];
}
