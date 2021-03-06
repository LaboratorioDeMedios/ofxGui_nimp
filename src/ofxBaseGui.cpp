#include "ofxBaseGui.h"
#include "ofXml.h"
#include "ofImage.h"
#include "ofBitmapFont.h"
#include "enumerations.h"


void ofxGuiSetFont(const string & fontPath,int fontsize, bool _bAntiAliased=true, bool _bFullCharacterSet=false, int dpi=0){
    ofxBaseGui::loadFont(fontPath,fontsize,_bAntiAliased,_bFullCharacterSet,dpi);
}

void ofxGuiSetBitmapFont(){
    ofxBaseGui::setUseTTF(false);
}

void ofxGuiSetHeaderColor(const ofColor & color){
    ofxBaseGui::setDefaultHeaderBackgroundColor(color);
}

void ofxGuiSetBackgroundColor(const ofColor & color){
    ofxBaseGui::setDefaultBackgroundColor(color);
}

void ofxGuiSetBorderColor(const ofColor & color){
    ofxBaseGui::setDefaultBorderColor(color);
}

void ofxGuiSetTextColor(const ofColor & color){
    ofxBaseGui::setDefaultTextColor(color);
}

void ofxGuiSetFillColor(const ofColor & color){
    ofxBaseGui::setDefaultFillColor(color);
}

void ofxGuiSetClickedColor(const ofColor & color){
    ofxBaseGui::setDefaultClickedColor(color);
}

void ofxGuiSetTextPadding(int padding){
    ofxBaseGui::setDefaultTextPadding(padding);
}

void ofxGuiSetDefaultWidth(int width){
    ofxBaseGui::setDefaultWidth(width);
    
}

void ofxGuiSetDefaultHeight(int height){
    ofxBaseGui::setDefaultHeight(height);
}

ofColor
ofxBaseGui::headerBackgroundColor(64),
ofxBaseGui::backgroundColor(0),
ofxBaseGui::borderColor(120,100),
ofxBaseGui::textColor(255),
ofxBaseGui::fillColor(128),
ofxBaseGui::clickedColor(150,150,250),
ofxBaseGui::midiLearnColor(176,217,24),
ofxBaseGui::leftAudioInColor(255,224,65),
ofxBaseGui::rightAudioInColor(255,150,65),
ofxBaseGui::oscColor(104,196,196);

int ofxBaseGui::textPadding = 4;
int ofxBaseGui::defaultWidth = 200;
int ofxBaseGui::defaultHeight = 18;

ofTrueTypeFont ofxBaseGui::font;
bool ofxBaseGui::fontLoaded = false;
bool ofxBaseGui::useTTF = false;

ofxBaseGui::ofxBaseGui(){
    currentFrame = ofGetFrameNum();
    serializer = ofPtr<ofBaseFileSerializer> (new ofXml);
    
    thisHeaderBackgroundColor=headerBackgroundColor;
    thisBackgroundColor=backgroundColor;
    thisBorderColor=borderColor;
    thisTextColor=textColor;
    thisFillColor=fillColor;
    thisClickedColor=clickedColor;
    thisMidiLearnColor=midiLearnColor;
    thisLeftAudioInColor=leftAudioInColor;
    thisRightAudioInColor=rightAudioInColor;
    thisOSCColor=oscColor;
    
    bRegisteredForMouseEvents = false;
    bRegisteredForKeyEvents   = false;
    
    /*if(!fontLoaded){
     loadFont(OF_TTF_MONO,10,true,true);
     useTTF=false;
     }*/
    
    // custom variables
    //
    draggable               = true;
    clicked                 = false;
    selectedForLeftAudio    = false;
    selectedForRightAudio   = false;
    selectedForNodeId       = 0;
    selectedForOSC          = false;
    midiLearnActive         = false;
    selectedForMIDI         = false;
    selectedForMidiControl  = -1;
    midiControlActive       = -1;
    editLeftAudioInActive   = false;
    editRightAudioInActive  = false;
    editNodeId              = -1;
    editOSCActive           = false;
    commandPressed          = false;
}

