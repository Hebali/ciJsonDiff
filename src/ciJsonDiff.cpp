/*             ciJsonDiff              */
/*          Patrick J. Hebron          */

#include "ciJsonDiff.h"

ciJsonDiff::ciJsonDiff() {
}

string ciJsonDiff::diff(fs::path iFrom, fs::path iTo) {
    string tFrom, tTo;
    if( readFile( iFrom.string(), &tFrom ) && readFile( iTo.string(), &tTo ) ) {
        return diff( tFrom, tTo );
    }
    return "";
}

string ciJsonDiff::diff(string iFrom, string iTo) {
    JsonTree::ParseOptions tOptions;
    tOptions.ignoreErrors( false );
    
    bool tErr = false;
    JsonTree tFrom, tTo;
    try { tFrom = JsonTree( iFrom, tOptions ); }
    catch(...) { tErr = true; console() << "Error parsing JSON: " << iFrom << endl; }
    try { tTo = JsonTree( iTo, tOptions ); }
    catch(...) { tErr = true; console() << "Error parsing JSON: " << iTo << endl; }
    
    if( !tErr ) { return diff( tFrom, tTo ).serialize(); }
    return "";
}

JsonTree ciJsonDiff::diff(JsonTree iFrom, JsonTree iTo) {
    // This method compares two root-level JSON trees:
    
    // Prepare a JSON-encoded diff tree:
    JsonTree tDiff;
    // Check that each root node is an object:
    if( iFrom.getNodeType() != JsonTree::NODE_OBJECT || iTo.getNodeType() != JsonTree::NODE_OBJECT ) { return tDiff; }
    // Compare objects:
    diffObjects( "diff", iFrom, iTo, &tDiff );
    // Return diff tree:
    return tDiff;
}

void ciJsonDiff::diffValues(const string& iName, const JsonTree& iFrom, const JsonTree& iTo, JsonTree* iParent) {
    int tFromValType = iFrom.getValueType();
    int tToValType   = iTo.getValueType();
    // Handle same value types
    if( tFromValType == tToValType ) {
        if( tFromValType == JsonTree::VALUE_BOOL ) {
            bool tFromValue = fromString<bool>( iFrom.getValue() );
            bool tToValue   = fromString<bool>( iTo.getValue() );
            if( tFromValue != tToValue ) {
                JsonTree tValDiff = JsonTree::makeArray( iName );
                JsonTree tOld = JsonTree( "_OLD_", tFromValue );
                JsonTree tNew = JsonTree( "_NEW_", tToValue );
                tValDiff.pushBack( tOld );
                tValDiff.pushBack( tNew );
                iParent->pushBack( tValDiff );
            }
        }
        else if( tFromValType == JsonTree::VALUE_DOUBLE ) {
            double tFromValue = fromString<double>( iFrom.getValue() );
            double tToValue   = fromString<double>( iTo.getValue() );
            if( tFromValue != tToValue ) {
                JsonTree tValDiff = JsonTree::makeArray( iName );
                JsonTree tOld = JsonTree( "_OLD_", tFromValue );
                JsonTree tNew = JsonTree( "_NEW_", tToValue );
                tValDiff.pushBack( tOld );
                tValDiff.pushBack( tNew );
                iParent->pushBack( tValDiff );
            }
        }
        else if( tFromValType == JsonTree::VALUE_INT ) {
            int tFromValue = fromString<int>( iFrom.getValue() );
            int tToValue   = fromString<int>( iTo.getValue() );
            if( tFromValue != tToValue ) {
                JsonTree tValDiff = JsonTree::makeArray( iName );
                JsonTree tOld = JsonTree( "_OLD_", tFromValue );
                JsonTree tNew = JsonTree( "_NEW_", tToValue );
                tValDiff.pushBack( tOld );
                tValDiff.pushBack( tNew );
                iParent->pushBack( tValDiff );
            }
        }
        else if( tFromValType == JsonTree::VALUE_UINT ) {
            unsigned int tFromValue = fromString<unsigned int>( iFrom.getValue() );
            unsigned int tToValue   = fromString<unsigned int>( iTo.getValue() );
            if( tFromValue != tToValue ) {
                JsonTree tValDiff = JsonTree::makeArray( iName );
                JsonTree tOld = JsonTree( "_OLD_", tFromValue );
                JsonTree tNew = JsonTree( "_NEW_", tToValue );
                tValDiff.pushBack( tOld );
                tValDiff.pushBack( tNew );
                iParent->pushBack( tValDiff );
            }
        }
        else if( tFromValType == JsonTree::VALUE_STRING ) {
            string tFromValue = iFrom.getValue();
            string tToValue   = iTo.getValue();
            if( tFromValue.compare( tToValue ) != 0 ) {
                JsonTree tValDiff = JsonTree::makeArray( iName );
                JsonTree tOld = JsonTree( "_OLD_", tFromValue );
                JsonTree tNew = JsonTree( "_NEW_", tToValue );
                tValDiff.pushBack( tOld );
                tValDiff.pushBack( tNew );
                iParent->pushBack( tValDiff );
            }
        }
        else {
            // UKNOWN VALUE TYPE.
        }
    }
    // Handle different value types
    else {
        JsonTree tValDiff = JsonTree::makeArray( iName );

        JsonTree tOld = JsonTree::makeArray( "_OLD_" );
        tOld.pushBack( iFrom );
        JsonTree tNew = JsonTree::makeArray( "_NEW_" );
        tNew.pushBack( iTo );
        
        tValDiff.pushBack( tOld );
        tValDiff.pushBack( tNew );
        iParent->pushBack( tValDiff );
    }
}

