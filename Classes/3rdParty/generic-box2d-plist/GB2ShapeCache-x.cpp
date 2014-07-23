//
//  GB2ShapeCache-x.cpp
//  
//  Loads physics sprites created with http://www.PhysicsEditor.de
//  To be used with cocos2d-x
//
//  Generic Shape Cache for box2d
//
//  Created by Thomas Broquist
//
//      http://www.PhysicsEditor.de
//      http://texturepacker.com
//      http://www.code-and-web.de
//  
//  All rights reserved.
//
//  Permission is hereby granted, free of charge, to any person obtaining a copy
//  of this software and associated documentation files (the "Software"), to deal
//  in the Software without restriction, including without limitation the rights
//  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//  copies of the Software, and to permit persons to whom the Software is
//  furnished to do so, subject to the following conditions:
//  
//  The above copyright notice and this permission notice shall be included in
//  all copies or substantial portions of the Software.
//  
//  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//  THE SOFTWARE.
//

#include "GB2ShapeCache-x.h"
#include "Box2D.h"
#include "CCNS.h"

using namespace cocos2d;

/**
 * Internal class to hold the fixtures
 */
class FixtureDef {
public:
    FixtureDef()
    : next(NULL) {}
    
    ~FixtureDef() {
        delete next;
        delete fixture.shape;
    }
    
    FixtureDef *next;
    b2FixtureDef fixture;
    int callbackData;
};

class BodyDef {
public:
	BodyDef()
	: fixtures(NULL) {}
	
	~BodyDef() {
		if (fixtures)
			delete fixtures;
	}
	
	FixtureDef *fixtures;
	Vec2 anchorPoint;
};

static GB2ShapeCache *_sharedGB2ShapeCache = NULL;

GB2ShapeCache* GB2ShapeCache::sharedGB2ShapeCache(void) {
	if (!_sharedGB2ShapeCache) {
		_sharedGB2ShapeCache = new GB2ShapeCache();
        _sharedGB2ShapeCache->init();
	}
	
	return _sharedGB2ShapeCache;
}

bool GB2ShapeCache::init() {
	return true;
}

void GB2ShapeCache::reset() {
	std::map<std::string, BodyDef *>::iterator iter;
	for (iter = shapeObjects.begin() ; iter != shapeObjects.end() ; ++iter) {
		delete iter->second;
	}
	shapeObjects.clear();
}

void GB2ShapeCache::addFixturesToBody(b2Body *body, const std::string &shape) {
	std::map<std::string, BodyDef *>::iterator pos = shapeObjects.find(shape);
	assert(pos != shapeObjects.end());
	
	BodyDef *so = (*pos).second;

	FixtureDef *fix = so->fixtures;
    while (fix) {
        body->CreateFixture(&fix->fixture);
        fix = fix->next;
    }
}

cocos2d::Vec2 GB2ShapeCache::anchorPointForShape(const std::string &shape) {
	std::map<std::string, BodyDef *>::iterator pos = shapeObjects.find(shape);
	assert(pos != shapeObjects.end());
	
	BodyDef *bd = (*pos).second;
	return bd->anchorPoint;
}


