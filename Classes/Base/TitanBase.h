//
//  TitanBase.h
//  Titan
//
//  Created by Yunjie.Zhang on 14-6-11.
//
//

#ifndef Titan_TitanBase_h
#define Titan_TitanBase_h

// namespace TitanBase {}
#ifdef __cplusplus
    #define NS_TT_BEGIN                     namespace TitanBase {
    #define NS_TT_END                       }
    #define USING_NS_TT                     using namespace TitanBase
#else
    #define NS_TT_BEGIN
    #define NS_TT_END
    #define USING_NS_TT
#endif

#define MAIN_LAYER 0

#endif
