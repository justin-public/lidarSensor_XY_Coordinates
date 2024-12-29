#include "ofApp.h"

//float screenXCal;
//float screenYCal;
//float screenXCalcopy;

//--------------------------------------------------------------
void ofApp::setup()
{
	lidarScale = 0.68;      // 0.1 0.68

	ofSetVerticalSync(true);
	ofSetFrameRate(60);

	/***************************************************/
	// GUI
	/***************************************************/
	panel.setup();
	panel.add(mLidarMirror0.set("Mirror 0", false));
	panel.add(mLidarAngleOffset0.set("AngleOffset 0", 100, -5, 5));
	panel.add(mlidarscale0.set("lidar scale0", 100, 1.0, 0));
	panel.add(mlidar0xpos.set("lidar X",100,3000,-3000));                // 0
	panel.add(mlidar0ypos.set("lidar y",100,3000,-3000));                // 0 
	panel.add(mlidar0Rot.set("Rotation",100,580,0));
	//panel.add(mlidar0xpos.set("lidar X", -2000, 2000, 2));
	//panel.add(mlidar0ypos.set("lidar y", 0, 3000, -3));
	
	panel.loadFromFile("server.xml");
#if 0
	panel = gui.addPanel();

	panel->loadTheme("theme/theme_light.json");
	panel->setName("Lidar server");

	broadcastGroup = panel->addGroup("Broadcast");
	broadcastGroup->add<ofxGuiTextField>(mBroadcastIP.set("Broadcast IP", "127.0.0.1"));
	broadcastGroup->add<ofxGuiIntInputField>(mBroadcastPort.set("Broadcast Port", 11111, 11110, 11119));

	listenerGroup = panel->addGroup("Listener");
	listenerGroup->add<ofxGuiIntInputField>(mListeningPort.set("Listening Port", 11121, 11120, 11129));

	lidarGroup = panel->addGroup("Lidar");
	lidarGroup->add(mLidarMirror0.set("Mirror 0", false));
	lidarGroup->add(mLidarAngleOffset0.set("AngleOffset 0", 0, -5, 5));
	panel->loadFromFile("server.xml");
#endif
	/***************************************************/
	// OSC
	/***************************************************/

	broadcaster.setup(mBroadcastIP.get(), mBroadcastPort.get());
	listener.setup(mListeningPort.get());

	/***************************************************/
	// Lidar
	/***************************************************/

	lidar10.setup("192.168.0.92", 10940);
	lidar10.startSensing();
	/***************************************************/
	// SensorField
	/***************************************************/

	sensor0.setup("sensor_0");
	setupViewports();
	mShowGraph = false;
	mShowHelp = true;
	createHelp();
}

//--------------------------------------------------------------
void ofApp::setupViewports() {
	//sensor0.panel->setWidth(MENU_WIDTH);
	//sensor0.panel->setPosition(ofGetWidth() - MENU_WIDTH, 20);

	float xOffset = VIEWGRID_WIDTH; //ofGetWidth() / 3;
	//float yOffset = VIEWPORT_HEIGHT / N_CAMERAS;
	float yOffset = VIEWPORT_HEIGHT;
	
	viewMain.x = xOffset;
	viewMain.y = 0;
	
	//viewMain.width = ofGetWidth() - xOffset - MENU_WIDTH; //xOffset * 2;
	//viewMain.height = VIEWPORT_HEIGHT;

	viewMain.width = ofGetWidth();
	viewMain.height = ofGetHeight();
#if 0
	for (int i = 0; i < N_CAMERAS; i++) {

		viewGrid[i].x = 0;
		viewGrid[i].y = yOffset * i;
		viewGrid[i].width = xOffset;
		viewGrid[i].height = yOffset;
	}
#endif
}

//--------------------------------------------------------------
void ofApp::update()
{
	if (lidar10.update()) {
		if (lidar10.calculateEuclidian(90, 270, mLidarAngleOffset0.get(), mLidarMirror0.get())) {
			if (sensor0.update(lidar10.getEuclidian())) {
				sensor0.broadcastEvents(broadcaster, lidar10.getTimeStamp());
			}
			distance0 = lidar10.getRawDistance(180. - mLidarAngleOffset0.get());
		}
	}
	updateListener();

	//std::cout << screenXCal << "," << screenYCal <<endl;
	//std::cout << screenXCal << endl;
}