void GB2ShapeCache::addShapesWithFile(const std::string &plist) {
    
	//const char *fullName = CCFileUtils::sharedFileUtils()->fullPathForFilename(plist.c_str()).c_str();
    
    __Dictionary *dict = __Dictionary::createWithContentsOfFile(plist.c_str());
    // not triggered - cocos2dx delivers empty dict if non was found

	CCAssert(dict != NULL, "Shape-file not found");
    
    CCAssert(dict->count() != 0, "plist file empty or not existing");
	
	__Dictionary *metadataDict = (__Dictionary *)dict->objectForKey("metadata");
    
    int format = static_cast<__String *>(metadataDict->objectForKey("format"))->intValue();
    ptmRatio = static_cast<__String *>(metadataDict->objectForKey("ptm_ratio"))->floatValue();
    CCLOG("ptmRatio = %f",ptmRatio);
	CCAssert(format == 1, "Format not supported");

    
	__Dictionary *bodyDict = (__Dictionary *)dict->objectForKey("bodies");

    b2Vec2 vertices[b2_maxPolygonVertices];
    
    DictElement *dictElem;
    std::string bodyName;
	__Dictionary *bodyData;
    //iterate body list
    CCDICT_FOREACH(bodyDict,dictElem )
    {
        bodyData = (__Dictionary*)dictElem->getObject();
        bodyName = dictElem->getStrKey();
        
        
        BodyDef *bodyDef = new BodyDef();
        bodyDef->anchorPoint = PointFromString(static_cast<__String *>(bodyData->objectForKey("anchorpoint"))->getCString());
        __Array *fixtureList = (__Array*)(bodyData->objectForKey("fixtures"));
        FixtureDef **nextFixtureDef = &(bodyDef->fixtures);
        
        //iterate fixture list
        Ref *arrayElem;
        CCARRAY_FOREACH(fixtureList, arrayElem)
        {
            b2FixtureDef basicData;
            __Dictionary* fixtureData = (__Dictionary*)arrayElem;
            
            basicData.filter.categoryBits = static_cast<__String *>(fixtureData->objectForKey("filter_categoryBits"))->intValue();
            
            basicData.filter.maskBits = static_cast<__String *>(fixtureData->objectForKey("filter_maskBits"))->intValue();
            basicData.filter.groupIndex = static_cast<__String *>(fixtureData->objectForKey("filter_groupIndex"))->intValue();
            basicData.friction = static_cast<__String *>(fixtureData->objectForKey("friction"))->floatValue();
            
            basicData.density = static_cast<__String *>(fixtureData->objectForKey("density"))->floatValue();
            
            basicData.restitution = static_cast<__String *>(fixtureData->objectForKey("restitution"))->floatValue();
            
            basicData.isSensor = (bool)static_cast<__String *>(fixtureData->objectForKey("isSensor"))->intValue();
           
            __String *cb = static_cast<__String *>(fixtureData->objectForKey("userdataCbValue"));
            
            int callbackData = 0;

			if (cb)
				callbackData = cb->intValue();

			std::string fixtureType = static_cast<__String *>(fixtureData->objectForKey("fixture_type"))->getCString();

			if (fixtureType == "POLYGON") {
				__Array *polygonsArray = (__Array *)(fixtureData->objectForKey("polygons"));
				
                Ref *dicArrayElem;
                CCARRAY_FOREACH(polygonsArray, dicArrayElem)
                {
                    FixtureDef *fix = new FixtureDef();
                    fix->fixture = basicData; // copy basic data
                    fix->callbackData = callbackData;
                    
                    b2PolygonShape *polyshape = new b2PolygonShape();
                    int vindex = 0;
                    
                    __Array *polygonArray = (__Array*)dicArrayElem;
                    
                    assert(polygonArray->count() <= b2_maxPolygonVertices);
                    
                    Ref *piter;
                    CCARRAY_FOREACH(polygonArray, piter)
                    {
                        __String *verStr = (__String*)piter;
                        Vec2 offset = PointFromString(verStr->getCString());
                        vertices[vindex].x = (offset.x / ptmRatio) ;
                        vertices[vindex].y = (offset.y / ptmRatio) ;
                        vindex++;
                    }
                    
                    polyshape->Set(vertices, vindex);
                    fix->fixture.shape = polyshape;
                    
                    // create a list
                    *nextFixtureDef = fix;
                    nextFixtureDef = &(fix->next);
                }


			}
            else if (fixtureType == "CIRCLE") {
				FixtureDef *fix = new FixtureDef();
                fix->fixture = basicData; // copy basic data
                fix->callbackData = callbackData;

                __Dictionary *circleData = (__Dictionary *)fixtureData->objectForKey("circle");

                b2CircleShape *circleShape = new b2CircleShape();
				
                circleShape->m_radius = static_cast<__String *>(circleData->objectForKey("radius"))->floatValue() / ptmRatio;
				Vec2 p = PointFromString(static_cast<__String *>(circleData->objectForKey("position"))->getCString());
                circleShape->m_p = b2Vec2(p.x / ptmRatio, p.y / ptmRatio);
                fix->fixture.shape = circleShape;
				
                // create a list
                *nextFixtureDef = fix;
                nextFixtureDef = &(fix->next);

			}
            else {
				CCAssert(0, "Unknown fixtureType");
			}
		}
        // add the body element to the hash
        shapeObjects[bodyName] = bodyDef;

    }

}