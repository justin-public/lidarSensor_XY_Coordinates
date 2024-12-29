#include "ofMain.h"
#include "ofxOsc.h"
//#include "ofxGuiExtended.h"
#include "ofxGui.h"
#include "extern.h"
#include "ofxNetwork.h"

#include <vector>
#include <iostream>

#pragma once
namespace sensor
{
	class Event
	{
	public:
		Event(int ID, glm::vec2 pos, int size, int generationSize, int lifeSpan);
		~Event();

		void	prepare();
		bool	isSame(glm::vec2 pos);
		void	update(glm::vec2 pos, int size, float smoothPos, float smoothSize);
		bool	cleanup();
		bool	isDying();
		int		getID();
		glm::vec2 getCenter();
		int		getSize();
		int		getElapsedMillis();

		void	 draw();

	protected:

		int mID;

		int mCountDown;
		int mBreathSize;
		bool mIsDying;

		int mLifeCycles;

		glm::vec2 mCenter;
		glm::vec2 mLastCenter;
		int mSize;
		int mGenerationSize;
	};

	class SensorField
	{
	public:
		SensorField();
		~SensorField();

		void setup(string name);
		bool update(std::vector<glm::vec3> data);
		void broadcastEvents(ofxOscSender sender, int frameNumber);
		void broadcastBox(ofxOscSender sender);

		void drawGui();
		void drawField();
		void drawEvents();
		void drawEventLabels();

		void save();

		ofxPanel panel;

	protected:
		ofTrueTypeFont myfont;

		vector<Event> events;

		ofxGuiGroup *IDGroup;
		ofxGuiGroup *fieldGroup;
		ofxGuiGroup *sensitivityGroup;

		ofxUDPManager udpConnection;

		ofParameter<int> fieldID;

		ofParameter<int> limitUp;
		ofParameter<int> limitDown;
		ofParameter<int> limitLeft;
		ofParameter<int> limitRight;

		ofParameter<int> eventSize;
		ofParameter<int> eventRayGap;
		ofParameter<int> eventBreathSize;
		ofParameter<float> smoothingPos;
		ofParameter<float> smoothingSize;

	};
}
