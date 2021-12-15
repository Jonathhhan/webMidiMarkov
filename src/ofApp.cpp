/*
 * Copyright (c) 2011 Dan Wilcox <danomatika@gmail.com>
 *
 * BSD Simplified License.
 * For information on usage and redistribution, and for a DISCLAIMER OF ALL
 * WARRANTIES, see the file, "LICENSE.txt," in this distribution.
 *
 * See https://github.com/danomatika/ofxPd for documentation
 *
 */
#include "ofApp.h"
#include "emscripten.h"
#include "emscripten/bind.h"

using namespace emscripten;

ofEvent<std::string> midiChangedEvent; 	

void midiIn(std::string ev) {
	midiChangedEvent.notify(ev); 
}

EMSCRIPTEN_BINDINGS(Module) {
	emscripten::function("midiIn", &midiIn);
}

void ofApp::midiChanged(std::string & ev) {
	unsigned char* uEv = (unsigned char*)ev.c_str();
	int inChannel = uEv[0];
	int inPitch = uEv[1];
	int inVelocity = uEv[2];
	pd.startMessage();
	pd.addSymbol("midiIn");
	pd.addFloat(inChannel);
	pd.addFloat(inPitch);
	pd.addFloat(inVelocity);
	pd.finishList(patch.dollarZeroStr()+"-fromOF");
	label_10.symbol = "Midi in: " + ofToString(inChannel) + " " + ofToString(inPitch) + " " + ofToString(inVelocity);
}

//--------------------------------------------------------------
void ofApp::toggle_1onMousePressed(bool & e){
	pd.startMessage();
	pd.addSymbol("Play");
	pd.addFloat(e);
	pd.finishList(patch.dollarZeroStr()+"-fromOF");
}

//--------------------------------------------------------------
void ofApp::number_1onMousePressed(float & e){
	pd.startMessage();
	pd.addSymbol("BPM");
	pd.addFloat(e);
	pd.finishList(patch.dollarZeroStr()+"-fromOF");
}

//--------------------------------------------------------------
void ofApp::number_2onMousePressed(float & e){
	pd.startMessage();
	pd.addSymbol("order");
	pd.addFloat(e);
	pd.finishList(patch.dollarZeroStr()+"-fromOF");
}

//--------------------------------------------------------------
void ofApp::number_3onMousePressed(float & e){
	pd.startMessage();
	pd.addSymbol("noteLength");
	pd.addFloat(e);
	pd.finishList(patch.dollarZeroStr()+"-fromOF");
}

//--------------------------------------------------------------
void ofApp::number_4onMousePressed(float & e){
	pd.startMessage();
	pd.addSymbol("pc");
	pd.addFloat(e);
	pd.finishList(patch.dollarZeroStr()+"-fromOF");
}
//--------------------------------------------------------------
void ofApp::bang_1onMousePressed(bool & e){
	pd.startMessage();
	pd.addSymbol("make");
	pd.addFloat(e);
	pd.finishList(patch.dollarZeroStr()+"-fromOF");
}

//--------------------------------------------------------------
void ofApp::bang_2onMousePressed(bool & e){
	pd.startMessage();
	pd.addSymbol("reset");
	pd.addFloat(e);
	pd.finishList(patch.dollarZeroStr()+"-fromOF");
}

//--------------------------------------------------------------
void ofApp::bang_3onMousePressed(bool & e){
	pd.startMessage();
	pd.addSymbol("random");
	pd.addFloat(e);
	pd.finishList(patch.dollarZeroStr()+"-fromOF");
}

