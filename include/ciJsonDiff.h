/*
 ciJsonDiff
 by Patrick J. Hebron
 
 Copyright (c) 2012, Patrick J. Hebron
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification, 
 are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of the Patrick J. Hebron nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL PATRICK J. HEBRON BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <set>

#include "cinder/app/AppBasic.h"
#include "cinder/Utilities.h"
#include "json/json.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciJsonDiff {
public:
    ciJsonDiff();
    
    static string           diff(fs::path iFrom, fs::path iTo, bool iStrict = true, bool iComments = false);
    static string           diff(string iFrom, string iTo, bool iStrict = true, bool iComments = false);
    static Json::Value      diff(Json::Value iFrom, Json::Value iTo);
    
protected:
    static Json::Value      diffObjects(const Json::Value& iFrom, const Json::Value& iTo, const string& iName = "");
    static void             diffValues(const Json::Value& iFrom, const Json::Value& iTo, Json::Value* iParent);
    
    static int              getValueType(const Json::Value& iValue);
    
    static bool             readFile(string iFileName, string* iFileOutput);
    
    enum JsonValTypes       { VT_NULL, VT_ARRAY, VT_OBJECT, VT_BOOL, VT_DOUBLE, VT_UINT, VT_INT, VT_STRING };
    
    // Value helpers:
    static void             push(Json::Value &iObject, const string &iKey, bool iValue);
    static void             push(Json::Value &iObject, const string &iKey, const char* iValue);
    static void             push(Json::Value &iObject, const string &iKey, double iValue);
    static void             push(Json::Value &iObject, const string &iKey, float iValue);
    static void             push(Json::Value &iObject, const string &iKey, int32_t iValue);
    static void             push(Json::Value &iObject, const string &iKey, string iValue);
    static void             push(Json::Value &iObject, const string &iKey, uint32_t iValue);
    static void             push(Json::Value &iObject, const string &iKey, Json::Value iValue);
    
    // Array helpers:
    static void             pushArr(Json::Value &iObject, const string &iKey, vector<bool> iValues);
    static void             pushArr(Json::Value &iObject, const string &iKey, vector<const char*> iValues);
    static void             pushArr(Json::Value &iObject, const string &iKey, vector<double> iValues);
    static void             pushArr(Json::Value &iObject, const string &iKey, vector<float> iValues);
    static void             pushArr(Json::Value &iObject, const string &iKey, vector<int32_t> iValues);
    static void             pushArr(Json::Value &iObject, const string &iKey, vector<string> iValues);
    static void             pushArr(Json::Value &iObject, const string &iKey, vector<uint32_t> iValues);
    static void             pushArr(Json::Value &iObject, const string &iKey, vector<Json::Value> iValues);
    
    // Serialization helpers:
    static Json::Value      deserialize(const string& iValue, bool *iErr, bool iStrict = true, bool iComments = false);
    static string           serialize(const Json::Value& iValue);
};
