#include "ofxGuiGroup.h"
#include "ofxPanel.h"
#include "ofxSliderGroup.h"
#include "ofGraphics.h"
#include "ofxLabel.h"

ofxGuiGroup::ofxGuiGroup(){
	minimized = false;
	parent = NULL;
	spacing  = 1;
	spacingNextElement = 3;
	header = defaultHeight;
	bGuiActive = false;
}

ofxGuiGroup::ofxGuiGroup(const ofParameterGroup & parameters, string filename, float x, float y){
	minimized = false;
	parent = NULL;
    setup(parameters, filename, x, y);
}

ofxGuiGroup * ofxGuiGroup::setup(string collectionName, string filename, float x, float y){
	parameters.setName(collectionName);
	return setup(parameters,filename,x,y);
}

ofxGuiGroup * ofxGuiGroup::setup(const ofParameterGroup & _parameters, string _filename, float x, float y){
	b.x = x;
	b.y = y;
	header = defaultHeight;
	spacing  = 1;
	spacingNextElement = 3;
	if(parent!=NULL){
		b.width = parent->getWidth();
	}else{
		b.width = defaultWidth;
	}
    clear();
	filename = _filename;
	bGuiActive = false;
    
	for(int i=0;i<_parameters.size();i++){
		string type = _parameters.getType(i);
		if(type==typeid(ofParameter<int>).name()){
			ofParameter<int> p = _parameters.getInt(i);
			add(p);
		}else if(type==typeid(ofParameter<float>).name()){
			ofParameter<float> p = _parameters.getFloat(i);
			add(p);
		}else if(type==typeid(ofParameter<bool>).name()){
			ofParameter<bool> p = _parameters.getBool(i);
			add(p);
		}else if(type==typeid(ofParameter<ofVec2f>).name()){
			ofParameter<ofVec2f> p = _parameters.getVec2f(i);
			add(p);
		}else if(type==typeid(ofParameter<ofVec3f>).name()){
			ofParameter<ofVec3f> p = _parameters.getVec3f(i);
			add(p);
		}else if(type==typeid(ofParameter<ofVec4f>).name()){
			ofParameter<ofVec4f> p = _parameters.getVec4f(i);
			add(p);
		}else if(type==typeid(ofParameter<ofColor>).name()){
			ofParameter<ofColor> p = _parameters.getColor(i);
			add(p);
		}else if(type==typeid(ofParameter<ofShortColor>).name()){
			ofParameter<ofShortColor> p = _parameters.getShortColor(i);
			add(p);
		}else if(type==typeid(ofParameter<ofFloatColor>).name()){
			ofParameter<ofFloatColor> p = _parameters.getFloatColor(i);
			add(p);
		}else if(type==typeid(ofParameter<string>).name()){
        		ofParameter<string> p = _parameters.getString(i);
			add(p);
		}else if(type==typeid(ofParameterGroup).name()){
			ofParameterGroup p = _parameters.getGroup(i);
			ofxGuiGroup * panel = new ofxGuiGroup(p);
			add(panel);
		}else{
			ofLogWarning() << "ofxBaseGroup; no control for parameter of type " << type;
		}
	}

	parameters = _parameters;
    registerMouseEvents();
    registerKeyEvents();

	generateDraw();
    
	return this;
}

void ofxGuiGroup::add(ofxBaseGui * element){
	collection.push_back( element );

	element->setPosition(b.x, b.y + b.height  + spacing);

	b.height += element->getHeight() + spacing;

	//if(b.width<element->getWidth()) b.width = element->getWidth();
    
    element->unregisterMouseEvents();
    element->unregisterKeyEvents();
    
	ofxGuiGroup * subgroup = dynamic_cast<ofxGuiGroup*>(element);
	if(subgroup!=NULL){
		subgroup->filename = filename;
		subgroup->parent = this;
		subgroup->setWidthElements(b.width*.98);
	}else{
		if(parent!=NULL){
			element->setSize(b.width*.98,element->getHeight());
			element->setPosition(b.x + b.width-element->getWidth(),element->getPosition().y);
		}
	}
    
	parameters.add(element->getParameter());
	generateDraw();
}