void ofxBaseGui::loadFont(string filename, int fontsize, bool _bAntiAliased, bool _bFullCharacterSet, int dpi){
    font.loadFont(filename,fontsize,_bAntiAliased,_bFullCharacterSet,dpi);
    fontLoaded = true;
    useTTF = true;
}

void ofxBaseGui::setUseTTF(bool bUseTTF){
    if(bUseTTF && !fontLoaded){
        loadFont(OF_TTF_MONO,10,true,true);
    }
    useTTF = bUseTTF;
}

ofxBaseGui::~ofxBaseGui(){
    unregisterMouseEvents();
    unregisterKeyEvents();
}

void ofxBaseGui::registerMouseEvents(){
    if(bRegisteredForMouseEvents == true) {
        return; // already registered.
    }
    bRegisteredForMouseEvents = true;
    ofRegisterMouseEvents(this, AFTER_MENU_BAR_EVENT_PRIORITY);
}

void ofxBaseGui::registerKeyEvents(){
    if(bRegisteredForKeyEvents == true) {
        return; // already registered.
    }
    bRegisteredForKeyEvents = true;
    ofRegisterKeyEvents(this, OF_EVENT_ORDER_BEFORE_APP);
}

void ofxBaseGui::unregisterMouseEvents(){
    if(bRegisteredForMouseEvents == false) {
        return; // not registered.
    }
    ofUnregisterMouseEvents(this, AFTER_MENU_BAR_EVENT_PRIORITY);
    bRegisteredForMouseEvents = false;
}

void ofxBaseGui::unregisterKeyEvents(){
    if(bRegisteredForKeyEvents == false) {
        return; // not registered.
    }
    ofUnregisterKeyEvents(this, OF_EVENT_ORDER_BEFORE_APP);
    bRegisteredForKeyEvents = false;
}

void ofxBaseGui::draw(){
    currentFrame = ofGetFrameNum();
    render();
}

bool ofxBaseGui::isGuiDrawing(){
    if( ofGetFrameNum() - currentFrame > 1 ){
        return false;
    }else{
        return true;
    }
}

void ofxBaseGui::bindFontTexture(){
    if(useTTF){
        font.getFontTexture().bind();
    }else{
        ofBitmapStringGetTextureRef().bind();
    }
}

void ofxBaseGui::unbindFontTexture(){
    if(useTTF){
        font.getFontTexture().unbind();
    }else{
        ofBitmapStringGetTextureRef().unbind();
    }
}


ofMesh & ofxBaseGui::getTextMesh(const string & text, float x, float y){
    if(useTTF){
        return font.getStringMesh(text,x,y);
    }else{
        return ofBitmapStringGetMesh(text,x,y);
    }
}

ofRectangle ofxBaseGui::getTextBoundingBox(const string & text,float x, float y){
    if(useTTF){
        return font.getStringBoundingBox(text,x,y);
    }else{
        return ofBitmapStringGetBoundingBox(text,x,y);
    }
}

void ofxBaseGui::saveToFile(string filename) {
    serializer->load(filename);
    saveTo(*serializer);
    serializer->save(filename);
}

void ofxBaseGui::loadFromFile(string filename) {
    serializer->load(filename);
    loadFrom(*serializer);
}


void ofxBaseGui::saveTo(ofBaseSerializer& serializer){
    serializer.serialize(getParameter());
}

void ofxBaseGui::loadFrom(ofBaseSerializer& serializer){
    serializer.deserialize(getParameter());
}


void ofxBaseGui::setDefaultSerializer(ofPtr<ofBaseFileSerializer>  _serializer){
    serializer = _serializer;
}

string ofxBaseGui::getName(){
    return getParameter().getName();
}

void ofxBaseGui::setName(string _name){
    getParameter().setName(_name);
}

