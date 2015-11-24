#ifdef SINGLE
#define REAL float
#else /* not SINGLE */
#define REAL double
#endif /* not SINGLE */

#include <iostream.h>
#include <fstream.h>
#include <math.h>
#include "triangle.h"

#include <vector>

#define EPS ((float)1.0e-08)
#define LF 0x0A
#define CR 0x0D

struct triangulateio in, mid, out, vorout;

using namespace std;
vector <float*> pts;

float minZ[3],maxZ[3];
int numContours = 20;
float threshhold = 28;

float dist(float v1[3], float v2[3])
{
  float x = v2[0]-v1[0];
  float y = v2[1]-v1[1];
  return sqrt(x*x+y*y);
}

void showPt(::fstream& out,float* f)
{
  out << f[0]+1.0 << " " << f[1] << " m" << endl;
  out << f[0]-1.0 << " " << f[1] << " l c s" << endl;
  out << f[0] << " " << f[1]+1.0 << " m" << endl;
  out << f[0] << " " << f[1]-1.0 << " l c s" << endl;
}

void showElev(::fstream& out,float* f, float level)
{
  char buf[80];
  sprintf(buf,"%g", level);
  out << "(" << level << ") ";  
  out << f[0] << " ";
  out << "("<<level<<") stringwidth pop 2 div sub ";
  out << f[1] << " stringBox" << endl;
}

void showPts(::fstream& out)
{
  vector<float*>::iterator iter = pts.begin();
  for( ; iter != pts.end() ; ++iter ){
    float* f = *iter;  
    showPt(out,f);
  }
}