//--------------------------------------------------------------
void ofApp::draw() {
	//ofDrawCircle(screenXCal, screenYCal, 32);
	//screenXCal = 0.0;
	//screenYCal = 0.0;
#if 1
	if (mShowGraph) {
		mainCam.begin(viewMain);
		glPushMatrix();
		/*----------------2D-----------------*/
#if 1
		glViewport(0, 0, 1920, 1200);                   // 1080
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		glOrtho(0.0, 1920.0, 1200.0, 0.0, -1.0f, 1.0f);
		glMatrixMode(GL_MODELVIEW);			// GL_MODELVIEW
		glPushMatrix();
		glLoadIdentity();
		glDisable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
#endif		
		/*----------------------------------*/
		//glTranslatef(15, -550, 0);      
		glTranslatef(mlidar0xpos, mlidar0ypos, 0);
		//glTranslatef(mlidarxpos, mlidarypos, 0);
		//glRotatef(180, 0, 0, 1);    
		glRotatef(mlidar0Rot, 0, 0, 1);
		glScalef(mlidarscale0, mlidarscale0, mlidarscale0);
		
		sensor0.drawEventLabels();
#if 0
		switch (iMainCamera) {
		case 0:
			lidar10.drawRays();
			//panel.draw();
			//sensor0.drawGui();
			sensor0.drawField();
			sensor0.drawEvents();
			sensor0.drawEventLabels();
			break;
		}
#endif
		glPopMatrix();
		mainCam.end();
		panel.draw();
		sensor0.drawGui();
		//panel->setHidden(false);             //
	}
	else {
		//panel->setHidden(true);
	}
	ofColor(0, 0, 0, 1);
	if (mShowHelp) {
		ofDrawBitmapString(help, VIEWGRID_WIDTH + 20, VIEWPORT_HEIGHT + 20);
	}
	ofDrawBitmapString("fps: " + ofToString(ofGetFrameRate()), ofGetWidth() - 200, 10);
#endif
}

bool ofApp::updateListener()
{
	while (listener.hasWaitingMessages()) {
		// get the next message
		ofxOscMessage m;
		listener.getNextMessage(m);
		//Log received message for easier debugging of participants' messages:
		ofLog(OF_LOG_NOTICE, "Server recvd msg " + getOscMsgAsString(m) + " from " + m.getRemoteIp());

		// check the address of the incoming message
		if (m.getAddress() == "/refresh") {
			//Identify host of incoming msg
			sensor0.broadcastBox(broadcaster);
			ofLogVerbose("Sensor received /refresh message");
		}
		else if (m.getAddress() == "/ping") {
			ofxOscMessage sensorbox;
			sensorbox.setAddress("/ping");
			sensorbox.addIntArg(1);

			broadcaster.sendMessage(sensorbox);
			ofLogVerbose("Sensor received /ping message");
		}
		// handle getting random OSC messages here
		else {
			ofLogWarning("Server got weird message: " + m.getAddress());
		}
	}
	return false;
}
void ofApp::exit()
{
	broadcaster.clear();
	listener.stop();
	lidar10.stopSensing();
	lidar10.exit();
}

void ofApp::createHelp() {
	help = string("press v -> to show visualizations\n");
	help += "press s -> to save current settings.\n";
	help += "press l -> to load last saved settings\n";
	help += "press 1 - 6 -> to change the viewport\n";
	help += "\n";
	help += "Height for lidar0 = " + ofToString(distance0) + "mm \n";
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
	switch (key) {
	case ' ':
		break;

	case'p':
		break;

	case'v':
		mShowGraph = !mShowGraph;
		break;

	case 'o':
		break;

	case 't':
		break;

	case 'r':
		break;

	case 'k':
		break;

	case 's':
		sensor0.save();
		panel.saveToFile("server.xml");
		break;

	case 'l':
		break;

	case 'm':
		break;

	case 'h':
		mShowHelp = !mShowHelp;
		if (mShowHelp) {
			createHelp();
		}
		break;

	case '>':
	case '.':
		//farThreshold ++;
		//if (farThreshold > 255) farThreshold = 255;
		break;

	case '<':
	case ',':
		//farThreshold --;
		//if (farThreshold < 0) farThreshold = 0;
		break;

	case '+':
	case '=':
		//nearThreshold ++;
		//if (nearThreshold > 255) nearThreshold = 255;
		break;

	case '-':
		//nearThreshold --;
		//if (nearThreshold < 0) nearThreshold = 0;
		break;

	case 'w':
		//kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
		break;

	case '0':
		//kinect.setLed(ofxKinect::LED_OFF);
		break;

	case '1':
		iMainCamera = 0;
		//kinect.setLed(ofxKinect::LED_GREEN);
		break;

	case '2':
		iMainCamera = 1;
		//kinect.setLed(ofxKinect::LED_YELLOW);
		break;

	case '3':
		iMainCamera = 2;
		//kinect.setLed(ofxKinect::LED_RED);
		break;

	case '4':
		iMainCamera = 3;
		//kinect.setLed(ofxKinect::LED_BLINK_GREEN);
		break;

	case '5':
		iMainCamera = 4;
		//kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
		break;

	case '6':
		iMainCamera = 5;
		//kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
		break;

	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {


}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

string ofApp::getOscMsgAsString(ofxOscMessage m) {
	string msg_string;
	msg_string = m.getAddress();
	msg_string += ":";
	for (int i = 0; i < m.getNumArgs(); i++) {
		// get the argument type
		msg_string += " " + m.getArgTypeName(i);
		msg_string += ":";
		// display the argument - make sure we get the right type
		if (m.getArgType(i) == OFXOSC_TYPE_INT32) {
			msg_string += ofToString(m.getArgAsInt32(i));
		}
		else if (m.getArgType(i) == OFXOSC_TYPE_FLOAT) {
			msg_string += ofToString(m.getArgAsFloat(i));
		}
		else if (m.getArgType(i) == OFXOSC_TYPE_STRING) {
			msg_string += m.getArgAsString(i);
		}
		else {
			msg_string += "unknown";
		}
	}
	return msg_string;
}