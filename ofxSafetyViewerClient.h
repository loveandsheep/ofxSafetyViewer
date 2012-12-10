//
//  ofxSafetyViewerClient.h
//  SMPTEVideoSync
//
//  Created by Ovis aries on 2012/11/22.
//
//

#include "ofMain.h"
#include "ofxPublishScreen.h"
#include "ofxOsc.h"
#include "ofxSVUtil.h"

class ofxSafetyViewerClient{
public:
    void setup(int level_);
    void update();
    void begin();
    void end();
    void setBuffer(ofFbo* buf);
    
    ofFbo* buffer;
    
    ofxPublishScreen::FboPublisher sender_pub;
    ofxOscSender sender_osc;
    ofxOscReceiver receiver_osc;
    
    int hostDeadCounter;
    bool registered;
    int level;
    int index;
};