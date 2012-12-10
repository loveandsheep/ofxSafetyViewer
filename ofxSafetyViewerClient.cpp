//
//  ofxSafetyViewerClient.cpp
//  SMPTEVideoSync
//
//  Created by Ovis aries on 2012/11/22.
//
//

#include "ofxSafetyViewerClient.h"

void ofxSafetyViewerClient::setup(int level_){
    FILE *fp;
    
    char con_ip[256];
    char con_lev[256];
    char* p;
    fp = fopen("../../../data/config.txt", "r");
    fgets(con_ip , 256, fp);
    fgets(con_lev, 256, fp);
    
    p = strchr(con_ip, '\n' );
    if ( p != NULL )
    {
        *p = '\0';
    }
    
    receiver_osc.setup(TOCLIENT_PORT);
    sender_osc.setup(con_ip, TOHOST_PORT);
    registered = false;
    level = ofToInt(con_lev);
    buffer = NULL;
    
    cout << con_ip << endl;
    cout << level << endl;
}

void ofxSafetyViewerClient::update(){
    while (receiver_osc.hasWaitingMessages()){          //ホスト側の信号を受信
        ofxOscMessage m;
        receiver_osc.getNextMessage(&m);
        
        if (m.getAddress() == "/SafetyHost/CRegist"){   //登録の申請が受理された場合
            index = m.getArgAsInt32(0);                 //クライアントリストに登録されたインデックスを取得
            registered = true;
        }
        if (m.getAddress() == "/SafetyHost/Index"){     //インデックスが変更された場合の通知
            index = m.getArgAsInt32(0);
        }
        if (m.getAddress() == "/SafetyHost/ImLive"){    //ホストが生存しているかどうか確認
            hostDeadCounter = 0;
        }
    }
    
    hostDeadCounter ++;
    if (hostDeadCounter > 120) registered = false;
    
    ofxOscMessage regmsg;
    if (!registered){                                   //ホストに登録申請を送信し続ける
        regmsg.setAddress("/SafetyClient/Register");
        regmsg.addIntArg(level);
    }else{
        regmsg.setAddress("/SafetyClient/ImLive");      //登録済みであれば、生存信号を送る
        regmsg.addIntArg(index);
    }
    sender_osc.sendMessage(regmsg);
    
    if (buffer != NULL){
        sender_pub.begin();
        ofSetColor(255, 255, 255);
        buffer->draw(0, buffer->getHeight(),buffer->getWidth(),-buffer->getHeight());
        sender_pub.end();
    }
}

void ofxSafetyViewerClient::begin(){
    sender_pub.begin();
}

void ofxSafetyViewerClient::end(){
    sender_pub.end();
}

void ofxSafetyViewerClient::setBuffer(ofFbo *buf){
    buffer = buf;
    sender_pub.setup(TOHOST_PORT_PUB, buffer->getWidth(), buffer->getHeight());
}