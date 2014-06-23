//
//  b2Sperator.h
//  Titan
//
//  Created by 张 韵杰 on 14-6-23.
//
//

#ifndef __Titan__b2Sperator__
#define __Titan__b2Sperator__

#include <vector>

#include "box2d.h"
#include "cocos2d.h"

class b2Separator
{
public:
    b2Separator(){}
    
    void separator(b2Body& body, b2FixtureDef& fixtureDef, std::vector<b2Vec2> verticesVec, float scale=30.f)
    {
        std::vector<b2Vec2>* vec = new std::vector<b2Vec2>();
        for (int i = 0; i < verticesVec.size(); i++)
        {
            vec->push_back(b2Vec2(verticesVec.at(i).x*scale, verticesVec.at(i).y*scale));
        }

        std::vector<std::vector<b2Vec2> *> figsVec = calcShapes(vec);
        
        for(int i = 0; i < verticesVec.size(); i++)
        {
            vec = figsVec.at(i);
            b2Vec2* vertices = new b2Vec2[vec->size()];

            int j = 0;
            for(; j < vec->size(); j++)
            {
                vertices[j] = b2Vec2(vec->at(j).x/scale, vec->at(j).y/scale);
            }
            
            b2PolygonShape polyShape;
            polyShape.Set(vertices, vec->size());
            fixtureDef.shape = &polyShape;
            body.CreateFixture(&fixtureDef);
        }
    }
    
