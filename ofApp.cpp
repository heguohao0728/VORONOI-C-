#include "ofApp.h"


//--------------------------------------------------------------

void ofApp::setup(){
	//while(1){
	ofSetWindowTitle("Voronoi_hunting");
	ofBackground(255);

	ofRectangle bounds = ofRectangle(10, 10, ofGetWidth() - 20, ofGetHeight() - 20);

	//随机点
	int agentTotal = 12;
	int seed;
	std::cout << "请输入1到50的随机整数用于生成随机agent" << endl;
	std::cin >> seed;
	std::cout << typeid(seed).name() << endl;
	while (seed>50) {
		std::cout << "请重新输入1到50的随机整数:";
		std::cin >> seed;
	}

	ofSeedRandom(seed);
	for (int i = 0; i < agentTotal; i++) {
		ofDefaultVec3 newPoint = ofDefaultVec3(ofRandom(bounds.x, bounds.width), ofRandom(bounds.y, bounds.height), 0);
		points.push_back(newPoint);
	}
	
	//ofApp::init_Voro(bounds, points);
	//ofApp::generateVoro();
	ofApp::update(bounds, points);
	//ofApp::draw();
	points.clear();
	cout << "programe reached here" << endl;
	for (auto cell : voronoi.getCells()) {
		points.push_back(cell.centroid);
	}
	//}
}

//--------------------------------------------------------------
//void ofApp::init_Voro(ofRectangle bounds,vector<ofDefaultVec3> points) {
//	//维诺图初始化
//    voronoi.setBounds(bounds);
//	voronoi.setPoints(points);
//}

//--------------------------------------------------------------
//void ofApp::generateVoro() {
//	//维诺图生成
//	voronoi.generate();
//}

//--------------------------------------------------------------
void ofApp::update(ofRectangle bounds, vector<ofDefaultVec3> points){
	voronoi.setBounds(bounds);
	voronoi.setPoints(points);
	voronoi.generate(); 
}

//--------------------------------------------------------------
void ofApp::update() {
	cout << "update22222" << endl;
	voronoi.cal_neighbor();
	voronoi.cal_up();
	voronoi.update_point();
	points = voronoi.getPoints();
	voronoi.setPoints(points);
	voronoi.generate();
	voronoi.catchornot();
}

//--------------------------------------------------------------
void ofApp::draw(){
	cout << "print!!!" << endl;
	voronoi.draw();
}

//--------------------------------------------------------------
bool ofApp::isBorder(ofDefaultVec3 _pt) {
	ofRectangle bounds = voronoi.getBounds();

	return (_pt.x == bounds.x || _pt.x == bounds.x + bounds.width
		|| _pt.y == bounds.y || _pt.y == bounds.y + bounds.height);
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
