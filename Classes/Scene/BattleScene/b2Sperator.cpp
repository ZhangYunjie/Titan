//
//  b2Sperator.cpp
//  Titan
//
//  Created by 张 韵杰 on 14-6-23.
//
//

#include "b2Sperator.h"

void b2Separator::separator(b2Body* body, b2FixtureDef* fixtureDef, std::vector<b2Vec2>* verticesVec, float scale)
{
    std::vector<b2Vec2> vec;
    std::vector<std::vector<b2Vec2>> figsVec;
    for (int i = 0; i < verticesVec->size(); i++)
    {
        vec.push_back(b2Vec2(verticesVec->at(i).x*scale, verticesVec->at(i).y*scale));
    }

    calcShapes(vec, figsVec);

    for(int i = 0; i < figsVec.size(); i++)
    {
        vec = figsVec[i];

        b2Vec2* vertices = new b2Vec2[vec.size()];

        int j = 0;
        for(; j < vec.size(); j++)
        {
            vertices[j] = b2Vec2(vec[j].x/scale, vec[j].y/scale);
        }

        // 将面片三角化
        j = 1;
        for (; j < vec.size() - 1; j++)
        {
            b2Vec2* vecTr = new b2Vec2[3];
            vecTr[0] = vertices[0];
            vecTr[1] = vertices[j];
            vecTr[2] = vertices[j+1];

            b2PolygonShape polyShape;
            polyShape.Set(vecTr, 3);

            fixtureDef->shape = &polyShape;
            body->CreateFixture(fixtureDef);

            delete[] vecTr;
        }
        delete[] vertices;
    }
}

/*
 @brief 验证

 ## 返回
 0: 正常
 1: 有交叉的连线
 2: 点没有按照顺时针的方式排列
 3: 有相交叉的连线且点未按照顺时针的方式排列
 */
int b2Separator::validate(const std::vector<b2Vec2>& verticesVec)
{
    int n = verticesVec.size(), i2, i3, ret = 0;
    bool fl2 = false;
    for (int i = 0; i<n; i++)
    {
        i2 = (i<n-1)?i+1:0;
        i3 = (i>0)?i-1:n-1;

        bool fl = false;
        for (int j = 0; j < n; j++)
        {
            if(j!=i&&j!=i2) {
                if(!fl)
                {
                    float d = det(verticesVec[i].x, verticesVec[i].y, verticesVec[i2].x, verticesVec[i2].y, verticesVec[j].x, verticesVec[j].y);
                    if (d>0) {
                        fl = true;
                    }
                }
                if (j!=i3) {
                    int j2 = (j<n-1?j+1:0);
                    b2Vec2* vec = hitSegment(verticesVec[i].x, verticesVec[i].y, verticesVec[i2].x, verticesVec[i2].y, verticesVec[j].x, verticesVec[j].y, verticesVec[j2].x, verticesVec[j2].y);
                    if(NULL != vec){
                        // ret = 1; //需要修正
                    }
                }
            }
        }

        if(!fl) fl2 = true;
    }

    if(fl2)
    {
        if(ret==1) ret = 3;
        else ret = 2;
    }

    return ret;
}

