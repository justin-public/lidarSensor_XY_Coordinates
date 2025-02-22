#pragma once

#include "ofxNetwork.h"
#include "ofMain.h"
#include "ofxCv.h"
//#include "ofxOpenCv.h"
#include "ofxHokuyo.h"
#include "Sensor.h"
#include "extern.h"

#define LIDARRANGE 1081

#define N_CAMERAS 5

#define VIEWGRID_WIDTH  0        // 200
#define MENU_WIDTH      200
#define VIEWPORT_HEIGHT 480

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void exit();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

protected:
	bool updateListener();
	void createHelp();
	string getOscMsgAsString(ofxOscMessage m);

	string help;

	void setupViewports();
	ofRectangle viewMain;
	ofRectangle viewGrid[N_CAMERAS];

	ofEasyCam mainCam;
	ofCamera previewCams[N_CAMERAS];

	/***************************************************/
	// Lidar
	/***************************************************/

	lidar::ofxHokuyo lidar10;
	lidar::ofxHokuyo lidar11;
	lidar::ofxHokuyo lidar12;
	lidar::ofxHokuyo lidar13;
	lidar::ofxHokuyo lidar14;

	/***************************************************/
	// SensorField
	/***************************************************/

	sensor::SensorField sensor0;
	sensor::SensorField sensor1;
	sensor::SensorField sensor2;
	sensor::SensorField sensor3;
	sensor::SensorField sensor4;
	sensor::SensorField sensor5;

	int iMainCamera;

	bool mShowGraph, mShowHelp;

	float lidarScale;

	int distance0, distance1, distance2, distance3, distance4;

	/***************************************************/
	// OSC
	/***************************************************/

	ofxOscSender broadcaster;
	ofxOscReceiver listener;

	/***************************************************/
	// GUI
	/***************************************************/

	//ofxGui gui;

	ofxPanel panel;

	ofxGuiGroup* broadcastGroup;
	ofxGuiGroup* listenerGroup;
	ofxGuiGroup* lidarGroup;

	ofParameter<string> mBroadcastIP;
	ofParameter<int> mBroadcastPort;
	ofParameter<int> mListeningPort;

	ofParameter<float> mLidarAngleOffset0;
	ofParameter<float> mLidarAngleOffset1;
	ofParameter<float> mLidarAngleOffset2;
	ofParameter<float> mLidarAngleOffset3;
	ofParameter<float> mLidarAngleOffset4;

	ofParameter<bool> mLidarMirror0;
	ofParameter<bool> mLidarMirror1;
	ofParameter<bool> mLidarMirror2;
	ofParameter<bool> mLidarMirror3;
	ofParameter<bool> mLidarMirror4;
	
	ofParameter<float> mlidarscale0;
	ofParameter<float> mlidar0xpos;
	ofParameter<float> mlidar0ypos;
	ofParameter<float> mlidar0Rot;
};