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
    
protected:
    static void     diffValues(const string& iName, const JsonTree& iFrom, const JsonTree& iTo, JsonTree* iParent);
    static void     diffObjects(const string& iName, const JsonTree& iFrom, const JsonTree& iTo, JsonTree* iParent);
    
    static bool     readFile(string iFileName, string* iFileOutput);
};
