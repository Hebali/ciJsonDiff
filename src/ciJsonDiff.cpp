/*             ciJsonDiff              */
/*          Patrick J. Hebron          */

#include "ciJsonDiff.h"

ciJsonDiff::ciJsonDiff() {
}

string ciJsonDiff::diff(fs::path iFrom, fs::path iTo, bool iStrict, bool iComments) {
    string tFrom, tTo;
    if( readFile( iFrom.string(), &tFrom ) && readFile( iTo.string(), &tTo ) ) {
        return diff( tFrom, tTo, iStrict, iComments );
    }
    return "";
}

string ciJsonDiff::diff(string iFrom, string iTo, bool iStrict, bool iComments) {
    bool tErrFrom = false;
    bool tErrTo   = false;
    Json::Value tFrom, tTo;
    tFrom = deserialize( iFrom, &tErrFrom, iStrict, iComments );
    tTo   = deserialize( iTo, &tErrTo, iStrict, iComments );
    
    if( !tErrFrom && !tErrTo ) { return serialize( diff( tFrom, tTo ) ); }
    return "";
}

Json::Value ciJsonDiff::diff(Json::Value iFrom, Json::Value iTo) {
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

void ciJsonDiff::diffValues(const Json::Value& iFrom, const Json::Value& iTo, Json::Value* iParent) {
    int tFromValType = getValueType( iFrom );
    int tToValType   = getValueType( iTo );
    // Handle same value types
    if( tFromValType == tToValType ) {
        if( tFromValType == VT_BOOL ) {
            bool tFromValue = iFrom.asBool();
            bool tToValue   = iTo.asBool();
            if( tFromValue != tToValue ) {
                push( *iParent, "_OLD_", tFromValue );
                push( *iParent, "_NEW_", tToValue );
            }
        }
        else if( tFromValType == VT_UINT ) {
            unsigned int tFromValue = iFrom.asUInt();
            unsigned int tToValue   = iTo.asUInt();
            if( tFromValue != tToValue ) {
                push( *iParent, "_OLD_", tFromValue );
                push( *iParent, "_NEW_", tToValue );
            }
        }
        else if( tFromValType == VT_INT ) {
            int tFromValue = iFrom.asInt();
            int tToValue   = iTo.asInt();
            if( tFromValue != tToValue ) {
                push( *iParent, "_OLD_", tFromValue );
                push( *iParent, "_NEW_", tToValue );
            }
        }
        else if( tFromValType == VT_DOUBLE ) {
            double tFromValue = iFrom.asDouble();
            double tToValue   = iTo.asDouble();
            if( tFromValue != tToValue ) {
                push( *iParent, "_OLD_", tFromValue );
                push( *iParent, "_NEW_", tToValue );
            }
        }
        else if( tFromValType == VT_STRING ) {
            string tFromValue = iFrom.asString();
            string tToValue   = iTo.asString();
            if( tFromValue.compare( tToValue ) != 0 ) {
                push( *iParent, "_OLD_", tFromValue );
                push( *iParent, "_NEW_", tToValue );
            }
        }
    }
    // Handle different value types
    else {
        push( *iParent, "_OLD_", iFrom );
        push( *iParent, "_NEW_", iTo );
    }
}

Json::Value ciJsonDiff::diffObjects(const Json::Value& iFrom, const Json::Value& iTo, const string& iName) {
    Json::Value tThis;
    
    // Get node types:
    int tFromType = getValueType( iFrom );
    int tToType   = getValueType( iTo );
    
    // Compare like types:
    if( tFromType == tToType ) {
        bool tIsArr = (tFromType == VT_ARRAY);
        bool tIsObj = (tFromType == VT_OBJECT);
        
        // Array comparison:
        if( tIsArr ) {
            /* TODO:
            size_t tFromCount = iFrom.size();
            size_t tToCount   = iTo.size();
            size_t tMinCount  = min( tFromCount, tToCount );
            size_t tMaxCount  = max( tFromCount, tToCount );
            
            // Handle common indices:
            JsonTree tContainerDelta = JsonTree::makeArray( iName );
            for(size_t i = 0; i < tMinCount; i++) {
                JsonTree tItem;
                diffObjects( "", iFrom.getChild( i ), iTo.getChild( i ), &tItem );
                if( tItem.hasChildren() ) {
                    stringstream ss; ss << i;
                    //tItem.getChild(0).setKey( ss.str() );
                    if( tItem.getChild(0).hasChildren() ) { tContainerDelta.pushBack( tItem.getChild(0) ); }
                }
            }
            
            // Handle non-common indices:
            bool iFromIsLonger = ( tFromCount > tToCount );
            for(size_t i = tMinCount; i < tMaxCount; i++) {
                stringstream ss; ss << i;
                JsonTree tArrItem  = JsonTree::makeArray( ss.str() );
                JsonTree tOldOrNew = ( iFromIsLonger ? iFrom.getChild( i ) : iTo.getChild( i ) );
                //tOldOrNew.setKey( iFromIsLonger ? "_DELETED_" : "_ADDED_" );
                tArrItem.pushBack( tOldOrNew );
                if( tArrItem.hasChildren() ) { tContainerDelta.pushBack( tArrItem ); }
            }
            
            // Add container
            if( tContainerDelta.hasChildren() ) { iParent->pushBack( tContainerDelta ); }
            */
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
                    if( !tVal.isNull() ) { push( tThis, *it, tVal ); }
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
        Json::Value mValData;
        push( mValData, "_OLD_", iFrom );
        push( mValData, "_NEW_", iTo );
        push( tThis, iName, mValData );
    }
    
    return tThis;
}

int ciJsonDiff::getValueType(const Json::Value& iValue) {
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

bool ciJsonDiff::readFile(string iFileName, string* iFileOutput) {
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

void ciJsonDiff::push(Json::Value &iObject, const string &iKey, bool iValue)          { iObject[iKey] = Json::Value(iValue); }
void ciJsonDiff::push(Json::Value &iObject, const string &iKey, const char* iValue)   { iObject[iKey] = Json::Value(iValue); }
void ciJsonDiff::push(Json::Value &iObject, const string &iKey, double iValue)        { iObject[iKey] = Json::Value(iValue); }
void ciJsonDiff::push(Json::Value &iObject, const string &iKey, float iValue)         { iObject[iKey] = Json::Value(iValue); }
void ciJsonDiff::push(Json::Value &iObject, const string &iKey, int32_t iValue)       { iObject[iKey] = Json::Value(iValue); }
void ciJsonDiff::push(Json::Value &iObject, const string &iKey, string iValue)        { iObject[iKey] = Json::Value(iValue.c_str()); }
void ciJsonDiff::push(Json::Value &iObject, const string &iKey, uint32_t iValue)      { iObject[iKey] = Json::Value(iValue); }
void ciJsonDiff::push(Json::Value &iObject, const string &iKey, Json::Value iValue)   { iObject[iKey] = iValue; }

void ciJsonDiff::pushArr(Json::Value &iObject, const string &iKey, vector<bool> iValues) {
    for(vector<bool>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

void ciJsonDiff::pushArr(Json::Value &iObject, const string &iKey, vector<const char*> iValues) {
    for(vector<const char *>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

void ciJsonDiff::pushArr(Json::Value &iObject, const string &iKey, vector<double> iValues) {
    for(vector<double>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

void ciJsonDiff::pushArr(Json::Value &iObject, const string &iKey, vector<float> iValues) {
    for(vector<float>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

void ciJsonDiff::pushArr(Json::Value &iObject, const string &iKey, vector<int32_t> iValues) {
    for(vector<int32_t>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

void ciJsonDiff::pushArr(Json::Value &iObject, const string &iKey, vector<string> iValues) {
    for(vector<string>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

void ciJsonDiff::pushArr(Json::Value &iObject, const string &iKey, vector<uint32_t> iValues) {
    for(vector<uint32_t>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

void ciJsonDiff::pushArr(Json::Value &iObject, const string &iKey, vector<Json::Value> iValues) {
    for(vector<Json::Value>::const_iterator it = iValues.begin(); it != iValues.end(); ++it) { iObject[iKey].append(*it); }
}

Json::Value ciJsonDiff::deserialize(const string& iValue, bool *iErr, bool iStrict, bool iComments) {
    Json::Features features;
	features.allowComments_ = iComments;
	features.strictRoot_ = iStrict;
	Json::Reader reader( features );    
    Json::Value root;
    try { reader.parse( iValue, root ); }
    catch(...) { *iErr = true; console() << "Error deserializing JSON: " << iValue << endl; }
    return root;
}

string ciJsonDiff::serialize(const Json::Value& iValue) {
    Json::StyledWriter writer;
    return writer.write( iValue );
}
