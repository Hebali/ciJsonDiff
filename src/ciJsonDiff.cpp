/*
 Diff
 by Patrick J. Hebron
 http://patrickhebron.com
 
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

#include "ciJsonDiff.h"

using namespace Json;

string Diff::diff(fs::path iFrom, fs::path iTo, const DiffOptions& iOptions) {
    string tFrom, tTo;
    if( readFile( iFrom.string(), &tFrom ) && readFile( iTo.string(), &tTo ) ) {
        return diff( tFrom, tTo, iOptions );
    }
    return "";
}

string Diff::diff(string iFrom, string iTo, const DiffOptions& iOptions) {
    bool tErrFrom = false;
    bool tErrTo   = false;
    Json::Value tFrom, tTo;
    tFrom = deserialize( iFrom, &tErrFrom, iOptions );
    tTo   = deserialize( iTo, &tErrTo, iOptions );
    
    if( !tErrFrom && !tErrTo ) { return serialize( diff( tFrom, tTo ) ); }
    return "";
}

Json::Value Diff::diff(Json::Value iFrom, Json::Value iTo, const DiffOptions& iOptions) {
    // This method compares two root-level JSON trees:
    
    // Prepare a JSON-encoded diff tree:
    Json::Value tDiff;
    // Check that each root node is an object:
    if( !iFrom.isObject() || !iTo.isObject() ) { return tDiff; }
    // Compare objects:
    tDiff = diffObjects( iFrom, iTo );
    // Return diff tree:
    return tDiff;
}

void Diff::diffValues(const Json::Value& iFrom, const Json::Value& iTo, Json::Value* iParent, const DiffOptions& iOptions) {
    int tFromValType = getValueType( iFrom );
    int tToValType   = getValueType( iTo );
    // Handle same value types
    if( tFromValType == tToValType ) {
        if( tFromValType == VT_BOOL ) {
            bool tFromValue = iFrom.asBool();
            bool tToValue   = iTo.asBool();
            if( tFromValue != tToValue ) {
                if( iOptions.mInclPatchBward ) { push( *iParent, "_OLD_", tFromValue ); }
                if( iOptions.mInclPatchFward ) { push( *iParent, "_NEW_", tToValue ); }
            }
        }
        else if( tFromValType == VT_UINT ) {
            unsigned int tFromValue = iFrom.asUInt();
            unsigned int tToValue   = iTo.asUInt();
            if( tFromValue != tToValue ) {
                if( iOptions.mInclPatchBward ) { push( *iParent, "_OLD_", tFromValue ); }
                if( iOptions.mInclPatchFward ) { push( *iParent, "_NEW_", tToValue ); }
            }
        }
        else if( tFromValType == VT_INT ) {
            int tFromValue = iFrom.asInt();
            int tToValue   = iTo.asInt();
            if( tFromValue != tToValue ) {
                if( iOptions.mInclPatchBward ) { push( *iParent, "_OLD_", tFromValue ); }
                if( iOptions.mInclPatchFward ) { push( *iParent, "_NEW_", tToValue ); }
            }
        }
        else if( tFromValType == VT_DOUBLE ) {
            double tFromValue = iFrom.asDouble();
            double tToValue   = iTo.asDouble();
            if( tFromValue != tToValue ) {
                if( iOptions.mInclPatchBward ) { push( *iParent, "_OLD_", tFromValue ); }
                if( iOptions.mInclPatchFward ) { push( *iParent, "_NEW_", tToValue ); }
            }
        }
        else if( tFromValType == VT_STRING ) {
            string tFromValue = iFrom.asString();
            string tToValue   = iTo.asString();
            if( tFromValue.compare( tToValue ) != 0 ) {
                if( iOptions.mInclPatchBward ) { push( *iParent, "_OLD_", tFromValue ); }
                if( iOptions.mInclPatchFward ) { push( *iParent, "_NEW_", tToValue ); }
            }
        }
    }
    // Handle different value types
    else {
        // TODO: Check this case:
        if( iOptions.mInclPatchBward ) { push( *iParent, "_OLD_", iFrom ); }
        if( iOptions.mInclPatchFward ) { push( *iParent, "_NEW_", iTo ); }
    }
}

Json::Value Diff::diffObjects(const Json::Value& iFrom, const Json::Value& iTo, const string& iName, const DiffOptions& iOptions) {
    Json::Value tThis;
    
    // TODO: integrate iOptions...
    
    // Get node types:
    int tFromType = getValueType( iFrom );
    int tToType   = getValueType( iTo );
    
    // Compare like types:
    if( tFromType == tToType ) {
        bool tIsArr = (tFromType == VT_ARRAY);
        bool tIsObj = (tFromType == VT_OBJECT);
        
        // Array comparison:
        if( tIsArr ) {
            size_t tFromCount = iFrom.size();
            size_t tToCount   = iTo.size();
            size_t tMinCount  = min( tFromCount, tToCount );
            size_t tMaxCount  = max( tFromCount, tToCount );
                        
            // Handle common indices:
            for(size_t i = 0; i < tMinCount; i++) {
                Json::Value tItem = diffObjects( iFrom.get( i, Json::Value::null ), iTo.get( i, Json::Value::null ) );
                if( !tItem.isNull() && !tItem.empty() ) {
                    stringstream ss; ss << i;
                    push( tThis, ss.str(), tItem );
                }
            }
            
            // Handle non-common indices:
            bool iFromIsLonger = ( tFromCount > tToCount );
            for(size_t i = tMinCount; i < tMaxCount; i++) {
                Json::Value tItem;
                if( iFromIsLonger ) { push( tItem, "_DELETED_", iFrom.get( i, Json::Value::null ) ); }
                else                { push( tItem, "_ADDED_", iTo.get( i, Json::Value::null ) ); }
                if( !tItem.isNull() && !tItem.empty() ) {
                    stringstream ss; ss << i;
                    push( tThis, ss.str(), tItem );
                }
            }
        }
        // Object comparison:
        else if( tIsObj ) {
            Json::Value::Members tFromMembers = iFrom.getMemberNames();
            Json::Value::Members tToMembers   = iTo.getMemberNames();
            
            // Get key sets for each tree:
            set<string> tFromSet, tToSet;
            for(Json::Value::Members::iterator item = tFromMembers.begin(); item < tFromMembers.end(); ++item) {
                tFromSet.insert( *item );
            }
            for(Json::Value::Members::iterator item = tToMembers.begin(); item < tToMembers.end(); ++item) {
                tToSet.insert( *item );
            }
            
            // Get intersections and differences:
            set<string> tIntersection;
            set_intersection( tFromSet.begin(), tFromSet.end(), tToSet.begin(), tToSet.end(), inserter( tIntersection, tIntersection.end() ) );
            set<string> tFromOnly;
            set_difference( tFromSet.begin(), tFromSet.end(), tToSet.begin(), tToSet.end(), inserter( tFromOnly, tFromOnly.end() ) );
            set<string> tToOnly;
            set_difference( tToSet.begin(), tToSet.end(), tFromSet.begin(), tFromSet.end(), inserter( tToOnly, tToOnly.end() ) );
            
            // Iterate INTERSECTION items:
            if( !tIntersection.empty() ) {
                for(set<string>::iterator it = tIntersection.begin(); it != tIntersection.end(); it++) {
                    Json::Value tVal = diffObjects( iFrom.get( *it, Json::Value::null ), iTo.get( *it, Json::Value::null ), *it );
                    if( !tVal.isNull() && !tVal.empty() ) { push( tThis, *it, tVal ); }
                }
            }
            // Iterate FROM only items:
            if( !tFromOnly.empty() ) {
                Json::Value tDeletedValues;
                for(set<string>::iterator it = tFromOnly.begin(); it != tFromOnly.end(); it++) {
                    Json::Value tVal = iFrom.get( *it, Json::Value::null );
                    if( !tVal.isNull() ) { push( tDeletedValues, *it, tVal ); }
                }
                if( !tDeletedValues.isNull() && !tDeletedValues.empty() ) { push( tThis, "_DELETED_", tDeletedValues ); }
            }
            // Iterate TO only items:
            if( !tToOnly.empty() ) {
                Json::Value tAddedValues;
                for(set<string>::iterator it = tToOnly.begin(); it != tToOnly.end(); it++) {
                    Json::Value tVal = iTo.get( *it, Json::Value::null );
                    if( !tVal.isNull() ) { push( tAddedValues, *it, tVal ); }
                }
                if( !tAddedValues.isNull() && !tAddedValues.empty() ) { push( tThis, "_ADDED_", tAddedValues ); }
            }
        }
        // Value comparison:
        else if( tFromType != VT_NULL ) {
            diffValues( iFrom, iTo, &tThis );
        }
    }
    // Otherwise, delete old and add new:
    else {
        // TODO: Check this case...
        Json::Value mValData;
        push( mValData, "_OLD_", iFrom );
        push( mValData, "_NEW_", iTo );
        push( tThis, iName, mValData );
    }
    
    return tThis;
}

int Diff::getValueType(const Json::Value& iValue) {
    if     ( iValue.isNull() )     { return VT_NULL; }
    else if( iValue.isArray() )    { return VT_ARRAY; }
    else if( iValue.isObject() )   { return VT_OBJECT; }
    else if( iValue.isBool() )     { return VT_BOOL; }
    else if( iValue.isUInt() )     { return VT_UINT; }
    else if( iValue.isInt() )      { return VT_INT; }
    else if( iValue.isDouble() )   { return VT_DOUBLE; }
    else if( iValue.isString() )   { return VT_STRING; }
    return VT_NULL;
}

bool Diff::readFile(string iFileName, string* iFileOutput) {
    string      tLine;
    ifstream    tFile( iFileName.c_str() );
    if(tFile.is_open()) {
        while(tFile.good()) {
            getline( tFile, tLine );
            iFileOutput->append( tLine );
        }
        tFile.close();
        return true;
    }
    return false;
}

void Diff::push(Json::Value &iObject, const string &iKey, bool iValue)          { iObject[iKey] = Json::Value(iValue); }
void Diff::push(Json::Value &iObject, const string &iKey, const char* iValue)   { iObject[iKey] = Json::Value(iValue); }
void Diff::push(Json::Value &iObject, const string &iKey, double iValue)        { iObject[iKey] = Json::Value(iValue); }
void Diff::push(Json::Value &iObject, const string &iKey, float iValue)         { iObject[iKey] = Json::Value(iValue); }
void Diff::push(Json::Value &iObject, const string &iKey, int32_t iValue)       { iObject[iKey] = Json::Value(iValue); }
void Diff::push(Json::Value &iObject, const string &iKey, string iValue)        { iObject[iKey] = Json::Value(iValue.c_str()); }
void Diff::push(Json::Value &iObject, const string &iKey, uint32_t iValue)      { iObject[iKey] = Json::Value(iValue); }
void Diff::push(Json::Value &iObject, const string &iKey, Json::Value iValue)   { iObject[iKey] = iValue; }

void Diff::pushArr(Json::Value &iObject, const string &iKey, vector<bool> iValues) {
    for(vector<bool>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

void Diff::pushArr(Json::Value &iObject, const string &iKey, vector<const char*> iValues) {
    for(vector<const char *>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

void Diff::pushArr(Json::Value &iObject, const string &iKey, vector<double> iValues) {
    for(vector<double>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

void Diff::pushArr(Json::Value &iObject, const string &iKey, vector<float> iValues) {
    for(vector<float>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

void Diff::pushArr(Json::Value &iObject, const string &iKey, vector<int32_t> iValues) {
    for(vector<int32_t>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

void Diff::pushArr(Json::Value &iObject, const string &iKey, vector<string> iValues) {
    for(vector<string>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

void Diff::pushArr(Json::Value &iObject, const string &iKey, vector<uint32_t> iValues) {
    for(vector<uint32_t>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

void Diff::pushArr(Json::Value &iObject, const string &iKey, vector<Json::Value> iValues) {
    for(vector<Json::Value>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

Json::Value Diff::deserialize(const string& iValue, bool *iErr, const DiffOptions& iOptions) {
    Json::Features features;
	features.allowComments_ = iOptions.mComments;
	features.strictRoot_ = iOptions.mStrict;
	Json::Reader reader( features );    
    Json::Value root;
    try { reader.parse( iValue, root ); }
    catch(...) { *iErr = true; console() << "Error deserializing JSON: " << iValue << endl; }
    return root;
}

string Diff::serialize(const Json::Value& iValue) {
    Json::StyledWriter writer;
    return writer.write( iValue );
}
