#include "ofApp.h"

#define kMTCFrames      0
#define kMTCSeconds     1
#define kMTCMinutes     2
#define kMTCHours       3

//--------------------------------------------------------------
void ofApp::setup(){
	ofSetBackgroundColor(0);
	_xml.load("settings.xml");
	_oscIp = _xml.getValue("settings:oscip", "");
	_oscPort = _xml.getValue("settings:oscport", 0);
	_midiIdx = _xml.getValue("settings:midiid", 0);

	_sender.setup(_oscIp, _oscPort);
	_midiIn.listPorts();

	_midiIn.openPort(_midiIdx);
	_midiIn.ignoreTypes(false, false, false);
	_midiIn.addListener(this);
	_midiIn.setVerbose(true);

}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

	ofDrawBitmapString(ofToString(hours) + ":" + ofToString(minutes) + ":" + ofToString(seconds) + ";" + ofToString(frames), 10.0f, 20.0f);
	ofDrawBitmapString("midi: " + ofToString(_midiIdx) + " [" + _midiIn.getPortName(_midiIdx) + "]", 10.0f, 30.0f);
	ofDrawBitmapString("osc: " + _oscIp + ":" + ofToString(_oscPort), 10.0f, 60.0f);
}
//--------------------------------------------------------------
void ofApp::newMidiMessage(ofxMidiMessage& msg) {

	if (msg.status == MIDI_TIME_CODE &&		// if this is a MTC message...
		msg.bytes.size() > 1)
	{

		// these static variables could be globals, or class properties etc.
		static int times[4] = { 0, 0, 0, 0 };                 // this static buffer will hold our 4 time componens (frames, seconds, minutes, hours)
															  //static char *szType     = "";                           // SMPTE type as string (24fps, 25fps, 30fps drop-frame, 30fps)
		static int numFrames = 100;                          // number of frames per second (start off with arbitrary high number until we receive it)

		int messageIndex = msg.bytes.at(1) >> 4;       // the high nibble: which quarter message is this (0...7).
		int value = msg.bytes.at(1) & 0x0F;     // the low nibble: value
		int timeIndex = messageIndex >> 1;              // which time component (frames, seconds, minutes or hours) is this
		bool bNewFrame = messageIndex % 4 == 0;


		// the time encoded in the MTC is 1 frame behind by the time we have received a new frame, so adjust accordingly
		if (bNewFrame) {
			times[kMTCFrames]++;
			if (times[kMTCFrames] >= numFrames) {
				times[kMTCFrames] %= numFrames;
				times[kMTCSeconds]++;
				if (times[kMTCSeconds] >= 60) {
					times[kMTCSeconds] %= 60;
					times[kMTCMinutes]++;
					if (times[kMTCMinutes] >= 60) {
						times[kMTCMinutes] %= 60;
						times[kMTCHours]++;
					}
				}
			}

			//printf("%i:%i:%i:%i | %s\n", times[3], times[2], times[1], times[0], szType);

			hours = times[3];
			minutes = times[2];
			seconds = times[1];
			frames = times[0];

//			secondFraction = (float)timcodeEventArgs.frames / (float)timcodeEventArgs.numFrames;

//			timcodeEventArgs.timeAsMillis = timeToMillis(timcodeEventArgs);

//			ofNotifyEvent(MTCEvent, timcodeEventArgs, this);

//			sprintf( reportString, "%i:%i:%i:%i | %s\n", times[3], times[2], times[1], times[0], szType );		

			ofxOscBundle bundle;
			ofxOscMessage mes;
			mes.setAddress("/mtc");
			mes.addIntArg(hours);
			mes.addIntArg(minutes);
			mes.addIntArg(seconds);
			mes.addIntArg(frames);
			_sender.sendMessage(mes);
		}

		if (timeIndex < 0 || timeIndex > 3)
		{
			ofLogError() << "ofxMTCReceiver::newMidiMessage,  timeIndex should not be able to reach " << timeIndex;
			timeIndex %= 3;
		}

		if (messageIndex % 2 == 0) {                             // if this is lower nibble of time component
			times[timeIndex] = value;
		}
		else {                                                // ... or higher nibble
			times[timeIndex] |= value << 4;
		}


		if (messageIndex == 7) {
			times[kMTCHours] &= 0x1F;                               // only use lower 5 bits for hours (higher bits indicate SMPTE type)
			int smpteType = value >> 1;
			switch (smpteType) {
			case 0: numFrames = 24; /*szType = "24 fps";*/ break;
			case 1: numFrames = 25; /*szType = "25 fps";*/ break;
			case 2: numFrames = 30; /*szType = "30 fps (drop-frame)";*/ break;
			case 3: numFrames = 30; /*szType = "30 fps";*/ break;
			default: numFrames = 100; /*szType = " **** unknown SMPTE type ****";8*/
			}

//			timcodeEventArgs.numFrames = numFrames;
		}
	}
}

//--------------------------------------------------------------
void ofApp::exit() {

	// clean up
	_midiIn.closePort();
	_midiIn.removeListener(this);
}
//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
