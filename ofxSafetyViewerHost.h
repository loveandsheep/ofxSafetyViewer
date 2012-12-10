//
//  ofxSafetyViewerHost.h
//  SMPTEVideoSync
//
//  Created by Ovis aries on 2012/11/22.
//
//

#include "ofMain.h"
#include "ofxPublishScreen.h"
#include "ofxOsc.h"
#include "ofxSVUtil.h"

class client{
public:
    string ip;
    int level;
    int CeckID;
    int counter_expDead;
	int publish_id;
};

class clientChecker{
public:
	bool operator()(client cl)const{
		return (cl.counter_expDead > SOURCE_TIMEOUT_FRAME);
	}
};

/******************受信側*******************/
class ofxSafetyViewerHost{
public:
    void setup();
    void update();
    void draw();
    
    vector<client> clients;
    ofxPublishScreen::Subscriber receiver_pub;
    
    
    bool bPubSetup;
    ofxOscReceiver receiver_osc;
    ofxOscSender sender_osc;
    
    string currentHost;
    int currentLevel;//現在使用しているシステムの優先レベル
    bool verbose;
    
    int rePlacer_count;//パブリッシャーが速く切り替わりすぎないようコントロール
	
};