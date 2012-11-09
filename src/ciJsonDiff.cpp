/*             ciJsonDiff              */
/*          Patrick J. Hebron          */

#include "ciJsonDiff.h"

ciJsonDiff::ciJsonDiff() {
    // TODO REMOVE
    srand(time(NULL));
}

string ciJsonDiff::diff(fs::path iFrom, fs::path iTo) {
    string tFrom, tTo;
    if( readFile( iFrom.string(), &tFrom ) && readFile( iTo.string(), &tTo ) ) {
        return diff( tFrom, tTo );
    }
    return "";
}

string ciJsonDiff::diff(string iFrom, string iTo) {
    // Parsing defaults:
    bool tStrict   = true;
    bool tComments = false;
    
    bool tErr = false;
    Json::Value tFrom, tTo;
    try { tFrom = deserialize( iFrom, tStrict, tComments ); }
    catch(...) { tErr = true; console() << "Error parsing JSON: " << iFrom << endl; }
    try { tTo = deserialize( iTo, tStrict, tComments ); }
    catch(...) { tErr = true; console() << "Error parsing JSON: " << iTo << endl; }
    
    if( !tErr ) { return serialize( diff( tFrom, tTo ) ); }
    return "";
}

Json::Value ciJsonDiff::diff(Json::Value iFrom, Json::Value iTo) {
    // This method compares two root-level JSON trees:
    
    // Need to move up one level... iParent becomes tThis (the return value).
    
    // Prepare a JSON-encoded diff tree:
    Json::Value tDiff;
    // Check that each root node is an object:
    if( !iFrom.isObject() || !iTo.isObject() ) { return tDiff; }
    // Compare objects:
    tDiff = diffObjects( "diff", iFrom, iTo );
    // Return diff tree:
    return tDiff;
}

void ciJsonDiff::diffValues(const string& iName, const Json::Value& iFrom, const Json::Value& iTo, Json::Value* iParent) {
    int tFromValType = getValueType( iFrom );
    int tToValType   = getValueType( iTo );
    // Handle same value types
    if( tFromValType == tToValType ) {
        if( tFromValType == VT_BOOL ) {
            bool tFromValue = iFrom.asBool();
            bool tToValue   = iTo.asBool();
            if( tFromValue != tToValue ) {
                Json::Value mValData;
                ciJsonDiff::append( mValData, "_OLD_", tFromValue );
                ciJsonDiff::append( mValData, "_NEW_", tToValue );
                ciJsonDiff::append( *iParent, iName, mValData );
            }
        }
        else if( tFromValType == VT_UINT ) {
            unsigned int tFromValue = iFrom.asUInt();
            unsigned int tToValue   = iTo.asUInt();
            if( tFromValue != tToValue ) {
                Json::Value mValData;
                ciJsonDiff::append( mValData, "_OLD_", tFromValue );
                ciJsonDiff::append( mValData, "_NEW_", tToValue );
                ciJsonDiff::append( *iParent, iName, mValData );
            }
        }
        else if( tFromValType == VT_INT ) {
            int tFromValue = iFrom.asInt();
            int tToValue   = iTo.asInt();
            if( tFromValue != tToValue ) {
                Json::Value mValData;
                ciJsonDiff::append( mValData, "_OLD_", tFromValue );
                ciJsonDiff::append( mValData, "_NEW_", tToValue );
                ciJsonDiff::append( *iParent, iName, mValData );
            }
        }
        else if( tFromValType == VT_DOUBLE ) {
            double tFromValue = iFrom.asDouble();
            double tToValue   = iTo.asDouble();
            if( tFromValue != tToValue ) {
                Json::Value mValData;
                ciJsonDiff::append( mValData, "_OLD_", tFromValue );
                ciJsonDiff::append( mValData, "_NEW_", tToValue );
                ciJsonDiff::append( *iParent, iName, mValData );
            }
        }
        else if( tFromValType == VT_STRING ) {
            string tFromValue = iFrom.asString();
            string tToValue   = iTo.asString();
            if( tFromValue.compare( tToValue ) != 0 ) {
                Json::Value mValData;
                ciJsonDiff::append( *iParent, "_OLD_", tFromValue );
                ciJsonDiff::append( *iParent, "_NEW_", tToValue );
                //ciJsonDiff::append( *iParent, iName, mValData );
            }
        }
        else {
            // UKNOWN VALUE TYPE.
        }
    }
    // Handle different value types
    else {
        Json::Value mValData;
        ciJsonDiff::append( mValData, "_OLD_", iFrom );
        ciJsonDiff::append( mValData, "_NEW_", iTo );
        ciJsonDiff::append( *iParent, iName, mValData );
    }
}