//--------------------------------------------------------------
void ofApp::bang_4onMousePressed(bool & e){
	pd.startMessage();
	pd.addSymbol("clear");
	pd.addFloat(e);
	pd.finishList(patch.dollarZeroStr()+"-fromOF");
}
//--------------------------------------------------------------
void ofApp::setup() {
	//ofAddListener(hSlider_1.onMousePressed, this, &ofApp::hSlider_1onMousePressed);
	ofAddListener(toggle_1.onMousePressed, this, &ofApp::toggle_1onMousePressed);
	ofAddListener(number_1.onMousePressed, this, &ofApp::number_1onMousePressed);
	ofAddListener(number_2.onMousePressed, this, &ofApp::number_2onMousePressed);
	ofAddListener(number_3.onMousePressed, this, &ofApp::number_3onMousePressed);
	ofAddListener(number_4.onMousePressed, this, &ofApp::number_4onMousePressed);
	ofAddListener(bang_1.onMousePressed, this, &ofApp::bang_1onMousePressed);
	ofAddListener(bang_2.onMousePressed, this, &ofApp::bang_2onMousePressed);
	ofAddListener(bang_3.onMousePressed, this, &ofApp::bang_3onMousePressed);
	ofAddListener(bang_4.onMousePressed, this, &ofApp::bang_4onMousePressed);
	ofAddListener(midiChangedEvent, this, &ofApp::midiChanged);
	label_1.setup(20, 20, 680, 50, "Markov Generator");
	label_2.setup(120, 80, 200, 20, "Play");
	label_3.setup(120, 120, 200, 20, "Beats per Minute");
	label_4.setup(500, 120, 200, 20, "Markov order");
	label_5.setup(500, 160, 200, 20, "Create the chain");
	label_6.setup(500, 200, 200, 20, "Start position");
	label_7.setup(500, 240, 200, 20, "Random position");
	label_8.setup(500, 280, 200, 20, "Reset");
	label_9.setup(120, 160, 200, 20, "Note length");
	label_10.setup(20, 240, 300, 20, "Midi in: 0 0 0");
	label_11.setup(20, 280, 300, 20, "Midi out: 0 0 0");
	label_12.setup(120, 200, 200, 20, "Program change");
	toggle_1.setup(20, 80, 20);
	bang_1.setup(400, 160, 20);
	bang_2.setup(400, 200, 20);
	bang_3.setup(400, 240, 20);
	bang_4.setup(400, 280, 20);
	number_1.setup(20, 120, 80, 20, 10, 500);
	number_1.value = 120;
	number_2.setup(400, 120, 80, 20, 1, 100);
	number_2.value = 2;
	number_3.setup(20, 160, 80, 20, 50, 2000);
	number_3.value = 500;
	number_4.setup(20, 200, 80, 20, 0, 127);
	number_4.value = 17;
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	//ofSetLogLevel("Pd", OF_LOG_VERBOSE); // see verbose info inside

	// double check where we are ...
	cout << ofFilePath::getCurrentWorkingDirectory() << endl;

	// the number of libpd ticks per buffer,
	// used to compute the audio buffer len: tpb * blocksize (always 64)
	#ifdef TARGET_LINUX_ARM
		// longer latency for Raspberry PI
		int ticksPerBuffer = 32; // 32 * 64 = buffer len of 2048
		int numInputs = 0; // no built in mic
	#else
		int ticksPerBuffer = 8; // 8 * 64 = buffer len of 512
		int numInputs = 0;
	#endif

	// setup OF sound stream
	ofSoundStreamSettings settings;
	settings.numInputChannels = 0;
	settings.numOutputChannels = 2;
	settings.sampleRate = 44100;
	settings.bufferSize = ofxPd::blockSize() * ticksPerBuffer;
	settings.setInListener(this);
	settings.setOutListener(this);
	ofSoundStreamSetup(settings);

	// setup Pd
	//
	// set 4th arg to true for queued message passing using an internal ringbuffer,
	// this is useful if you need to control where and when the message callbacks
	// happen (ie. within a GUI thread)
	//
	// note: you won't see any message prints until update() is called since
	// the queued messages are processed there, this is normal
	//
	if(!pd.init(2, numInputs, 44100, ticksPerBuffer, false)) {
		OF_EXIT_APP(1);
	}

	midiChan = 1; // midi channels are 1-16

	// subscribe to receive source names
	pd.subscribe("toOF");
	pd.subscribe("env");

	// add message receiver, required if you want to recieve messages
	pd.addReceiver(*this); // automatically receives from all subscribed sources
	pd.ignoreSource(*this, "env");        // don't receive from "env"
	//pd.ignoreSource(*this);             // ignore all sources
	//pd.receiveSource(*this, "toOF");	  // receive only from "toOF"

	// add midi receiver, required if you want to recieve midi messages
	pd.addMidiReceiver(*this); // automatically receives from all channels
	//pd.ignoreMidiChannel(*this, 1);     // ignore midi channel 1
	//pd.ignoreMidiChannel(*this);        // ignore all channels
	//pd.receiveMidiChannel(*this, 1);    // receive only from channel 1

	// add the data/pd folder to the search path
	//pd.addToSearchPath("pd/abs");

	// audio processing on
	pd.start();

	// -----------------------------------------------------
	//cout << endl << "BEGIN Patch Test" << endl;
	// open patch
	patch = pd.openPatch("pd/test.pd");
	cout << patch << endl;
	//cout << "FINISH Patch Test" << endl;
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {
	label_1.draw();
	label_2.draw();
	label_3.draw();
	label_4.draw();
	label_5.draw();
	label_6.draw();
	label_7.draw();
	label_8.draw();
	label_9.draw();
	label_10.draw();
	label_11.draw();
	label_12.draw();
	number_1.draw();
	number_2.draw();
	number_3.draw();
	number_4.draw();
	toggle_1.draw();
	bang_1.draw();
	bang_2.draw();
	bang_3.draw();
	bang_4.draw();
}

//--------------------------------------------------------------
void ofApp::exit() {

	// cleanup
	ofSoundStreamStop();
}

//--------------------------------------------------------------
void ofApp::playTone(int pitch) {
	pd << StartMessage() << "pitch" << pitch << FinishList("tone") << Bang("tone");
}

//--------------------------------------------------------------
void ofApp::keyPressed (int key) {

	switch(key) {
		
		// musical keyboard
		case 'a':
			playTone(60);
			break;
		case 'w':
			playTone(61);
			break;
		case 's':
			playTone(62);
			break;
		case 'e':
			playTone(63);
			break;
		case 'd':
			playTone(64);
			break;
		case 'f':
			playTone(65);
			break;
		case 't':
			playTone(66);
			break;
		case 'g':
			playTone(67);
			break;
		case 'y':
			playTone(68);
			break;
		case 'h':
			playTone(69);
			break;
		case 'u':
			playTone(70);
			break;
		case 'j':
			playTone(71);
			break;
		case 'k':
			playTone(72);
			break;

		case ' ':
			if(pd.isReceivingSource(*this, "env")) {
				pd.ignoreSource(*this, "env");
				cout << "ignoring env" << endl;
			}
			else {
				pd.receiveSource(*this, "env");
				cout << "receiving from env" << endl;
			}
			break;

		default:
			break;
	}
}

//--------------------------------------------------------------
void ofApp::audioReceived(float * input, int bufferSize, int nChannels) {
	pd.audioIn(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::audioRequested(float * output, int bufferSize, int nChannels) {
	pd.audioOut(output, bufferSize, nChannels);
}

//--------------------------------------------------------------
void ofApp::print(const std::string &message) {
	cout << message << endl;
}

//--------------------------------------------------------------
void ofApp::receiveBang(const std::string &dest) {
	cout << "OF: bang " << dest << endl;
}

void ofApp::receiveFloat(const std::string &dest, float value) {
	cout << "OF: float " << dest << ": " << value << endl;
}

void ofApp::receiveSymbol(const std::string &dest, const std::string &symbol) {
	cout << "OF: symbol " << dest << ": " << symbol << endl;
}

void ofApp::receiveList(const std::string &dest, const pd::List &list) {
	// cout << "OF: list " << dest << ": ";
	if (list.getSymbol(0) == "midiOut:"){
	int outChannel = list.getFloat(1);
	int outPitch = list.getFloat(2);
	int outVelocity = list.getFloat(3);
	int array[3] = {outChannel, outPitch, outVelocity};
	size_t lengthOfArray = sizeof array / sizeof array[0];
	EM_ASM_(
	var data = new Uint32Array(HEAPU32.buffer, $0, $1);
	sendMIDI(data), array, lengthOfArray);
	label_11.symbol = "Midi out: " + ofToString(outChannel) + " " + ofToString(outPitch) + " " + ofToString(outVelocity);
	}
	if (list.getSymbol(0) == "ct:"){
	int outChannel = list.getFloat(1);
	int outPitch = list.getFloat(2);
	int array[2] = {outChannel, outPitch};
	size_t lengthOfArray = sizeof array / sizeof array[0];
	EM_ASM_(
	var data = new Uint32Array(HEAPU32.buffer, $0, $1);
	sendMIDI(data), array, lengthOfArray);
	label_11.symbol = "Control change: " + ofToString(outChannel) + " " + ofToString(outPitch);
	}
	// cout << list.toString() << endl;
}

void ofApp::receiveMessage(const std::string&dest, const std::string &msg, const pd::List &list) {
	cout << "OF: message " << dest << ": " << msg << " " << list.toString() << list.types() << endl;
}

//--------------------------------------------------------------
void ofApp::receiveNoteOn(const int channel, const int pitch, const int velocity) {
	cout << "OF MIDI: note on: " << channel << " " << pitch << " " << velocity << endl;
}

void ofApp::receiveControlChange(const int channel, const int controller, const int value) {
	cout << "OF MIDI: control change: " << channel << " " << controller << " " << value << endl;
}

// note: pgm nums are 1-128 to match pd
void ofApp::receiveProgramChange(const int channel, const int value) {
	cout << "OF MIDI: program change: " << channel << " " << value << endl;
}

void ofApp::receivePitchBend(const int channel, const int value) {
	cout << "OF MIDI: pitch bend: " << channel << " " << value << endl;
}

void ofApp::receiveAftertouch(const int channel, const int value) {
	cout << "OF MIDI: aftertouch: " << channel << " " << value << endl;
}

void ofApp::receivePolyAftertouch(const int channel, const int pitch, const int value) {
	cout << "OF MIDI: poly aftertouch: " << channel << " " << pitch << " " << value << endl;
}

// note: pd adds +2 to the port num, so sending to port 3 in pd to [midiout],
//       shows up at port 1 in ofxPd
void ofApp::receiveMidiByte(const int port, const int byte) {
	cout << "OF MIDI: midi byte: " << port << " " << byte << endl;
}