void ofxBaseGui::setPosition(ofPoint p){
    setPosition(p.x,p.y);
}

void ofxBaseGui::setPosition(float x, float y){
    b.x = x;
    b.y = y;
    generateDraw();
}

void ofxBaseGui::setSize(float w, float h){
    b.width = w;
    b.height = h;
    generateDraw();
}

void ofxBaseGui::setShape(ofRectangle r){
    b = r;
    generateDraw();
}

void ofxBaseGui::setShape(float x, float y, float w, float h){
    b.set(x,y,w,h);
    generateDraw();
}

ofPoint ofxBaseGui::getPosition(){
    return ofPoint(b.x,b.y);
}

ofRectangle ofxBaseGui::getShape(){
    return b;
}

float ofxBaseGui::getWidth(){
    return b.width;
}

float ofxBaseGui::getHeight(){
    return b.height;
}

ofColor ofxBaseGui::getHeaderBackgroundColor(){
    return thisHeaderBackgroundColor;
}

ofColor ofxBaseGui::getBackgroundColor(){
    return thisBackgroundColor;
}

ofColor ofxBaseGui::getBorderColor(){
    return thisBorderColor;
}

ofColor ofxBaseGui::getTextColor(){
    return thisTextColor;
}

ofColor ofxBaseGui::getFillColor(){
    return thisFillColor;
}

void ofxBaseGui::setHeaderBackgroundColor(const ofColor & color){
    generateDraw();
    thisHeaderBackgroundColor = color;
}

void ofxBaseGui::setBackgroundColor(const ofColor & color){
    generateDraw();
    thisBackgroundColor = color;
}

void ofxBaseGui::setBorderColor(const ofColor & color){
    generateDraw();
    thisBorderColor = color;
}

void ofxBaseGui::setTextColor(const ofColor & color){
    generateDraw();
    thisTextColor = color;
}

void ofxBaseGui::setFillColor(const ofColor & color){
    generateDraw();
    thisFillColor = color;
}

void ofxBaseGui::setClickedColor(const ofColor & color){
    generateDraw();
    thisClickedColor = color;
}

void ofxBaseGui::setDefaultHeaderBackgroundColor(const ofColor & color){
    headerBackgroundColor = color;
}

void ofxBaseGui::setDefaultBackgroundColor(const ofColor & color){
    backgroundColor = color;
}

void ofxBaseGui::setDefaultBorderColor(const ofColor & color){
    borderColor = color;
}

void ofxBaseGui::setDefaultTextColor(const ofColor & color){
    textColor = color;
}

void ofxBaseGui::setDefaultFillColor(const ofColor & color){
    fillColor = color;
}

void ofxBaseGui::setDefaultClickedColor(const ofColor & color){
    clickedColor = color;
}

void ofxBaseGui::setDefaultTextPadding(int padding){
    textPadding = padding;
}

void ofxBaseGui::setDefaultWidth(int width){
    defaultWidth = width;
}

void ofxBaseGui::setDefaultHeight(int height){
    defaultHeight = height;
}

string ofxBaseGui::saveStencilToHex(ofImage& img) {
    stringstream strm;
    int width = img.getWidth();
    int height = img.getHeight();
    int n = width * height;
    unsigned char cur = 0;
    int shift = 0;
    strm << "{";
    for(int i = 0; i < n;) {
        if(img.getPixels()[i * 4 + 3] > 0) {
            cur |= 1 << shift;
        }
        i++;
        if(i % 8 == 0) {
            strm << "0x" << hex << (unsigned int) cur;
            cur = 0;
            shift = 0;
            if(i < n) {
                strm << ",";
            }
        } else {
            shift++;
        }
    }
    strm << "}";
    return strm.str();
}