void ofxGuiGroup::setWidthElements(float w){
	for(int i=0;i<(int)collection.size();i++){
		collection[i]->setSize(w,collection[i]->getHeight());
		collection[i]->setPosition(b.x + b.width-w,collection[i]->getPosition().y);
		ofxGuiGroup * subgroup = dynamic_cast<ofxGuiGroup*>(collection[i]);
		if(subgroup!=NULL){
			subgroup->setWidthElements(w*.98);
		}
	}
	sizeChangedCB();
	generateDraw();
}

void ofxGuiGroup::add(const ofParameterGroup & parameters){
	ofxGuiGroup * panel = new ofxGuiGroup(parameters);
	panel->parent = this;
	add(panel);
}

void ofxGuiGroup::add(ofParameter<float> & parameter){
	add(new ofxFloatSlider(parameter,b.width));
}

void ofxGuiGroup::add(ofParameter<int> & parameter){
	add(new ofxIntSlider(parameter,b.width));
}

ofxBaseGui* ofxGuiGroup::add(ofParameter<bool> & parameter){
    ofxBaseGui* newComponent = new ofxToggle(parameter,b.width);
	add(newComponent);
    return newComponent;
}

void ofxGuiGroup::add(ofParameter<string> & parameter){
	add(new ofxLabel(parameter,b.width));
}

void ofxGuiGroup::add(ofParameter<ofVec2f> & parameter){
	add(new ofxVecSlider_<ofVec2f>(parameter,b.width));
}

void ofxGuiGroup::add(ofParameter<ofVec3f> & parameter){
	add(new ofxVecSlider_<ofVec3f>(parameter,b.width));
}

void ofxGuiGroup::add(ofParameter<ofVec4f> & parameter){
	add(new ofxVecSlider_<ofVec4f>(parameter,b.width));
}

void ofxGuiGroup::add(ofParameter<ofColor> & parameter){
	add(new ofxColorSlider_<unsigned char>(parameter,b.width));
}

void ofxGuiGroup::add(ofParameter<ofShortColor> & parameter){
	add(new ofxColorSlider_<unsigned short>(parameter,b.width));
}

void ofxGuiGroup::add(ofParameter<ofFloatColor> & parameter){
	add(new ofxColorSlider_<float>(parameter,b.width));
}

void ofxGuiGroup::remove(string label) {
    for(int i = 0; i < (int)collection.size(); i++){
        if(collection[i]->getName() == label) {
            collection[i]->unregisterKeyEvents();
            collection[i]->unregisterMouseEvents();
            collection.erase(collection.begin() + i);
        }
    }
}

void ofxGuiGroup::remove(ofxBaseGui* parameter) {
    for(int i = 0; i < (int)collection.size(); i++){
        if(collection[i] == (ofxBaseGui*)parameter) {
            collection[i]->unregisterKeyEvents();
            collection[i]->unregisterMouseEvents();
            collection.erase(collection.begin() + i);
        }
    }
}

void ofxGuiGroup::clear(){
	collection.clear();
	b.height = header + spacing + spacingNextElement ;
}

bool ofxGuiGroup::mouseMoved(ofMouseEventArgs & args){
	ofMouseEventArgs a = args;
	for(int i = 0; i < (int)collection.size(); i++){
		if(collection[i]->mouseMoved(a)) return true;
	}
	if(isGuiDrawing() && b.inside(ofPoint(args.x,args.y))){
		return true;
	}else{
		return false;
	}
}

bool ofxGuiGroup::mousePressed(ofMouseEventArgs & args){
	if(setValue(args.x, args.y, true)){
		return true;
	}
	if( bGuiActive ){
		ofMouseEventArgs a = args;
        bool result = false;
		for(int i = 0; i < (int)collection.size(); i++){
            if(collection[i]->mousePressed(a)) {
                result = true;
            }
		}
        return result;
	}
    else if (commandPressed || midiLearnActive || editLeftAudioInActive || editRightAudioInActive) {
        return false;
    }
    else {
        for(int i = 0; i < (int)collection.size(); i++){
            collection[i]->setClicked(false);
        }
    }
	return false;
}

bool ofxGuiGroup::mouseDragged(ofMouseEventArgs & args){
	if(setValue(args.x, args.y, false)){
		return true;
	}
	if( bGuiActive ){
		ofMouseEventArgs a = args;
		for(int i = 0; i < (int)collection.size(); i++){
			if(collection[i]->mouseDragged(a)) return true;
		}
	}
	return false;
}

