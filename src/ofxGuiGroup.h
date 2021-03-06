#pragma once
#include "ofxBaseGui.h"

#include "ofxSlider.h"
#include "ofxButton.h"
#include "ofParameterGroup.h"

class ofxGuiGroup : public ofxBaseGui {
public:
	ofxGuiGroup();
	ofxGuiGroup(const ofParameterGroup & parameters, string _filename="settings.xml", float x = 10, float y = 10);
    virtual ~ofxGuiGroup() {}
    virtual ofxGuiGroup * setup(string collectionName="", string filename="settings.xml", float x = 10, float y = 10);
	virtual ofxGuiGroup * setup(const ofParameterGroup & parameters, string filename="settings.xml", float x = 10, float y = 10);
    
	void add(ofxBaseGui * element);
    void add(const ofParameterGroup & parameters);
	void add(ofParameter<float> & parameter);
	void add(ofParameter<int> & parameter);
	ofxBaseGui* add(ofParameter<bool> & parameter);
	void add(ofParameter<string> & parameter);
    void add(ofParameter<ofVec2f> & parameter);
    void add(ofParameter<ofVec3f> & parameter);
    void add(ofParameter<ofVec4f> & parameter);
    void add(ofParameter<ofColor> & parameter);
    void add(ofParameter<ofShortColor> & parameter);
    void add(ofParameter<ofFloatColor> & parameter);
    
    void remove(string label);
    void remove(ofxBaseGui* parameter);

    void minimize();
    void maximize();
    void minimizeAll();
    void maximizeAll();

	void setWidthElements(float w);

	void clear();
	
	virtual bool mouseMoved(ofMouseEventArgs & args);
	virtual bool mousePressed(ofMouseEventArgs & args);
	virtual bool mouseDragged(ofMouseEventArgs & args);
	virtual bool mouseReleased(ofMouseEventArgs & args);
	
    virtual bool keyPressed(ofKeyEventArgs &args);
    virtual bool keyReleased(ofKeyEventArgs &args);
    
	vector<string> getControlNames();
	int getNumControls();
    
	ofxIntSlider & getIntSlider(string name);
	ofxFloatSlider & getFloatSlider(string name);
	ofxToggle & getToggle(string name);
	ofxButton & getButton(string name);
	ofxGuiGroup & getGroup(string name);
    
	ofxBaseGui * getControl(string name);
    ofxBaseGui * getControl(int num);
    
	virtual ofAbstractParameter & getParameter();

	virtual void setPosition(ofPoint p);
	virtual void setPosition(float x, float y);
    
    //custom
    void setClicked(bool click_);
    virtual void setMidiLearnActive(bool active_);
    virtual void setMidiControlActive(int control_);
    virtual void resetMidiControlActive(int control_);
    virtual void setEditLeftAudioInActive(bool active_, int node_);
    virtual void setEditRightAudioInActive(bool active_, int node_);
    virtual void setEditOSCActive(bool active_, int node_);
    virtual void setSelectedForLeftAudio(bool active_, int node_, string name_);
    virtual void setSelectedForRightAudio(bool active_, int node_, string name_);
    virtual void setSelectedForOSC(bool active_, int node_, string name_);
    virtual void setSelectedForMIDI(bool active_, int control_, string name_);
    virtual vector <string> getAttributesClicked();
    //    virtual vector <string> getAttributesForAudioIn();
    //    virtual vector <string> getAttributesForMidiLearn();
    void addOrRemoveOSCInput(ofxOSCGuiEvent &e_);
    virtual ofxBaseGui* find(string name);
    
    ofEvent<ofxOSCGuiEvent> addOrRemoveOSCInputGui;
    
protected:
	virtual void render();
    virtual bool setValue(float mx, float my, bool bCheck);
    void sizeChangedCB();
    
	float spacing,spacingNextElement;
	float header;
	
    template<class ControlType>
	ControlType & getControlType(string name);

    virtual void generateDraw();

	vector <ofxBaseGui *> collection;
	ofParameterGroup parameters;
	
	string filename;
	bool minimized;
	bool bGuiActive;

	ofxGuiGroup * parent;
	ofPath headerBg;
	ofVboMesh textMesh;
};

template<class ControlType>
ControlType & ofxGuiGroup::getControlType(string name){
	ControlType * control = dynamic_cast<ControlType*>(getControl(name));
	if(control){
		return *control;
	}else{
		ofLogWarning() << "getControlType " << name << " not found, creating new";
		control = new ControlType;
		control->setName(name);
		add(control);
		return *control;
	}
}