void b2Separator::calcShapes(std::vector<b2Vec2>& vertivesVec, std::vector<std::vector<b2Vec2>> &result)
{
    b2Vec2 p1, p2, p3;
    int i1, i2, i3;
    int j1, j2;
    int h = 0, k = 0;

    b2Vec2 v1, v2;
    b2Vec2 *pv, hitV(0, 0);

    std::vector<b2Vec2> vec;
    std::vector<std::vector<b2Vec2> > figsVec;
    std::queue<std::vector<b2Vec2> > queue;
    queue.push(vertivesVec);

    bool isConvex;

    while (!queue.empty())
    {
        vec = queue.front();
        int n = vec.size();
        isConvex = true;

        for(int i = 0; i<n; i++)
        {
            i1 = i;
            i2 = i < n - 1 ? i + 1 : i + 1 - n;
            i3 = i < n - 2 ? i + 2 : i + 2 - n;

            p1 = vec[i1];
            p2 = vec[i2];
            p3 = vec[i3];

            float d = det(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
            if(d<0)
            {
                isConvex = false;
                int minLen = MAX_VALUE;

                for(int j = 0; j<n; j++)
                {
                    if(j!=i1&&j!=i2)
                    {
                        j1 = j;
                        j2 = j < n - 1 ? j + 1 : 0;

                        v1 = vec[j1];
                        v2 = vec[j2];

                        pv = hitRay(p1.x, p1.y, p2.x, p2.y, v1.x, v1.y, v2.x, v2.y);
                        if(pv)
                        {
                            b2Vec2 v = *pv;
                            float dx = p2.x - v.x;
                            float dy = p2.y - v.y;
                            float t = dx * dx + dy * dy;

                            if(t < minLen)
                            {
                                h = j1;
                                k = j2;
                                hitV = v;
                                minLen = t;
                            }
                        }
                    }
                }

                assert(minLen != FLT_MAX);

                std::vector<b2Vec2>* vec1 = new std::vector<b2Vec2>();
                std::vector<b2Vec2>* vec2 = new std::vector<b2Vec2>();

                j1 = h;
                j2 = k;
                v1 = vec[j1];
                v2 = vec[j2];

                if(!pointsMatch(hitV.x, hitV.y, v2.x, v2.y)) vec1->push_back(hitV);
                if(!pointsMatch(hitV.x, hitV.y, v1.x, v2.y)) vec2->push_back(hitV);

                h = -1;
                k = i1;
                while(true)
                {
                    if(k != j2)
                        vec1->push_back(vec[k]);
                    else
                    {
                        assert(h>=0 && h<n);
                        if(!isOnSegment(v2.x, v2.y, vec[h].x, vec[h].y, p1.x, p1.y))
                            vec1->push_back(vec[k]);
                        break;
                    }

                    h = k;
                    if(k-1<0) k = n-1;
                    else k--;
                }

                reverse(vec1->begin(), vec1->end());

                h = -1;
                k = i2;
                while(true)
                {
                    if(k!=j1)
                        vec2->push_back(vec[k]);
                    else
                    {
                        assert(h >= 0 || h < n);
                        if(k == j1 && !isOnSegment(v1.x, v1.y, vec[h].x, vec[h].y, p2.x, p2.y))
                            vec2->push_back(vec[k]);
                        break;
                    }
                    h = k;
                    if(k+1>n-1) k = 0;
                    else k++;
                }
                queue.push(*vec1);
                queue.push(*vec2);
                queue.pop();
                break;
            }
        }
        if(isConvex)
        {
            figsVec.push_back(queue.front());
            queue.pop();
        }
    }
    result = figsVec;
}

b2Vec2* b2Separator::hitRay(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
    float t1 = x3 - x1;
    float t2 = y3 - y1;
    float t3 = x2 - x1;
    float t4 = y2 - y1;
    float t5 = x4 - x3;
    float t6 = y4 - y3;
    float t7 = t4 * t5 - t3 * t6;

    if(t7 == 0) return NULL;

    float a = ( t5 * t2 - t6 * t1 ) / t7;
    float px = x1 + a * t3;
    float py = y1 + a * t4;
    bool b1 = isOnSegment(x2, y2, x1, y1, px, py);
    bool b2 = isOnSegment(px, py, x3, y3, x4, y4);

    if(b1&&b2) return new b2Vec2(px, py);

    return NULL;
}

b2Vec2* b2Separator::hitSegment(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
    float t1 = x3 - x1;
    float t2 = y3 - y1;
    float t3 = x2 - x1;
    float t4 = y2 - y1;
    float t5 = x4 - x3;
    float t6 = y4 - y3;
    float t7 = t4 * t5 - t3 * t6;

    if(t7 == 0) return NULL;

    float a = ( t5 * t2 - t6 * t1 ) / t7;
    float px = x1 + a * t3, py = y1 + a * t4;
    bool b1 = isOnSegment(px, py, x1, y1, x2, y2);
    bool b2 = isOnSegment(px, py, x3, y3, x4, y4);

    if(b1&&b2) return new b2Vec2(px, py);

    return NULL;
}

bool b2Separator::isOnSegment(float px, float py, float x1, float y1, float x2, float y2)
{
    bool b1 = ((x1 + 0.1 >= px && px >= x2 - 0.1) || (x1 - 0.1 <= px && px <= x2 + 0.1));
    bool b2 = ((y1 + 0.1 >= py && py >= y2 - 0.1) || (y1 - 0.1 <= py && py <= y2 + 0.1));

    return (b1&&b2&&isOnLine(px, py, x1, y1, x2, y2));
}

bool b2Separator::pointsMatch(float x1, float y1, float x2, float y2)
{
    float dx = (x2 >= x1) ? x2 - x1 : x1 - x2;
    float dy = (y2 >= y1) ? y2 - y1 : y1 - y2;

    return (dx < 0.1 && dy < 0.1);
}

bool b2Separator::isOnLine(float px, float py, float x1, float y1, float x2, float y2)
{
    if( x2 - x1 > 0.1f || x1 - x2 < 0.1f )
    {
        float a = ( y2 - y1 ) / ( x2 - x1 );
        float possibleY = a * ( px - x1 ) + y1;
        float diff = possibleY > py ? possibleY - py: py - possibleY;

        return diff < 0.1;
    }
    return px - x1 < 0.1 || x1 - px < 0.1f ;
}

float b2Separator::det(float x1, float y1, float x2, float y2, float x3, float y3)
{
    return x1 * y2 + x2 * y3 + x3 * y1 - y1 * x2 -y2 * x3 - y3 * x1;
}
