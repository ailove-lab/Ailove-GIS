#pragma once

#include "ofMain.h"
#include "shapefil.h"
#include "proj_api.h"

class testApp : public ofBaseApp
{
public:
    void setup();
    void update();
    void draw();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y);
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

    // shape file
    SHPHandle	hSHP;
    int		    nShapeType, nEntities, i, iPart, bValidate,nInvalidCount;
    int         bHeaderOnly;
    const char 	*pszPlus;
    double 	adfMinBound[4], adfMaxBound[4];
    vector<ofPath*> shapes;
    vector<ofPath*> projectedShapes;

	ofIcoSpherePrimitive sphere;

    ofMesh mesh;
    ofEasyCam cam;

    double mbrXmin;
    double mbrYmin;
    double mbrXmax;
    double mbrYmax;

    projPJ pjFrom, pjTo;

    string pjFromStr, pjToStr;

};