void rndr_indexed_tri_contour(void)
{
  ::fstream out;
  float xRange = maxZ[0]-minZ[0];
  float yRange = maxZ[1]-minZ[1];
  float xScale = (8.5*72.0)/xRange;
  float yScale = (11.0*72.0)/yRange;
  float theScale = (yScale>xScale)?xScale:yScale;
  out.open("contour.ps",ios::out);
  out << "%!" << endl;
  out << "/fontSize 6 def" << endl;
  out << "/down fontSize 12 div def" << endl;
  out << "/up   fontSize 0.7 mul def" << endl;
  out << "" << endl;
  out << "/stringBox {" << endl;
  out << " /y exch def" << endl;
  out << " /x exch def" << endl;
  out << " /str exch def" << endl;
  out << " str stringwidth" << endl;
  out << " /yw exch def" << endl;
  out << " /xw exch def" << endl;
  out << " gsave" << endl;
  out << " newpath" << endl;
  out << " x        y down sub moveto" << endl;
  out << " x        y up   add lineto" << endl;
  out << " x xw add y up   add lineto" << endl;
  out << " x xw add y down sub lineto" << endl;
  out << " x        y down sub lineto" << endl;
  out << " closepath" << endl;
  out << " 1.0 setgray fill" << endl;
  out << " stroke" << endl;
  out << " 0.0 setgray" << endl;
  out << " x y moveto " << endl;
  out << " str show" << endl;
  out << " grestore" << endl;
  out << "} def" << endl;

  out << "/Courier findfont fontSize scalefont setfont" << endl;
  out << "/n   { newpath } bind def" << endl;
  out << "/m   { moveto } bind def" << endl;
  out << "/l   { lineto } bind def" << endl;
  out << "/c   { closepath } bind def" << endl;
  out << "/s   { stroke } bind def" << endl;
  out << "/sw  { stringwidth } bind def" << endl;
  out << "0 setlinewidth 0 setgray 0 setlinejoin" << endl;
  out << "/pageX 8.5 72 mul def" << endl;
  out << "/pageY 11  72 mul def" << endl;
  out << "/xc pageX 2 div def" << endl;
  out << "/yc pageY 2 div def" << endl;
  out << "/midx " << (minZ[0]+maxZ[0])/2.0 << " def"<<endl;
  out << "/midy " << (minZ[1]+maxZ[1])/2.0 << " def"<<endl;
  out << "/theScale " << theScale << " def"<<endl;
  out << "xc yc translate" << endl;
  out << "theScale 0.9 mul theScale 0.9 mul scale" << endl;
  out << "midx -1 mul midy -1 mul translate" << endl;
  out << endl;

  int tInd,*iPtr = mid.trianglelist;
  int riMin,riMax,riMid;
  int cMinIndex;
  float t0,t1,fmin,fmax;
  float v1[3],v2[3],d,midPt[3];
  float cCur,cMin,cInc = (maxZ[2]-minZ[2])/numContours;
  float *src,*dst;
  
  /* loop over all triangles in this component */
  for( tInd=0 ; tInd<mid.numberoftriangles ; tInd++ , iPtr += 3)
  {
    // get the minmax across the tri
    
    riMin = riMax = 0; 
    //fmin = fmax = sPtr[ iPtr[0] ];

    fmin = fmax = pts[iPtr[0]][2];
    
    for(int i=1;i<=2;i++){
      if(fmin > pts[iPtr[i]][2] ){
	fmin = pts[iPtr[i]][2];
	riMin = i; // save index of the min vertex
      }
      if(fmax < pts[iPtr[i]][2] ){
	fmax = pts[iPtr[i]][2];
	riMax = i; // save index of the max vertex
      }
    }
    
    // no contours if all verts have the 'same' value
    if( fmax-fmin < EPS )
      // advance passed this tri
      continue;
        
    // calc the index of the middle rank
    //   
    // 0,1,2: 3-(2+0) = 1
    // 0,2,1: 3-(1+0) = 2
    // 1,0,2: 3-(1+2) = 0
    // 1,2,0: 3-(1+0) = 2
    // 2,0,1: 3-(2+1) = 0
    // 2,1,0: 3-(2+0) = 1
    
    riMid = 3 - (riMin + riMax);
    
    // now, we know the ranking of the three indices
    // iPtr[0], iPtr[1] and iPtr[2], use these directly
    riMin = iPtr[riMin];
    riMid = iPtr[riMid];
    riMax = iPtr[riMax];
    
    // determine the range of contours on this tri
    cMinIndex = (pts[riMin][2] - minZ[2])/cInc + 1;
    cMin = minZ[2] + cMinIndex*cInc;
    
    /* loop over the requested contour values */
    for( cCur = cMin ; cCur <= pts[riMax][2] ; cCur += cInc )
    {
      // map the scalar
      //setColor(cCur);
      
      // are two verts equal?
      if( pts[riMid][2]-pts[riMin][2] < EPS ){
	
	// use same interpolant for both remaining sides
	t1 = (cCur - pts[riMid][2]) / ( pts[riMax][2] - pts[riMid][2]);
	
	// go from riMin -> riMax
	src = pts[riMin];
	dst = pts[riMax];

	v1[0] = *src + t1 * ( *dst - *src ); ++src; ++dst;
	v1[1] = *src + t1 * ( *dst - *src ); ++src; ++dst;
	v1[2] = *src + t1 * ( *dst - *src );
	
	// go from riMid -> riMax
	src = pts[riMid];
	dst = pts[riMax];
	v2[0] = *src + t1 * ( *dst - *src ); ++src; ++dst;
	v2[1] = *src + t1 * ( *dst - *src ); ++src; ++dst;
	v2[2] = *src + t1 * ( *dst - *src );

	d = dist(v1,v2);
	out << "n" << endl;
	out << v1[0] << " " << v1[1] << " m" << endl;
	out << v2[0] << " " << v2[1] << " l c s" << endl;
	if( d > threshhold ){
	  midPt[0] = (v1[0] + v2[0])/2.0;
	  midPt[1] = (v1[1] + v2[1])/2.0;
	  midPt[2] = (v1[2] + v2[2])/2.0;
	  showElev(out,midPt,cCur);
	}
      } else if( pts[riMax][2]-pts[riMid][2] < EPS ){
	
	// use same interpolant for both remaining sides
	t1 = (cCur - pts[riMin][2]) / ( pts[riMid][2] - pts[riMin][2]);
	
	// go from riMin -> riMid
	src = pts[riMin];
	dst = pts[riMid];
	v1[0] = *src + t1 * ( *dst - *src ); ++src; ++dst;
	v1[1] = *src + t1 * ( *dst - *src ); ++src; ++dst;
	v1[2] = *src + t1 * ( *dst - *src );
	
	// go from riMin -> riMax
	src = pts[riMin];
	dst = pts[riMax];
	v2[0] = *src + t1 * ( *dst - *src ); ++src; ++dst;
	v2[1] = *src + t1 * ( *dst - *src ); ++src; ++dst;
	v2[2] = *src + t1 * ( *dst - *src );

	d = dist(v1,v2);
	out << "n" << endl;
	out << v1[0] << " " << v1[1] << " m" << endl;
	out << v2[0] << " " << v2[1] << " l c s" << endl;
	if( d > threshhold ){
	  midPt[0] = (v1[0] + v2[0])/2.0;
	  midPt[1] = (v1[1] + v2[1])/2.0;
	  midPt[2] = (v1[2] + v2[2])/2.0;
	  showElev(out,midPt,cCur);
	}
	
      } else { // all verts are different
	
	// calc the interpolants: 
	
	if( cCur <= pts[riMid][2] ){
	  //1st point lies: riMin <= src <= riMid
	  t0 = (cCur - pts[riMin][2])/( pts[riMid][2] - pts[riMin][2]);
	  src = pts[riMin];
	  dst = pts[riMid];
	} else {
	  //1st point lies: riMid <= src <= riMax
	  t0 = (cCur - pts[riMid][2])/( pts[riMax][2] - pts[riMid][2]);
	  src = pts[riMid];
	  dst = pts[riMax];
	}
	v1[0] = *src + t0 * ( *dst - *src ); ++src; ++dst;
	v1[1] = *src + t0 * ( *dst - *src ); ++src; ++dst;
	v1[2] = *src + t0 * ( *dst - *src );
	
	// 2nd point lies: riMin <= dst <= riMax
	t1 = (cCur - pts[riMin][2]) / ( pts[riMax][2] - pts[riMin][2]);
	src = pts[riMin];
	dst = pts[riMax];
	v2[0] = *src + t1 * ( *dst - *src ); ++src; ++dst;
	v2[1] = *src + t1 * ( *dst - *src ); ++src; ++dst;
	v2[2] = *src + t1 * ( *dst - *src );

	d = dist(v1,v2);
	out << "n" << endl;
	out << v1[0] << " " << v1[1] << " m" << endl;
	out << v2[0] << " " << v2[1] << " l c s" << endl;
	if( d > threshhold ){
	  midPt[0] = (v1[0] + v2[0])/2.0;
	  midPt[1] = (v1[1] + v2[1])/2.0;
	  midPt[2] = (v1[2] + v2[2])/2.0;
	  showElev(out,midPt,cCur);
	}
      }
    } /*  for( cCur = mn ... )*/
  } /* for( tInd=0 ... ) */  

  showPts(out);

  out << "showpage" << endl;

} /* render_indexed_tri_contour() */