void ofxBaseGui::loadStencilFromHex(ofImage& img, unsigned char* data) {
    int width = img.getWidth();
    int height = img.getHeight();
    int i = 0;
    ofColor on(255, 255);
    ofColor off(255, 0);
    for(int y = 0; y < height; y++) {
        for(int x = 0; x < width; x++) {
            int shift = i % 8;
            int offset = i / 8;
            bool cur = (data[offset] >> shift) & 1;
            img.setColor(x, y, cur ? on : off);
            i++;
        }
    }
    img.update();
}

void ofxBaseGui::setMidiLearnActive(bool active_){
    
    midiLearnActive         = active_;
    
    editLeftAudioInActive   = false;
    editRightAudioInActive  = false;
    editOSCActive           = false;
    editNodeId              = -1;
}

void ofxBaseGui::setEditLeftAudioInActive(bool active_, int node_) {
    
    editLeftAudioInActive   = active_;
    editNodeId              = active_ ? node_ : -1;
    
    midiLearnActive         = false;
    editOSCActive           = false;
    
}

void ofxBaseGui::setEditRightAudioInActive(bool active_, int node_) {
    
    editRightAudioInActive  = active_;
    editNodeId              = active_ ? node_ : -1;
    
    midiLearnActive         = false;
    editOSCActive           = false;
}

void ofxBaseGui::setEditOSCActive(bool active_, int node_) {
    
    editOSCActive           = active_;
    editNodeId              = active_ ? node_ : -1;
    
    editLeftAudioInActive   = false;
    editRightAudioInActive  = false;
    midiLearnActive         = false;
}

void ofxBaseGui::setSelectedForLeftAudio(bool active_, int node_, string name_) {
    
    if (getParameter().getName() == name_ || (name_.compare(0, 5, "Blend")==0 && getParameter().getName().compare(0, 5, "Blend")==0)) {
        selectedForLeftAudio   = active_;
        selectedForNodeId      = node_;
    }
}

void ofxBaseGui::setSelectedForRightAudio(bool active_, int node_, string name_) {
    
    if (getParameter().getName() == name_ || (name_.compare(0, 5, "Blend")==0 && getParameter().getName().compare(0, 5, "Blend")==0)) {
        selectedForRightAudio  = active_;
        selectedForNodeId      = node_;
    }
}

void ofxBaseGui::setSelectedForOSC(bool active_, int node_, string name_) {
    
    if (getParameter().getName() == name_ || (name_.compare(0, 5, "Blend")==0 && getParameter().getName().compare(0, 5, "Blend")==0)) {
        selectedForOSC      = active_;
        selectedForNodeId   = node_;
    }
}

void ofxBaseGui::setSelectedForMIDI(bool active_, int control_, string name_) {
    
    if (getParameter().getName() == name_ || (name_.compare(0, 5, "Blend")==0 && getParameter().getName().compare(0, 5, "Blend")==0)) {
        selectedForMIDI        = active_;
        selectedForMidiControl = control_;
    }
}

//vector<string> ofxBaseGui::getAttributesForMidiLearn() {
//    
//    vector <string> result;
//    if (clicked) {
//        result.push_back(getName());
//    }
//    return result;
//}
//
//vector<string> ofxBaseGui::getAttributesForAudioIn() {
//
//    vector <string> result;
//    if (clicked) {
//        result.push_back(getName());
//    }
//    return result;
//}

vector<string> ofxBaseGui::getAttributesClicked() {

    vector <string> result;
    if ((clicked && midiLearnActive) ||                                                             // selected for midi learn
        (selectedForRightAudio && editRightAudioInActive && selectedForNodeId == editNodeId) ||     // selected for right audio in
        (selectedForLeftAudio && editLeftAudioInActive && selectedForNodeId == editNodeId) ||       // selected for left audio in
        (selectedForOSC && editOSCActive && selectedForNodeId == editNodeId)) {                     // selected for osc
        
        result.push_back(getName());
    }
    return result;
}

ofxBaseGui * ofxBaseGui::find(string name){
    if (getName() == name)
        return this;
    else return NULL;
}
