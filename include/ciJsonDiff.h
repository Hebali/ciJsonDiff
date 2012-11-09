/*             ciJsonDiff              */
/*          Patrick J. Hebron          */

#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <set>

#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "cinder/Json.h"
#include "json/json.h"

using namespace ci;
using namespace ci::app;
using namespace std;

enum JsonValTypes { VT_NULL, VT_ARRAY, VT_OBJECT, VT_BOOL, VT_DOUBLE, VT_UINT, VT_INT, VT_STRING };

class ciJsonDiff {
public:
    ciJsonDiff();
    
    static string       diff(fs::path iFrom, fs::path iTo);
    static string       diff(string iFrom, string iTo);
    static Json::Value  diff(Json::Value iFrom, Json::Value iTo);
    
    // TODO: There are still problems with nesting...
    // additions and deletions within children dont seem to show up!
    // where are:
    /*
     "mArrChiDiffA" : [ 0.34, 1.341, 2.34, 3.341 ],
     "mObjDiffCHIII" : {
     "mIntDiffIIII" : 2222
     },
     "mDoubleChiDiffIIII" : 2398.01
    */
    
    // SWITCH DIFF TOOLS OVER TO THIS APPROACH, better manual control...
    
    // These helper methods are based on BanTheRewind's ciJson:
    // http://www.bantherewind.com/cijson
    // Append value
    static void append(Json::Value & object, const string & key, bool value);
    static void append(Json::Value & object, const string & key, const char * value);
    static void append(Json::Value & object, const string & key, double value);
    static void append(Json::Value & object, const string & key, float value);
    static void append(Json::Value & object, const string & key, int32_t value);
    static void append(Json::Value & object, const string & key, string value);
    static void append(Json::Value & object, const string & key, uint32_t value);
    static void append(Json::Value & object, const string & key, Json::Value value);
    // Append list of values
    static void append(Json::Value & object, const string & key, vector<bool> values);
    static void append(Json::Value & object, const string & key, vector<const char *> value);
    static void append(Json::Value & object, const string & key, vector<double> values);
    static void append(Json::Value & object, const string & key, vector<float> values);
    static void append(Json::Value & object, const string & key, vector<int32_t> values);
    static void append(Json::Value & object, const string & key, vector<string> values);
    static void append(Json::Value & object, const string & key, vector<uint32_t> values);
    static void append(Json::Value & object, const string & key, vector<Json::Value> values);
    // De/serialize
    static Json::Value deserialize(const string& iValue, bool iStrict = true, bool iComments = false);
    static string serialize(const Json::Value& iValue);
    //////////
    
protected:
    static Json::Value      diffObjects(const string& iName, const Json::Value& iFrom, const Json::Value& iTo);
    static void             diffValues(const string& iName, const Json::Value& iFrom, const Json::Value& iTo, Json::Value* iParent);
    
    static int              getValueType(const Json::Value& iValue);
    
    static bool             readFile(string iFileName, string* iFileOutput);
};
