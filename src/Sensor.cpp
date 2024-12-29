#include "Sensor.h"

float screenXCal[100];
float screenYCal[100];

//float screenXCal;
//float screenYCal;

/*
float screenX[100];
float screenY[100];
int Xpos[100];
int Ypos[100];
float SettingYoffset[100];
*/

sensor::Event::Event(int ID, glm::vec2 pos, int size, int generationSize, int lifeSpan)
{
	mID = ID;
	mCenter = pos;
	mLastCenter = pos;
	mSize = size;
	mGenerationSize = generationSize;
	mBreathSize = lifeSpan;
	mCountDown = mBreathSize;
	mLifeCycles = ofGetElapsedTimeMillis();
	mIsDying = false;
}

sensor::Event::~Event()
{
}

void sensor::Event::prepare()
{
	mCountDown--;
}

bool sensor::Event::isSame(glm::vec2 pos)
{
	return (!mIsDying && glm::distance(pos, mCenter) <= mGenerationSize)?true: false;
}

void sensor::Event::update(glm::vec2 pos, int size, float smoothPos, float smoothSize)
{
	mCenter = pos * (1.0 - smoothPos) + mCenter * smoothPos;
	mSize = size * (1.0 - smoothSize) + mSize * smoothSize;;
	mCountDown = mBreathSize;
}

bool sensor::Event::cleanup()
{
	if (mCountDown == 0) {
		mIsDying = true;
	}
	return  (mCountDown < 0)?true: false;
}

bool sensor::Event::isDying()
{
	return mIsDying;
}

int sensor::Event::getID()
{
	return mID;
}

glm::vec2 sensor::Event::getCenter()
{
	return mCenter;
}

int sensor::Event::getSize()
{
	return mSize;
}

int sensor::Event::getElapsedMillis()
{
	return ofGetElapsedTimeMillis() - mLifeCycles;
}

void sensor::Event::draw()
{
	ofDrawCircle(mCenter, mSize);
}

sensor::SensorField::SensorField()
{
	fieldID.set("sensorFieldID", 0, 0, 10);
#if 1
	limitUp.set("UpperLimit [mm]", -1, 0, -4000);                          // 0      
	limitDown.set("LowerLimit [mm]", -1, 0, -4000);                        // 0
	limitLeft.set("LeftLimit [mm]", -1, -8000, 8000);
	limitRight.set("RightLimit [mm]", -1, -8000, 8000);
#endif
#if 0	
	limitUp.set("UpperLimit [mm]", -500, 0, -4000);
	limitDown.set("LowerLimit [mm]", -2500, 0, -4000);
	limitLeft.set("LeftLimit [mm]", -2000, -8000, 8000);
	limitRight.set("RightLimit [mm]", 2000, -8000, 8000);
#endif	
	eventSize.set("Search size", 100, 0, 1000);
	eventRayGap.set("Ray Gap", 5, 0, 20);
	eventBreathSize.set("Cycles to death", 10, 0, 100);
	smoothingPos.set("Smooth Pos", 0.5, 0.0, 1.0);
	smoothingSize.set("Smooth Size", 0.5, 0.0, 1.0);
}

sensor::SensorField::~SensorField()
{
}

void sensor::SensorField::setup(string name) {
	udpConnection.Create();
	udpConnection.Connect("127.0.0.1",8000);
	udpConnection.SetNonBlocking(true);
	
	
	panel.setup();
	panel.add(limitUp);
	panel.add(limitDown);
	panel.add(limitLeft);
	panel.add(limitRight);
	
	panel.add(eventSize);
	panel.add(eventRayGap);
	panel.add(eventBreathSize);
	panel.add(smoothingPos);
	panel.add(smoothingSize);
	
	panel.loadFromFile("sensor_0.xml");
}

#if 0
void sensor::SensorField::setup(ofxGui &gui, string name)
{
	panel = gui.addPanel();

	panel->loadTheme("theme/theme_light.json");
	panel->setName(name);

	//panel->add<ofxGuiIntInputField>(fieldID);

	fieldGroup = panel->addGroup("SensorField");
	fieldGroup->add(limitUp);
	fieldGroup->add(limitDown);
	fieldGroup->add(limitLeft);
	fieldGroup->add(limitRight);

	sensitivityGroup = panel->addGroup("Sensitivity");
	sensitivityGroup->add(eventSize);
	sensitivityGroup->add(eventRayGap);
	sensitivityGroup->add(eventBreathSize);
	sensitivityGroup->add(smoothingPos);
	sensitivityGroup->add(smoothingSize);

	panel->loadFromFile(name + ".xml");


	myfont.load("verdana.ttf", 100);
}
#endif