bool ofxGuiGroup::mouseReleased(ofMouseEventArgs & args){
	bGuiActive = false;
    bool result = false;
	for(int k = 0; k < (int)collection.size(); k++){
		ofMouseEventArgs a = args;
		if(collection[k]->mouseReleased(a)) result = true;
	}
    if (result) {
        return true;
    }
	else if(isGuiDrawing() && b.inside(ofPoint(args.x,args.y))){
		return true;
	}else{
		return false;
	}
}

bool ofxGuiGroup::keyPressed(ofKeyEventArgs & args){
    commandPressed = (OF_KEY_LEFT_COMMAND || args.key == OF_KEY_RIGHT_COMMAND);
    
    ofKeyEventArgs a = args;
    bool result      = false;
    for(int i = 0; i < (int)collection.size(); i++){
        if (collection[i]->keyPressed(a))
        result = true;
    }
    return result;
}

bool ofxGuiGroup::keyReleased(ofKeyEventArgs & args){
    commandPressed = false;
    ofKeyEventArgs a = args;
    for(int i = 0; i < (int)collection.size(); i++){
        collection[i]->keyReleased(a);
    }
    return false;
}

void ofxGuiGroup::generateDraw(){
	border.clear();
	border.setFillColor(ofColor(thisBorderColor,180));
	border.setFilled(true);
	border.rectangle(b.x,b.y+ spacingNextElement,b.width+1,b.height);


	headerBg.clear();
	headerBg.setFillColor(thisHeaderBackgroundColor);
	headerBg.setFilled(true);
	headerBg.rectangle(b.x,b.y +1 + spacingNextElement, b.width, header);

	textMesh = getTextMesh(getName(), textPadding + b.x, header / 2 + 4 + b.y+ spacingNextElement);
	if(minimized){
		textMesh.append(getTextMesh("+", b.width-textPadding-8 + b.x, header / 2 + 4+ b.y+ spacingNextElement));
	}else{
		textMesh.append(getTextMesh("-", b.width-textPadding-8 + b.x, header / 2 + 4 + b.y+ spacingNextElement));
	}
}

void ofxGuiGroup::render(){
	border.draw();
	headerBg.draw();

	ofBlendMode blendMode = ofGetStyle().blendingMode;
	if(blendMode!=OF_BLENDMODE_ALPHA){
		ofEnableAlphaBlending();
	}
	ofColor c = ofGetStyle().color;
	ofSetColor(thisTextColor);

	bindFontTexture();
	textMesh.draw();
	unbindFontTexture();
    
	if(!minimized){
		for(int i = 0; i < (int)collection.size(); i++){
			collection[i]->draw();
		}
	}

	ofSetColor(c);
	if(blendMode!=OF_BLENDMODE_ALPHA){
		ofEnableBlendMode(blendMode);
	}
}

vector<string> ofxGuiGroup::getControlNames(){
	vector<string> names;
	for(int i=0; i<(int)collection.size(); i++){
		names.push_back(collection[i]->getName());
	}
	return names;
}

ofxIntSlider & ofxGuiGroup::getIntSlider(string name){
	return getControlType<ofxIntSlider>(name);
}

ofxFloatSlider & ofxGuiGroup::getFloatSlider(string name){
	return getControlType<ofxFloatSlider>(name);
}

ofxToggle & ofxGuiGroup::getToggle(string name){
	return getControlType<ofxToggle>(name);
}

ofxButton & ofxGuiGroup::getButton(string name){
	return getControlType<ofxButton>(name);
}

ofxGuiGroup & ofxGuiGroup::getGroup(string name){
	return getControlType<ofxGuiGroup>(name);
}

ofxBaseGui * ofxGuiGroup::getControl(string name){
	for(int i=0; i<(int)collection.size(); i++){
		if(collection[i]->getName()==name){
			return collection[i];
		}
	}
	return NULL;
}

ofxBaseGui * ofxGuiGroup::find(string name){
    ofxBaseGui* result;
    for(int i=0; i<(int)collection.size(); i++){
        result = collection[i]->find(name);
        if(result != NULL){
            return result;
        }
    }
    return NULL;
}