    int validate(std::vector<b2Vec2> &verticesVec)
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
                            ret = 1;
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
    
private:
    std::vector<std::vector<b2Vec2> *> calcShapes(std::vector<b2Vec2> *vertivesVec)
    {
        std::vector<std::vector<b2Vec2> *> figsVec;
        std::vector<std::vector<b2Vec2> *> queue;
        queue.push_back(vertivesVec);
        bool isConvex;
        
        while (!queue.empty())
        {
            std::vector<b2Vec2>* vec = queue.front();
            int n = vec->size();
            isConvex = true;

            for(int i = 0; i<n; i++)
            {
                int i1 = i;
                int i2 = i < n - 1 ? i + 1 : i + 1 - n;
                int i3 = i < n - 2 ? i + 2 : i + 2 - n;
                
                b2Vec2 p1 = vec->at(i1);
                b2Vec2 p2 = vec->at(i2);
                b2Vec2 p3 = vec->at(i3);
                
                float d = det(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y);
                if(d<0)
                {
                    isConvex = false;
                    float minLen = FLT_MAX;
                    
                    int j1;
                    int j2;
                    int h = 0, k = 0;
                    
                    b2Vec2 v1;
                    b2Vec2 v2;
                    b2Vec2 hitV;
                    
                    for(int j = 0; j<n; j++)
                    {
                        if(j!=i1&&j!=i2)
                        {
                            j1 = j;
                            j2 = j < n - 1 ? j + 1 : 0;
                            
                            v1 = vec->at(j1);
                            v2 = vec->at(j2);
                            
                            b2Vec2* v = hitRay(p1.x, p1.y, p2.x, p2.y, v1.x, v1.y, v2.x, v2.y);
                            if(NULL != v)
                            {
                                float dx = p2.x - v->x;
                                float dy = p2.y - v->y;
                                float t = dx * dx + dy * dy;
                                
                                if(t < minLen)
                                {
                                    h = j1;
                                    k = j2;
                                    hitV = *v;
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
                    v1 = vec->at(j1);
                    v2 = vec->at(j2);
                    
                    if(!pointsMatch(hitV.x, hitV.y, v2.x, v2.y)) vec1->push_back(hitV);
                    if(!pointsMatch(hitV.x, hitV.y, v1.x, v2.y)) vec2->push_back(hitV);
                    
                    h = -1;
                    k = i1;
                    while(true)
                    {
                        if(k != j2)
                            vec1->push_back(vec->at(k));
                        else
                        {
                            assert(h>=0 && h<n);
                            if(!isOnSegment(v2.x, v2.y, vec->at(h).x, vec->at(h).y, p1.x, p1.y))
                               vec1->push_back(vec->at(k));
                            break;
                        }
                        
                        h = k;
                        if(k+1>n-1) k = n-1;
                        else k--;
                    }
                    
                    std::vector<b2Vec2>* vec1_reverse;
                    vec1_reverse->clear();
                    while (vec1->size())
                    {
                        vec1_reverse->push_back(vec1->back());
                        vec1->pop_back();
                    }

                    vec1 = vec1_reverse;
                    h = -1;
                    k = i2;
                    while(true)
                    {
                        if(k!=j1)
                            vec2->push_back(vec->at(k));
                        else
                        {
                            assert(h >= 0 || h < n);
                            if(k == j1 && !isOnSegment(v1.x, v1.y, vec->at(h).x, vec->at(h).y, p2.x, p2.y))
                                vec2->push_back(vec->at(k));
                            break;
                        }
                        h = k;
                        if(k+1>n-1) k = 0;
                        else k++;
                    }
                    queue.push_back(vec1);
                    queue.push_back(vec2);
                    queue.erase(queue.begin());
                    break;
                }
            }
            if(isConvex)
            {
                figsVec.push_back(queue.front());
                queue.erase(queue.begin());
            }
        }
        return figsVec;
    }
    
    b2Vec2* hitRay(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
    {
        float t1 = x3 - x1;
        float t2 = y3 - y1;
        float t3 = x2 - x1;
        float t4 = y2 - y1;
        float t5 = x4 - x3;
        float t6 = y4 - y3;
        float t7 = t4 * t5 - t3 * t6;

        float a = ( t5 * t2 - t6 * t1 ) / t7;
        float px = x1 + a * t3, py = y1 + a * t4;
        bool b1 = isOnSegment(x2, y2, x1, y1, px, py);
        bool b2 = isOnSegment(px, py, x3, y3, x4, y4);
        
        if(b1&&b2) return new b2Vec2(px, py);
        
        return NULL;
    }
    
    b2Vec2* hitSegment(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
    {
        float t1 = x3 - x1;
        float t2 = y3 - y1;
        float t3 = x2 - x1;
        float t4 = y2 - y1;
        float t5 = x4 - x3;
        float t6 = y4 - y3;
        float t7 = t4 * t5 - t3 * t6;

        float a = ( t5 * t2 - t6 * t1 ) / t7;
        float px = x1 + a * t3, py = y1 + a * t4;
        bool b1 = isOnSegment(px, py, x1, y1, x2, y2);
        bool b2 = isOnSegment(px, py, x3, y3, x4, y4);
        
        if(b1&&b2) return new b2Vec2(px, py);
        
        return NULL;
    }
    
    bool isOnSegment(float px, float py, float x1, float y1, float x2, float y2)
    {
        bool b1 = ((x1 + 0.1 >= px && px >= x2 - 0.1) || (x1 - 0.1 <= px && px <= x2 + 0.1));
        bool b2 = ((y1 + 0.1 >= py && py >= y2 - 0.1) || (y1 - 0.1 <= py && py <= y2 + 0.1));

        return (b1&&b2&&isOnLine(px, py, x1, y1, x2, y2));
    }
    
    bool pointsMatch(float x1, float y1, float x2, float y2)
    {
        float dx = x2 >= x1 ? x2 - x1 : x1 - x2;
        float dy = y2 >= y1 ? y2 - y1 : y1 - y2;

        return (dx < 0.1 && dy < 0.1);
    }
    
    bool isOnLine(float px, float py, float x1, float y1, float x2, float y2)
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
    
    int det(float x1, float y1, float x2, float y2, float x3, float y3)
    {
        return x1 * y2 + x2 * y3 + x3 * y1 - y1 * x2 -y2 * x3 - y3 * x1;
    }
};

#endif /* defined(__Titan__b2Sperator__) */