bool sensor::SensorField::update(std::vector<glm::vec3> data)
{
	//panel.setHidden(true);
	int minID = 0;

	// we prepare all the previous events
	for (int e = 0; e < events.size(); e++) {
		events[e].prepare();
		minID = (events[e].getID() >= minID)? events[e].getID() + 1: minID;
	}

	int rayGap = 0;
	int counter = 0;
	glm::vec2 hit = glm::vec2();
	glm::vec2 firsthit = glm::vec2();
	glm::vec2 lastthit = glm::vec2();
	// now do our job
	for (int i = 0; i < data.size(); i++) {
		// first check if the point is inside the sensorfield:	
		if (limitDown.get() < data[i].y && data[i].y < limitUp.get() &&
			limitLeft.get() < data[i].x && data[i].x < limitRight.get()) {
			
			//std::cout << "got event at " << data[i].x << " , " << data[i].y << " size: " << counter << "\n";
			//std::cout << "got event... " << "\n";
			if (counter == 0) {
				firsthit = glm::vec2(data[i].x, data[i].y);
			}
			else {
				lastthit = glm::vec2(data[i].x, data[i].y);
			}
			counter++;    // 선들의 수
			hit += glm::vec2(data[i].x, data[i].y);
			//std::cout << counter << endl;
		}
		else {
			//if (counter > 0 && rayGap++) {
			if (counter > 0 && rayGap++ > eventRayGap.get()) {
				hit /= counter;        // 점만큼 나눈 각도의 좌표 (평균값.. )
				//std::cout<< hit << " , " << counter << " , " << rayGap << endl;
		#if 0
				float maxX = 1440.0;
				float minX = -1440.0;
				float maxY = 0;
				float minY = -1200;
				float SettingY = 2880.0;
				float offsetY = -980.0;
				/*
				screenWidth : 1920
				screenHeigth : 1200
				xfactor = screenWidth / SettingX;  // 0.666
				yfactor = screenHeigth / offsetY    //  0.779
				*/

				float screenX = (hit.x - minX) / (maxX - minX) * SettingY;
				screenXCal = screenX * 0.666;

				float SettingYoffset = hit.y - offsetY;
				screenYCal = (SettingYoffset * 0.779) * -1;

				//ofDrawCircle(screenXCal, screenYCal,32);
				//std::cout << screenXCal << " , " << screenYCal << endl;
		
				int xpos = (int)screenXCal + 0.5;
				int ypos = (int)screenYCal + 0.5;
				
				string message = ofToString(xpos) + "," + ofToString(ypos) + "\n";
				udpConnection.Send(message.c_str(), message.length());
				//std::cout << message << endl;
				message = "";
		#endif				
				int size = glm::distance(firsthit, lastthit);
				for (int e = 0; e < events.size(); e++) {
					if (events[e].isSame(hit)) {
						events[e].update(hit, size, smoothingPos.get(), smoothingSize.get());
						counter = 0;
						hit = glm::vec2();
						break;
					}
				}
				if (counter > 0) {
					// no previous event was found, so create a new one.
					events.push_back(Event(minID, hit, size, eventSize.get(), eventBreathSize.get()));
				}
				counter = 0;
				rayGap = 0;
				hit = glm::vec2();
			}
		}
	}

	// we clean up all the events
	for (int e = events.size() - 1; e >= 0; e--) {
		if (events[e].cleanup()) {
			events.erase(events.begin() + e);
		}
	}

	return (events.size() > 0) ? true : false;
}

void sensor::SensorField::broadcastEvents(ofxOscSender sender, int frameNumber)
{
	ofxOscMessage sensorbox;
	sensorbox.setAddress("/sensorfield/frame/start");
	sensorbox.addIntArg(fieldID.get());
	sensorbox.addIntArg(frameNumber);
	sender.sendMessage(sensorbox);

	for (int e = 0; e < events.size(); e++) {
		if (!events[e].isDying()) {
			sensorbox.clear();
			sensorbox.setAddress("/sensorfield/event");
			sensorbox.addIntArg(fieldID.get());
			sensorbox.addIntArg(events[e].getID());
			sensorbox.addIntArg(events[e].getElapsedMillis());
			sensorbox.addIntArg(events[e].getCenter().x);
			sensorbox.addIntArg(events[e].getCenter().y);
			sensorbox.addIntArg(events[e].getSize());

			sender.sendMessage(sensorbox);
		}
		else {
			sensorbox.clear();
			sensorbox.setAddress("/sensorfield/event/end");
			sensorbox.addIntArg(fieldID.get());
			sensorbox.addIntArg(events[e].getID());
			sensorbox.addIntArg(events[e].getElapsedMillis());

			sender.sendMessage(sensorbox);
		}
	}
	sensorbox.clear();
	sensorbox.setAddress("/sensorfield/frame/end");
	sensorbox.addIntArg(fieldID.get());
	sensorbox.addIntArg(frameNumber);
	sender.sendMessage(sensorbox);
}