Json::Value ciJsonDiff::diffObjects(const string& iName, const Json::Value& iFrom, const Json::Value& iTo) {
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
                    Json::Value tVal = diffObjects( *it, iFrom.get( *it, Json::Value::null ), iTo.get( *it, Json::Value::null ) );
                    if( !tVal.isNull() ) { append( tThis, *it, tVal ); }
                }
            }
            // Iterate FROM only items:
            if( !tFromOnly.empty() ) {
                Json::Value tDeletedValues;
                for(set<string>::iterator it = tFromOnly.begin(); it != tFromOnly.end(); it++) {
                    Json::Value tVal = iFrom.get( *it, Json::Value::null );
                    if( !tVal.isNull() ) { ciJsonDiff::append( tDeletedValues, *it, tVal ); }
                }
                if( !tDeletedValues.isNull() && !tDeletedValues.empty() ) { append( tThis, "_DELETED_", tDeletedValues ); }
            }
            // Iterate TO only items:
            if( !tToOnly.empty() ) {
                Json::Value tAddedValues;
                for(set<string>::iterator it = tToOnly.begin(); it != tToOnly.end(); it++) {
                    Json::Value tVal = iTo.get( *it, Json::Value::null );
                    if( !tVal.isNull() ) { ciJsonDiff::append( tAddedValues, *it, tVal ); }
                }
                if( !tAddedValues.isNull() && !tAddedValues.empty() ) { append( tThis, "_ADDED_", tAddedValues ); }
            }
        }
        // Value comparison:
        else if( tFromType != VT_NULL ) {
            diffValues( iName, iFrom, iTo, &tThis );
        }
    }
    // Otherwise, delete old and add new:
    else {
        Json::Value mValData;
        ciJsonDiff::append( mValData, "_OLD_", iFrom );
        ciJsonDiff::append( mValData, "_NEW_", iTo );
        ciJsonDiff::append( tThis, iName, mValData );
    }
    
    return tThis;
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

void ciJsonDiff::append(Json::Value & object, const string & key, bool value) { object[key] = Json::Value(value); }
void ciJsonDiff::append(Json::Value & object, const string & key, const char * value) { object[key] = Json::Value(value); }
void ciJsonDiff::append(Json::Value & object, const string & key, double value) { object[key] = Json::Value(value); }
void ciJsonDiff::append(Json::Value & object, const string & key, float value) { object[key] = Json::Value(value); }
void ciJsonDiff::append(Json::Value & object, const string & key, int32_t value) { object[key] = Json::Value(value); }
void ciJsonDiff::append(Json::Value & object, const string & key, string value) { object[key] = Json::Value(value.c_str()); }
void ciJsonDiff::append(Json::Value & object, const string & key, uint32_t value) { object[key] = Json::Value(value); }
void ciJsonDiff::append(Json::Value & object, const string & key, Json::Value value) { object[key] = value; }

void ciJsonDiff::append(Json::Value & object, const string & key, vector<bool> values)
{
    for (vector<bool>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
        object[key].append(* memberIt);
}
void ciJsonDiff::append(Json::Value & object, const string & key, vector<const char *> values)
{
    for (vector<const char *>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
        object[key].append(* memberIt);
}
void ciJsonDiff::append(Json::Value & object, const string & key, vector<double> values)
{
    for (vector<double>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
        object[key].append(* memberIt);
}
void ciJsonDiff::append(Json::Value & object, const string & key, vector<float> values)
{
    for (vector<float>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
        object[key].append(* memberIt);
}
void ciJsonDiff::append(Json::Value & object, const string & key, vector<int32_t> values)
{
    for (vector<int32_t>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
        object[key].append(* memberIt);
}
void ciJsonDiff::append(Json::Value & object, const string & key, vector<string> values)
{
    for (vector<string>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
        object[key].append(* memberIt);
}
void ciJsonDiff::append(Json::Value & object, const string & key, vector<uint32_t> values)
{
    for (vector<uint32_t>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
        object[key].append(* memberIt);
}
void ciJsonDiff::append(Json::Value & object, const string & key, vector<Json::Value> values)
{
    for (vector<Json::Value>::const_iterator memberIt = values.begin(); memberIt != values.end(); ++memberIt)
        object[key].append(* memberIt);
}

Json::Value ciJsonDiff::deserialize(const string& iValue, bool iStrict, bool iComments) {
    Json::Features features;
	features.allowComments_ = iComments;
	features.strictRoot_ = iStrict;
	Json::Reader reader( features );    
    Json::Value root;
    try { reader.parse( iValue, root ); }
    catch(...) { console() << "Error deserializing JSON: " << iValue << endl; }
    return root;
}

string ciJsonDiff::serialize(const Json::Value& iValue) {
    Json::StyledWriter writer;
    return writer.write( iValue );
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