void ciJsonDiff::diffObjects(const string& iName, const JsonTree& iFrom, const JsonTree& iTo, JsonTree* iParent) {
    // Get node types:
    int tFromType = iFrom.getNodeType();
    int tToType   = iTo.getNodeType();
    
    // Compare like types:
    if( tFromType == tToType ) {
        bool tIsArr = (tFromType == JsonTree::NODE_ARRAY);
        bool tIsObj = (tFromType == JsonTree::NODE_OBJECT);
        
        // Array comparison:
        if( tIsArr ) {
            size_t tFromCount = iFrom.getChildCount();
            size_t tToCount   = iTo.getChildCount();
            size_t tMinCount  = min( tFromCount, tToCount );
            size_t tMaxCount  = max( tFromCount, tToCount );
            
            // Handle common indices:
            JsonTree tContainerDelta = JsonTree::makeArray( iName );
            for(size_t i = 0; i < tMinCount; i++) {
                JsonTree tItem;
                diffObjects( "**", iFrom.getChild( i ), iTo.getChild( i ), &tItem );
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
        }
        // Object comparison:
        else if( tIsObj ) {
            // Get key sets for each tree:
            set<string> tFromSet, tToSet;
            for(JsonTree::ConstIter item = iFrom.begin(); item != iFrom.end(); ++item) {
                tFromSet.insert( item->getKey() );
            }
            for(JsonTree::ConstIter item = iTo.begin(); item != iTo.end(); ++item) {
                tToSet.insert( item->getKey() );
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
                    diffObjects( (*it), iFrom.getChild( *it ), iTo.getChild( *it ), iParent );
                }
            }
            // TODO: THESE ARENT HAPPENING IN RECURSIVE LEVELS????
            // look at contents of pushBack()... kinda messy?...overriding key may not be viable...
            // Iterate FROM only items:
            if( !tFromOnly.empty() ) {
                JsonTree tContainerDeleted = JsonTree::makeArray( "_DELETED_" );
                for(set<string>::iterator it = tFromOnly.begin(); it != tFromOnly.end(); it++) {
                    tContainerDeleted.pushBack( iFrom.getChild( *it ) );
                }
                if( tContainerDeleted.hasChildren() ) { iParent->pushBack( tContainerDeleted ); }
            }
            // Iterate TO only items:
            if( !tToOnly.empty() ) {
                JsonTree tContainerAdded = JsonTree::makeArray( "_ADDED_" );
                for(set<string>::iterator it = tToOnly.begin(); it != tToOnly.end(); it++) {
                    tContainerAdded.pushBack( iTo.getChild( *it ) );
                }
                if( tContainerAdded.hasChildren() ) { iParent->pushBack( tContainerAdded ); }
            }
        }
        // Value comparison
        else if( tFromType == JsonTree::NODE_VALUE ) {
            diffValues( iFrom.getKey(), iFrom, iTo, iParent );
        }
    }
    // Otherwise, delete old and add new:
    else {
        JsonTree tObjDiff = JsonTree::makeArray( iName );
        
        JsonTree tOld = JsonTree::makeArray( "_OLD_" );
        tOld.pushBack( iFrom );
        JsonTree tNew = JsonTree::makeArray( "_NEW_" );
        tNew.pushBack( iTo );
        
        tObjDiff.pushBack( tOld );
        tObjDiff.pushBack( tNew );
        iParent->pushBack( tObjDiff );
    }
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

Json::Value ciJsonDiff::deserialize(const string & value) {
    // Parse and return data
    Json::Reader reader;
    Json::Value root;
    reader.parse(value, root);
    return root;
}

string ciJsonDiff::serialize(const Json::Value & value) {
    Json::StyledWriter writer;
    return writer.write(value);
}
