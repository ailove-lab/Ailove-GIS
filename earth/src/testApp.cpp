#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup()
{
    ofSetVerticalSync(true);
    ofSetDepthTest(true);

    pjFromStr = "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs";
    pjToStr   = "+proj=laea +lat_0=90 +lon_0=90 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs";
    // mesh.load("lofi-bunny.ply");

    // projections
    if (!(pjFrom = pj_init_plus(pjFromStr.c_str())) )
       ofExit(1);
    if (!(pjTo = pj_init_plus(pjToStr.c_str())) )
       ofExit(1);

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

    mbrXmin = adfMinBound[0];
    mbrYmin = adfMinBound[1];
    mbrXmax = adfMaxBound[0];
    mbrYmax = adfMaxBound[1];


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
		ofPath* projectedPath = new ofPath();
		//path->setFilled(false);
		projectedPath->setFilled(false);
		path->setFillColor(ofColor(rand()/2.0+0.5, rand()/2.0+0.5, rand()/2.0+0.5, 224));
        projectedPath->setFillColor(ofColor(rand()/2.0+0.5, rand()/2.0+0.5, rand()/2.0+0.5, 224));


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
				double lng = psShape->padfX[j]*DEG_TO_RAD;
				double lat = psShape->padfY[j]*DEG_TO_RAD;
				float x = R*cos(lat)*cos(lng);
				float y = R*cos(lat)*sin(lng);
				float z = R*sin(lat);

				int p = pj_transform(pjFrom, pjTo, 1, 1, &lng, &lat, NULL);

                if(mbrXmin>lng) mbrXmin = lng;
                if(mbrYmin>lat) mbrYmin = lat;
                if(mbrXmax<lng) mbrXmax = lng;
                if(mbrYmax<lat) mbrYmax = lat;

                if(p)
                    printf("%s\nlng: %f, lat: %f\n",pj_strerrno(p), lng, lat);
                //else
                //    printf("lng: %f, lat: %f\n", lng, lat);

				if(oldiPart == iPart) {
                    path->lineTo(x,y,z);
                    projectedPath->lineTo(lng, lat);
                } else {
                    path->moveTo(x,y,z);
                    projectedPath->moveTo(lng, lat);
                }

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
        projectedPath->close();

        shapes.push_back(path);
        projectedShapes.push_back(projectedPath);

        SHPDestroyObject( psShape );
    }

    printf("MBR\n   min: %f, %f;\n   max: %f, %f\n", mbrXmin, mbrYmin, mbrXmax, mbrYmax);

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

    ofDrawBitmapString(pjFromStr,10,20);
    ofDrawBitmapString(pjToStr  ,10,40);


	double s=-768.0/abs(mbrYmax-mbrYmin);
    ofTranslate(1024/2, 768/2, 0);
    ofScale(-s,s,1.0);

    for(vector< ofPath* >::iterator shape = projectedShapes.begin(); shape != projectedShapes.end(); ++shape) {
		(*shape)->draw();
	}

    cam.begin();

    ofSetColor(255,255,255,20);
	sphere.drawWireframe();
    //mesh.drawWireframe();

    //glPointSize(2);
    //ofSetColor(ofColor::white);
    //mesh.drawVertices();

	//for(vector< ofPath* >::iterator shape = shapes.begin(); shape != shapes.end(); ++shape) {
	//	(*shape)->draw();
	//}
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
