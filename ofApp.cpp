#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(239);
	ofSetLineWidth(3);

	this->fbo.allocate(ofGetWidth() + 25, ofGetHeight() + 25);
	this->fbo.readToPixels(this->pixels);
	// Link pixels And pixels_mat
	this->pixels_mat = cv::Mat(this->pixels.getHeight(), this->pixels.getWidth(), CV_8UC4, this->pixels.getData());
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);
	ofSetColor(39);

	this->fbo.begin();
	ofClear(0);
	ofTranslate(this->fbo.getWidth() * 0.5, this->fbo.getHeight() * 0.5);

	auto radius_base = 250;
	auto len = 125;
	auto deg_span = 5;
	for (auto deg = 0; deg < 360; deg += deg_span) {

		auto noise_point = glm::vec2(radius_base * cos(deg * DEG_TO_RAD), radius_base * sin(deg * DEG_TO_RAD));
		auto noise_value = ofNoise(noise_point.x * 0.01, noise_point.y * 0.01, ofGetFrameNum() * 0.01);
		auto radius = radius_base;
		if (noise_value > 0.7) { radius += ofMap(noise_value, 0.7, 1.0, 0, radius_base * 0.5); }
		if (noise_value < 0.3) { radius -= ofMap(noise_value, 0.0, 0.3, radius_base * 0.5, 0); }

		ofBeginShape();
		ofVertex(glm::vec2(radius * cos((deg - deg_span * 0.45)  * DEG_TO_RAD), radius * sin((deg - deg_span * 0.45) * DEG_TO_RAD)));
		ofVertex(glm::vec2(radius * cos((deg + deg_span * 0.45)  * DEG_TO_RAD), radius * sin((deg + deg_span * 0.45) * DEG_TO_RAD)));
		ofVertex(glm::vec2((radius - len) * cos((deg + deg_span * 0.45)  * DEG_TO_RAD), (radius - len) * sin((deg + deg_span * 0.45) * DEG_TO_RAD)));
		ofVertex(glm::vec2((radius - len) * cos((deg - deg_span * 0.45)  * DEG_TO_RAD), (radius - len) * sin((deg - deg_span * 0.45) * DEG_TO_RAD)));
		ofEndShape(true);
	}

	this->fbo.end();
	// Copy Fbo To pixels
	this->fbo.readToPixels(this->pixels);

	// Gray
	cv::Mat gray_mat;
	cv::cvtColor(this->pixels_mat, gray_mat, cv::COLOR_RGBA2GRAY);
	for (int y = 0; y < this->pixels_mat.cols; y++) {

		for (int x = 0; x < this->pixels_mat.rows; x++) {

			if (gray_mat.at<unsigned char>(y, x) > 0) {

				this->pixels_mat.at<cv::Vec4b>(y, x) = cv::Vec4b(gray_mat.at<unsigned char>(y, x), gray_mat.at<unsigned char>(y, x), gray_mat.at<unsigned char>(y, x), this->pixels_mat.at<cv::Vec4b>(y, x)[3]);
			}
		}
	}

	// Blur Process
	cv::GaussianBlur(this->pixels_mat, this->pixels_mat, cv::Size(19, 19), 10, 10);
}

//--------------------------------------------------------------
void ofApp::draw() {

	// Draw Shadow
	ofSetColor(255, 192);
	ofImage draw_image;
	draw_image.setFromPixels(this->pixels);
	draw_image.draw(-5, -5);

	// Draw Fbo Image 
	ofSetColor(255);
	this->fbo.draw(-25, -25);
}

//--------------------------------------------------------------
int main() {

	ofSetupOpenGL(720, 720, OF_WINDOW);
	ofRunApp(new ofApp());
}