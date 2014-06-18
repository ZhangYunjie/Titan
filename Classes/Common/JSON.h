//
//  JSON.h
//  Titan
//
//  Created by zhang yunjie on 2014/06/17.
//
//

#ifndef __Titan__JSON__
#define __Titan__JSON__

#include "picojson.h"
/*
class JSON
{
public:
    typedef picojson::value  Value;
    typedef picojson::object Object;
    typedef picojson::array  Array;
    typedef std::vector<const char*> Keys;

    static void parse(Value &jvalue, const char *json);
    static bool parseFile(Value &jvalue, const char *file);

    static bool existKey(const Value &jvalue, const char *key);
    static void getKeys(const Value &jvalue, Keys &keys);

    static const Object*    getObject   (const Value &jvalue);
    static const Value*     getValue    (const Value &jvalue, const char* key);
    static const Array*     getArray    (const Value &jvalue, const char* key);

    static std::string      getString   (const Value &jvalue, const char* key = NULL);
    static double           getNumber   (const Value &jvalue, const char* key = NULL);
    static bool             getBool     (const Value &jvalue, const char* key = NULL);

    static void setObject(Object& base, const char* key, Array data);
    static void setObject(Object& base, const char* key, Object data);
    static void setObject(Object& base, const char* key, double data);
    static void setObject(Object& base, const char* key, int data);
    static void setObject(Object& base, const char* key, const char* data);
    static void setObject(Object& base, const char* key, std::string data);
    static void setObject(Object& base, const char* key, bool data);

    static std::string serialize(Object base);
};
*/
#endif /* defined(__Titan__JSON__) */
