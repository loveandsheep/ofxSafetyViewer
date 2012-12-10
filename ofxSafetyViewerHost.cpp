//
//  ofxSafetyViewerHost.cpp
//  SMPTEVideoSync
//
//  Created by Ovis aries on 2012/11/22.
//
//

#include "ofxSafetyViewerHost.h"

void ofxSafetyViewerHost::setup(){
    receiver_osc.setup(TOHOST_PORT);
    currentLevel = 0;
    verbose = true;
    rePlacer_count = 0;
    bPubSetup = false;
}

void ofxSafetyViewerHost::update(){
    while(receiver_osc.hasWaitingMessages()){               //【クライアントからのデータ管理】
        ofxOscMessage m;
        receiver_osc.getNextMessage(&m);
        if (m.getAddress() == "/SafetyClient/Register"){    //クライアントの申請を受信
            bool isExist = false;                           //ダブってたら捨てる
			cout << "OSC:New Client: from " << m.getRemoteIp() << " Lv:" << m.getArgAsInt32(0) << endl;
            for (int i = 0;i < clients.size();i++){
                if (clients[i].ip == m.getRemoteIp()) isExist = true;
            }
            if (!isExist){
                client newcl;                                   //クライアント情報のアーカイブ
                newcl.ip = m.getRemoteIp();
                newcl.level = m.getArgAsInt32(0);
                newcl.counter_expDead = 0;
				newcl.publish_id = m.getArgAsInt32(1);
                clients.push_back(newcl);
                ofxOscMessage cnfrm;
                cnfrm.setAddress("/SafetyHost/CRegist");         //返答を送信
                cnfrm.addIntArg(clients.size()-1);               //参照しやすいように配列のインデックスを渡す
                sender_osc.setup(clients[clients.size()-1].ip, TOCLIENT_PORT);
                sender_osc.sendMessage(cnfrm);
            }else{
				cout << "But is Exist." << endl;
			}
        }
        
        
        if (m.getAddress() == "/SafetyClient/ImLive"){       //システムの生存確認
			clients[m.getArgAsInt32(0)].counter_expDead = 0;
        }
    }
    
    bool bDeadSomeone = false;
    
    for (int i = 0;i < clients.size();i++){
        clients[i].counter_expDead++;
		if (clients[i].counter_expDead > SOURCE_TIMEOUT_FRAME){
			bDeadSomeone = true;
			if (clients[i].level == currentLevel){
				currentLevel = 0;
			}
		}
//        if (clients[i].counter_expDead > SOURCE_TIMEOUT_FRAME){ //一定時間通信が途絶えた場合、ソースを捨てる
//            bDeadSomeone = true;
//            if (clients[i].level == currentLevel){
//                currentLevel = 0;
//            }
//            clients.erase(clients.begin()+i);
//			break;
//        }
    }
	
	if (bDeadSomeone){
		cout << "Dead Client lev::" << endl;
		vector<client>::iterator end_it = remove_if(clients.begin(), clients.end(), clientChecker());
		clients.erase(end_it,clients.end());
		cout << "current Lv:" << currentLevel << endl;
	}
	
    for (int i = 0;i < clients.size();i++){                     //順序が変わった場合、新しいインデックスを再発行する
		sender_osc.setup(clients[i].ip, TOCLIENT_PORT);
        if (bDeadSomeone){
            ofxOscMessage idx;
            idx.setAddress("/SafetyHost/Index");
            idx.addIntArg(i);
            sender_osc.sendMessage(idx);
        }
        ofxOscMessage liv;
        liv.setAddress("/SafetyHost/ImLive");
		if (clients[i].ip == currentHost){
			liv.addIntArg(1);
		}else{
			liv.addIntArg(0);
		}
        sender_osc.sendMessage(liv);
    }
}

void ofxSafetyViewerHost::draw(){
    rePlacer_count--;
    if (rePlacer_count < 0){
        for (int i = 0;i < clients.size();i++){
            if (clients[i].level > currentLevel){                   //優先度を評価して、受信先を判別
				cout << "Hit!" << endl;
                if (currentHost != clients[i].ip){
					receiver_pub.setup(clients[i].ip, TOHOST_PORT_PUB);
                    currentHost = clients[i].ip;
					cout << "new Client at:" << clients[i].ip << "  Lv:" << clients[i].level << endl;
					cout << "Port is:" << clients[i].publish_id << endl;
                }
                currentLevel = clients[i].level;
                rePlacer_count = 100;
            }
        }
    }
    
    
    if ((currentLevel != 0)&&(rePlacer_count < 95)){
		receiver_pub.update();
        receiver_pub.draw(0,0);
    }
}