int main(int argc, char** argv)
{
	::fstream fin;
	int i,done=0,foundEOL;
	char ch;
	float* thisPt;
	int fIndex = 1;
	
	if( argc == 1 ){
		printf("usage: %s -c<Number of contours> <file>\n",argv[0]);
		return 0;
	} 
	
	if( argc > 2 ){
		if( argv[1][0] == '-' && argv[1][1] == 'c' ){
			if( argv[1][2] == '\0' ){
				numContours = atoi(argv[2]);
				fIndex = 3;
			} else {
				numContours = atoi(&argv[1][2]);
				fIndex = 2;
			}
		} else {
			printf("usage: %s -c<Number of contours> <file>\n",argv[0]);
			return 0;
		}
	}
	
	fin.open(argv[fIndex],ios::in);
	
	if( ! fin.good() ){
		perror("open");    
		return 0;
	}
	
	i=0;
	while( ! done ){
		if( ! fin.good() ){
			done = 1;
			break;
		}        
		// read this component
		if( i == 0 )  
			thisPt = new float[3];
		
		fin >> thisPt[i++];
		if( ! fin.good() ){
			done = 1;
			break;
		}
		
		// eat comma or EOL
		ch = ' ';
		while( ch != ',' && ch != '\n' && fin.peek()!=EOF)
			fin>>ch; 
		
		// all components read, skip to next line
		if( i == 3 ){
			pts.push_back( thisPt );
			foundEOL = 0;
			while( ! foundEOL ){
				fin.unsetf(ios::skipws);
				if( ! fin.good() ){
					done = 1;
					foundEOL = 1;
					break;
				}    
				ch = ' ';
				while( ch != LF && ch != CR && fin.peek()!=EOF)
					fin>>ch;    
				if( ch == LF || ch == CR )
					foundEOL = 1;
			}
			// reset index
			i = 0;
			fin.setf(ios::skipws);
		}
	}
	
	/* Define input points. */
	
	in.numberofpoints = pts.size();
	in.pointlist = new float[in.numberofpoints * 2];
	
	vector<float*>::iterator iter = pts.begin();
	i=0;
	minZ[0]=minZ[1]=minZ[2]= 1.0e10;
	maxZ[0]=maxZ[1]=maxZ[2]=-1.0e10;
	for( ; iter != pts.end() ; ++iter ){
		float* f = *iter;
		in.pointlist[i*2]   = f[0];
		in.pointlist[i*2+1] = f[1];
		++i;
		if( f[0]<minZ[0] )minZ[0]=f[0];
		if( f[0]>maxZ[0] )maxZ[0]=f[0];
		if( f[1]<minZ[1] )minZ[1]=f[1];
		if( f[1]>maxZ[1] )maxZ[1]=f[1];
		if( f[2]<minZ[2] )minZ[2]=f[2];
		if( f[2]>maxZ[2] )maxZ[2]=f[2];
	}
	in.numberofpointattributes = 0;
	in.pointattributelist = (REAL *)NULL;
	in.pointmarkerlist = (int *) NULL;
	in.numberofsegments = 0;
	in.numberofholes = 0;
	in.numberofregions = 1;
	in.regionlist = (REAL *)NULL;
	
	/* Make necessary initializations so that Triangle can return a */
	/*   triangulation in `mid' and a voronoi diagram in `vorout'.  */
	
	mid.pointlist = (REAL *) NULL;
	mid.pointattributelist = (REAL *) NULL;
	mid.pointmarkerlist = (int *) NULL;
	mid.trianglelist = (int *) NULL;
	mid.triangleattributelist = (REAL *) NULL;
	mid.neighborlist = (int *) NULL;
	mid.segmentlist = (int *) NULL;
	mid.segmentmarkerlist = (int *) NULL;
	mid.edgelist = (int *) NULL;
	mid.edgemarkerlist = (int *) NULL;
	
	/* Triangulate the points.  Switches are chosen to read and write a  */
	/*   PSLG (p), preserve the convex hull (c), number everything from  */
	/*   zero (z), assign a regional attribute to each element (A), and  */
	/*   produce an edge list (e), a Voronoi diagram (v), and a triangle */
	/*   neighbor list (n).                                              */
	
	triangulate("zBPNOYiCV", &in, &mid, &vorout);
	
	rndr_indexed_tri_contour();
	
	return 0;
}