bool ofxGuiGroup::setValue(float mx, float my, bool bCheck){
    
	if( !isGuiDrawing() ){
		bGuiActive = false;
		return false;
	}


	if( bCheck ){
		if( b.inside(mx, my) ){
			bGuiActive = true;

			ofRectangle minButton(b.x+b.width-textPadding*3,b.y,textPadding*3,header);
			if(minButton.inside(mx,my)){
				minimized = !minimized;
				if(minimized){
					minimize();
				}else{
					maximize();
				}
				return true;
			}
        }
	}

	return false;
}

void ofxGuiGroup::minimize(){
	minimized=true;
	b.height = header + spacing + spacingNextElement + 1 /*border*/;
	if(parent) parent->sizeChangedCB();
	generateDraw();
}

void ofxGuiGroup::maximize(){
	minimized=false;
	for(int i=0;i<(int)collection.size();i++){
		b.height += collection[i]->getHeight() + spacing;
	}
	if(parent) parent->sizeChangedCB();
	generateDraw();
}

void ofxGuiGroup::minimizeAll(){
	for(int i=0;i<(int)collection.size();i++){
		ofxGuiGroup * group = dynamic_cast<ofxGuiGroup*>(collection[i]);
		if(group)group->minimize();
	}
}

void ofxGuiGroup::maximizeAll(){
	for(int i=0;i<(int)collection.size();i++){
		ofxGuiGroup * group = dynamic_cast<ofxGuiGroup*>(collection[i]);
		if(group)group->maximize();
	}
}

void ofxGuiGroup::sizeChangedCB(){
	float y;
	if(parent){
		y = b.y  + header + spacing + spacingNextElement;
	}else{
		y = b.y  + header + spacing;
	}
	for(int i=0;i<(int)collection.size();i++){
		collection[i]->setPosition(collection[i]->getPosition().x,y + spacing);
		y += collection[i]->getHeight() + spacing;
	}
	b.height = y - b.y;
	if(parent) parent->sizeChangedCB();
	generateDraw();
}


int ofxGuiGroup::getNumControls(){
	return collection.size();
}

ofxBaseGui * ofxGuiGroup::getControl(int num){
	if(num<(int)collection.size())
		return collection[num];
	else
		return NULL;
}

ofAbstractParameter & ofxGuiGroup::getParameter(){
	return parameters;
}

void ofxGuiGroup::setPosition(ofPoint p){
	ofVec2f diff = p - b.getPosition();

	for(int i=0;i<(int)collection.size();i++){
		collection[i]->setPosition(collection[i]->getPosition()+diff);
	}

	b.setPosition(p);
	generateDraw();
}

void ofxGuiGroup::setPosition(float x, float y){
	setPosition(ofVec2f(x,y));
}

void ofxGuiGroup::setClicked(bool click_) {
    
    for(int i = 0; i < (int)collection.size(); i++){
        collection[i]->setClicked(false);
    }
    clicked = click_;
};

void ofxGuiGroup::setMidiLearnActive(bool active_) {
    
    for(int i = 0; i < (int)collection.size(); i++){
        collection[i]->setMidiLearnActive(active_);
        if (!active_){
            collection[i]->setClicked(false);
        }
    }
    midiLearnActive        = active_;
    editLeftAudioInActive  = false;
    editRightAudioInActive = false;
    editNodeId             = -1;
    editOSCActive          = false;
}

void ofxGuiGroup::setMidiControlActive(int control_) {

    for(int i = 0; i < (int)collection.size(); i++){
        collection[i]->setMidiControlActive(control_);
    }
}

void ofxGuiGroup::resetMidiControlActive(int control_) {
    
    for(int i = 0; i < (int)collection.size(); i++){
        collection[i]->resetMidiControlActive(control_);
    }
}

void ofxGuiGroup::setEditLeftAudioInActive(bool active_, int node_) {
    
    for(int i = 0; i < (int)collection.size(); i++){
        collection[i]->setEditLeftAudioInActive(active_, node_);
        if (!active_){
            collection[i]->setClicked(false);
        }
    }
    editLeftAudioInActive = active_;
    editNodeId            = active_ ? node_ : -1;
    midiLearnActive       = false;
    editOSCActive         = false;
}