void sensor::SensorField::broadcastBox(ofxOscSender sender)
{
	ofxOscMessage sensorbox;
	sensorbox.setAddress("/sensorfield/box");
	sensorbox.addIntArg(fieldID.get());
	sensorbox.addIntArg(limitLeft.get());
	sensorbox.addIntArg(limitRight.get());
	sensorbox.addIntArg(limitUp.get());
	sensorbox.addIntArg(limitDown.get());

	sender.sendMessage(sensorbox);
}

void sensor::SensorField::drawGui()
{
	//panel->setHidden(false);
	panel.draw();
}

void sensor::SensorField::drawField()
{
	ofSetColor(255, 0, 0);
	ofDrawLine(limitLeft.get(), limitUp.get(), limitRight.get(), limitUp.get());
	ofDrawLine(limitLeft.get(), limitDown.get(), limitRight.get(), limitDown.get());
	ofDrawLine(limitLeft.get(), limitDown.get(), limitLeft.get(), limitUp.get());
	ofDrawLine(limitRight.get(), limitDown.get(), limitRight.get(), limitUp.get());
}

void sensor::SensorField::drawEvents()
{
	ofNoFill();
	for (int e = 0; e < events.size(); e++) {
		ofSetColor(255. * events[e].isDying(), 0, 0);
		events[e].draw();
	}
}

void sensor::SensorField::drawEventLabels()
{
	float maxX = 1440.0;
	float minX = -1440.0;
	float maxY = 0;
	float minY = -1200;
	float SettingX = 2880.0;
	
	//float offsetY= -920.0;     // Y 축의 해상도 반전을 위한 변수
	float offsetY = -2500.0;

	float screenX[100];
	float screenY[100];
	int Xpos[100] = { 0 ,};
	int Ypos[100] = { 0 ,};
	float SettingYoffset[100];
	//string xymsg;
	string strsum[100];
	const char* xymsg1[100];
	int count = 0;
	float xhit = 0;
	float yhit = 0;
	
	
	std::vector<std::string> list;
	
	for (int e = 0; e < events.size(); e++) { 
		int eventsize = events.size();

		xhit = events[e].getCenter().x;
		yhit = events[e].getCenter().y;

		screenX[e] = (xhit - minX) / (maxX - minX) * SettingX;
		screenXCal[e] = screenX[e] * 0.666;  // factor = screen width / sensor width

		SettingYoffset[e] = yhit - offsetY;
		
		
		// Y축의 해상도 반전을 위해서는 -1
		//screenYCal[e] = (SettingYoffset[e] * 0.759) * -1;
		screenYCal[e] = (SettingYoffset[e] * 0.759);           // Lower limit - upper limit = value    screen height / value = factor   

		Xpos[e] = (int)screenXCal[e];
		Ypos[e] = (int)screenYCal[e];

		//std::cout << Xpos[e] << " , " << Ypos[e] << endl;

		string message = ofToString(Xpos[e]) + "," + ofToString(Ypos[e]) + "\n";
		udpConnection.Send(message.c_str(), message.length());
		message = "";
		Sleep(12.0);

#if 0	
		screenX[e] = (xhit - minX) / (maxX - minX) * SettingX;
		screenXCal[e] = screenX[e] * 0.666;  // factor = screen width / sensor width
		
		SettingYoffset[e] = yhit - offsetY;
		screenYCal[e] = (SettingYoffset[e] * 0.779) * -1;           // Lower limit - upper limit = value    screen height / value = factor   
		
		Xpos[e] = (int)screenXCal[e];
		Ypos[e] = (int)screenYCal[e];

		
		//std::cout << Xpos[e] << " , " << Ypos[e] << endl;
		
		
		string message = ofToString(Xpos[e]) + "," + ofToString(Ypos[e]) + "\n";
		udpConnection.Send(message.c_str(), message.length());
		message = "";
		Sleep(12.0);
		
#if 0
		strsum[e] = ofToString(Xpos[e]) + ":" + ofToString(Ypos[e]);
		list.push_back(strsum[e]);
		std::string delimiter = ",";

		std::string resultStr;
		std::for_each(list.begin(), list.end(), [&resultStr, &delimiter](const std::string & elem) {
			if (resultStr.empty())
				resultStr += elem;
			else
				resultStr += delimiter + elem;
		});
		if (e == eventsize - 1) {
			string msg = resultStr + "\n";
			udpConnection.Send(msg.c_str(), msg.length());
			resultStr = "";
			msg = "";
		}
#endif
#endif		
#if 0
		int m = sizeof(Xpos) / sizeof(Xpos[0]);
		int n = sizeof(Ypos) / sizeof(Ypos[0]);

		int size_t = m + n;
		std::cout << size_t << endl;
		
		int result[200];

		for (int i = 0; i < m + n; i++) {
			if (i < m) {
				result[i] = Xpos[i];
			}
			else {
				result[i] = Xpos[i];
			}
		}
		
		std::string str;
		for (int i : Xpos) {
			str += std::to_string(i) + ",";
		}
		std::cout << str << endl;
#endif
	}
}

void sensor::SensorField::save()
{
	panel.saveToFile("sensor_0.xml");
}

