#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    ofSetVerticalSync(true);
    ofSetDepthTest(true);
    // mesh.load("lofi-bunny.ply");

	sphere.set(200.0, 3);

    // shape file
    bValidate     = 0;
    nInvalidCount = 0;
    bHeaderOnly   = 0;
    string fname = "data/ne_110m_admin_0_countries";
    hSHP = SHPOpen( fname.c_str(), "rb" );
    if( hSHP == NULL ) {
        printf( "Unable to open:%s\n", fname.c_str() );
        ofExit(1);
    }

    SHPGetInfo( hSHP, &nEntities, &nShapeType, adfMinBound, adfMaxBound );

    printf( "Shapefile Type: %s   # of Shapes: %d\n\n",
            SHPTypeName( nShapeType ), nEntities );

    printf( "File Bounds: (%.15g,%.15g,%.15g,%.15g)\n"
            "         to  (%.15g,%.15g,%.15g,%.15g)\n",
            adfMinBound[0],
            adfMinBound[1],
            adfMinBound[2],
            adfMinBound[3],
            adfMaxBound[0],
            adfMaxBound[1],
            adfMaxBound[2],
            adfMaxBound[3] );

    for( i = 0; i < nEntities && !bHeaderOnly; i++ ) {

        int		    j;
        SHPObject	*psShape;

        psShape = SHPReadObject( hSHP, i );

        if( psShape == NULL ) {
            fprintf( stderr,
                     "Unable to read shape %d, terminating object reading.\n",
                    i );
            break;
        }



        if( psShape->bMeasureIsUsed ) {
            /*
            printf( "\nShape:%d (%s)  nVertices=%d, nParts=%d\n"
                    "  Bounds:(%.15g,%.15g, %.15g, %.15g)\n"
                    "      to (%.15g,%.15g, %.15g, %.15g)\n",
                    i, SHPTypeName(psShape->nSHPType),
                    psShape->nVertices, psShape->nParts,
                    psShape->dfXMin, psShape->dfYMin,
                    psShape->dfZMin, psShape->dfMMin,
                    psShape->dfXMax, psShape->dfYMax,
                    psShape->dfZMax, psShape->dfMMax );
            */
        } else {
            /*
            printf( "\nShape:%d (%s)  nVertices=%d, nParts=%d\n"
                    "  Bounds:(%.15g,%.15g, %.15g)\n"
                    "      to (%.15g,%.15g, %.15g)\n",
                    i, SHPTypeName(psShape->nSHPType),
                    psShape->nVertices, psShape->nParts,
                    psShape->dfXMin, psShape->dfYMin,
                    psShape->dfZMin,
                    psShape->dfXMax, psShape->dfYMax,
                    psShape->dfZMax );
            */
        }

        if( psShape->nParts > 0 && psShape->panPartStart[0] != 0 ) {
            fprintf( stderr, "panPartStart[0] = %d, not zero as expected.\n",
                     psShape->panPartStart[0] );
        }

		ofPath* path = new ofPath();
		//path->setFilled(false);
		path->setFillColor(ofColor(rand()/2.0+0.5, rand()/2.0+0.5, rand()/2.0+0.5, 224));
		int oldiPart=-1;
        for( j = 0, iPart = 1; j < psShape->nVertices; j++ ) {
            const char	*pszPartType = "";

            if( j == 0 && psShape->nParts > 0 )
                pszPartType = SHPPartTypeName( psShape->panPartType[0] );

            if( iPart < psShape->nParts
                && psShape->panPartStart[iPart] == j ) {
                pszPartType = SHPPartTypeName( psShape->panPartType[iPart] );
                iPart++;
                pszPlus = "+";
            }
            else pszPlus = " ";

            if( psShape->bMeasureIsUsed ) {
                /*
                printf("   %s (%.15g,%.15g, %.15g, %.15g) %s \n",
                       pszPlus,
                       psShape->padfX[j],
                       psShape->padfY[j],
                       psShape->padfZ[j],
                       psShape->padfM[j],
                       pszPartType );
                */
            } else {
				/*
                printf("   %s (%.15g,%.15g, %.15g) %s \n",
                       pszPlus,
                       psShape->padfX[j],
                       psShape->padfY[j],
                       psShape->padfZ[j],
                       pszPartType );
				*/
				float R = 200.0;
				float lng = psShape->padfX[j];
				float lat = psShape->padfY[j];
				float x = R*cos(lat*3.14159/180.0)*cos(lng*3.14159/180.0);
				float y = R*cos(lat*3.14159/180.0)*sin(lng*3.14159/180.0);
				float z = R*sin(lat*3.14159/180.0);
				if(oldiPart == iPart) path->lineTo(x,y,z); else path->moveTo(x,y,z);
				oldiPart = iPart;
            }
        }

		if( bValidate ) {
            int nAltered = SHPRewindObject( hSHP, psShape );

            if( nAltered > 0 )
            {
                printf( "  %d rings wound in the wrong direction.\n",
                        nAltered );
                nInvalidCount++;
            }
        }

        path->close();
        shapes.push_back(path);

        SHPDestroyObject( psShape );
    }

    SHPClose( hSHP );

    if( bValidate ) {
        printf( "%d object has invalid ring orderings.\n", nInvalidCount );
    }

    #ifdef USE_DBMALLOC
        malloc_dump(2);
    #endif
}

//--------------------------------------------------------------
void testApp::update()
{

}

//--------------------------------------------------------------
void testApp::draw()
{
    ofBackgroundGradient(ofColor(64), ofColor(0));

    ofSetColor(255);
    cam.begin();

    ofSetColor(255,255,255,20);
	sphere.drawWireframe();
    //mesh.drawWireframe();

    //glPointSize(2);
    //ofSetColor(ofColor::white);
    //mesh.drawVertices();

	for(vector< ofPath* >::iterator shape = shapes.begin(); shape != shapes.end(); ++shape) {
		(*shape)->draw();
	}
	cam.end();

	/*
    int n = mesh.getNumVertices();
    float nearestDistance = 0;
    ofVec2f nearestVertex;
    int nearestIndex = 0;
    ofVec2f mouse(mouseX, mouseY);
    for(int i = 0; i < n; i++)
    {
        ofVec3f cur = cam.worldToScreen(mesh.getVertex(i));
        float distance = cur.distance(mouse);
        if(i == 0 || distance < nearestDistance)
        {
            nearestDistance = distance;
            nearestVertex = cur;
            nearestIndex = i;
        }
    }

    ofSetColor(ofColor::gray);
    ofLine(nearestVertex, mouse);

    ofNoFill();
    ofSetColor(ofColor::yellow);
    ofSetLineWidth(2);
    ofCircle(nearestVertex, 4);
    ofSetLineWidth(1);

    ofVec2f offset(10, -10);
    ofDrawBitmapStringHighlight(ofToString(nearestIndex), mouse + offset);
	*/

}

//--------------------------------------------------------------
void testApp::keyPressed(int key)
{

}

//--------------------------------------------------------------
void testApp::keyReleased(int key)
{

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y)
{

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button)
{

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h)
{

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg)
{

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo)
{

}