void ofxGuiGroup::setEditRightAudioInActive(bool active_, int node_) {
    
    for(int i = 0; i < (int)collection.size(); i++){
        collection[i]->setEditRightAudioInActive(active_, node_);
        if (!active_){
            collection[i]->setClicked(false);
        }
    }
    editRightAudioInActive = active_;
    editNodeId             = active_ ? node_ : -1;
    midiLearnActive        = false;
    editOSCActive          = false;
}

void ofxGuiGroup::setEditOSCActive(bool active_, int node_) {
    
    for(int i = 0; i < (int)collection.size(); i++){
        collection[i]->setEditOSCActive(active_, node_);
        if (!active_){
            collection[i]->setClicked(false);
        }
    }
    editOSCActive          = active_;
    editNodeId             = active_ ? node_ : -1;
    midiLearnActive        = false;
    editLeftAudioInActive  = false;
    editRightAudioInActive = false;
}

void ofxGuiGroup::setSelectedForLeftAudio(bool active_, int node_, string name_) {
    
    for(int i = 0; i < (int)collection.size(); i++){
        collection[i]->setSelectedForLeftAudio(active_, node_, name_);
    }
}

void ofxGuiGroup::setSelectedForRightAudio(bool active_, int node_, string name_) {
    
    for(int i = 0; i < (int)collection.size(); i++){
        collection[i]->setSelectedForRightAudio(active_, node_, name_);
    }
}

void ofxGuiGroup::setSelectedForOSC(bool active_, int node_, string name_) {
    
    for(int i = 0; i < (int)collection.size(); i++){
        collection[i]->setSelectedForOSC(active_, node_, name_);
    }
}

void ofxGuiGroup::setSelectedForMIDI(bool active_, int control_, string name_) {
    
    for(int i = 0; i < (int)collection.size(); i++){
        collection[i]->setSelectedForMIDI(active_, control_, name_);
    }
}

//vector <string> ofxGuiGroup::getAttributesForMidiLearn() {
//    
//    vector <string> result;
//    
//    if ((int)collection.size() > 0) {
//        vector<string> partialResult;
//        for(int i = 0; i < (int)collection.size(); i++){
//            partialResult = collection[i]->getAttributesForMidiLearn();
//            if (partialResult.size() > 0) {
//                result.insert(result.end(), partialResult.begin(), partialResult.end());
//            }
//        }
//        partialResult.clear();
//    }
//    else if (clicked) {
//        result.push_back(getName());
//    }
//    return result;
//}
//
//vector <string> ofxGuiGroup::getAttributesForAudioIn() {
//    
//    vector <string> result;
//    
//    if ((int)collection.size() > 0) {
//        vector<string> partialResult;
//        for(int i = 0; i < (int)collection.size(); i++){
//            partialResult = collection[i]->getAttributesForAudioIn();
//            if (partialResult.size() > 0) {
//                result.insert(result.end(), partialResult.begin(), partialResult.end());
//            }
//        }
//        partialResult.clear();
//    }
//    else if (clicked) {
//        result.push_back(getName());
//    }
//    return result;
//}

vector <string> ofxGuiGroup::getAttributesClicked() {
    
    vector <string> result;
    
    if ((int)collection.size() > 0) {
        vector<string> partialResult;
        for(int i = 0; i < (int)collection.size(); i++){
            partialResult = collection[i]->getAttributesClicked();
            if (partialResult.size() > 0) {
                result.insert(result.end(), partialResult.begin(), partialResult.end());
            }
        }
        partialResult.clear();
    }
    else if ((clicked && midiLearnActive) ||                                                            // selected for midi learn
             (selectedForRightAudio && editRightAudioInActive && selectedForNodeId == editNodeId) ||    // selected for right audio in
             (selectedForLeftAudio && editLeftAudioInActive && selectedForNodeId == editNodeId) ||      // selected for left audio in
             (selectedForOSC && editOSCActive && selectedForNodeId == editNodeId)) {                    // selected for osc
        
        result.push_back(getName());
    }
    return result;
}

void ofxGuiGroup::addOrRemoveOSCInput(ofxOSCGuiEvent &e_) {
    
    ofNotifyEvent(addOrRemoveOSCInputGui, e_);
}