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

class ciJsonDiff {
public:
    ciJsonDiff();
    
    static string   diff(fs::path iFrom, fs::path iTo);
    static string   diff(string iFrom, string iTo);
    static JsonTree diff(JsonTree iFrom, JsonTree iTo);
    
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
    
    // These helper methods were taken from BanTheRewind's original ciJson:
    // http://www.bantherewind.com/cijson
    // Append value
    static void append(Json::Value & object, const std::string & key, bool value);
    static void append(Json::Value & object, const std::string & key, const char * value);
    static void append(Json::Value & object, const std::string & key, double value);
    static void append(Json::Value & object, const std::string & key, float value);
    static void append(Json::Value & object, const std::string & key, int32_t value);
    static void append(Json::Value & object, const std::string & key, std::string value);
    static void append(Json::Value & object, const std::string & key, uint32_t value);
    // Append list of values
    static void append(Json::Value & object, const std::string & key, std::vector<bool> values);
    static void append(Json::Value & object, const std::string & key, std::vector<const char *> value);
    static void append(Json::Value & object, const std::string & key, std::vector<double> values);
    static void append(Json::Value & object, const std::string & key, std::vector<float> values);
    static void append(Json::Value & object, const std::string & key, std::vector<int32_t> values);
    static void append(Json::Value & object, const std::string & key, std::vector<std::string> values);
    static void append(Json::Value & object, const std::string & key, std::vector<uint32_t> values);
    static void append(Json::Value & object, const std::string & key, std::vector<Json::Value> values);
    // De/serialize
    static Json::Value deserialize(const std::string & value);
    static std::string serialize(const Json::Value & value);
    //////////
    
protected:
    static void     diffValues(const string& iName, const JsonTree& iFrom, const JsonTree& iTo, JsonTree* iParent);
    static void     diffObjects(const string& iName, const JsonTree& iFrom, const JsonTree& iTo, JsonTree* iParent);
    
    static bool     readFile(string iFileName, string* iFileOutput);
};
