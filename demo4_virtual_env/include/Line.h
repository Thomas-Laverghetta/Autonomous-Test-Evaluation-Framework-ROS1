
#include <math.h>

class Line
{
 private:
  float la,lb,lc;
  float x0, x1, y0, y1;
  
 public:
  Line ( const float& A, const float& B, const float& C) { la=A; lb=B; lc=C; }

  void SetBounds(const float& minX, const float& maxX, const float& minY, const float& maxY)
  {x0=minX; x1=maxX; y0=minY; y1=maxY;}

  inline bool Intersect(const float& Heading, float origX, float origY, float& x, float& y)
  {
    // float a1 = la;
    // float b1 = lb;
    // float c1 = lc;

    // // equation for -xsin + ycos = 0
    // //float a2 = -sin(Heading);
    // float b2 = cos(Heading);
    // float c2 = 0.0f;
    // if (b2 != 0)
    // {
    //   float m = sin(Heading)/cos(Heading);
    //   x = -(c1/(a1+m*b1));
    //   y = m*x;
    // }
    // else{ 
    //   x = 0; 
    //   y = -c1/b1;
    // }    

    float a = la;
    float b = lb;
    float c = lc;

    // equation for -xsin + ycos = 0
    float u = (sin(Heading));
    float v = (cos(Heading));
    float h = origX;
    float k = origY;

    if (v != 0 && b != 0)
    {
      float m1 = u/v;
      float m2 = a/b;
      x = (m1*h - k - (c/b)) / (m1 + m2);
      y = (-1/b)*(a*x + c);
    }
    else if(v == 0)
    {
      x = h;
      y = -(c + a*h) / b;
    }
    else if(b == 0)
    {
      float m1 = u/v;
      y = (-c/a)*m1 - h*m1 + k;
      x = y/m1 + h - k/m1;
    }
    else
    {
      return (false); // Lines are parallel vertical lines
    }



    float OUTRANGE = sqrt(y*y+x*x);
      
    // Testing the output (DEBUGGING)
    // printf("X: %f | Y: %f | OUTRANGE: %f\n", x, y,OUTRANGE);

    //float det = a1*b2 - a2*b1;

    /*if(det == 0)
      {
	// lines are parallel
	return(false);
      }
    else
      {
	x = (-c1*b2) / det;
	y = (-a2*c1) / det;
      }
  */

    if(x < x0 || x > x1 || y < y0 || y > y1)
    {
        // intersection is out of bounds
      return(false);
    }

  	// float mag = sqrt((x*x)+(y*y));
  	// float dot = cos(Heading)*x/mag + sin(Heading)*y/mag;
  	// if(dot < 0)
  	// {
  	// 	// Intersection is pointing away from Heading
  	// 	return(false);
  	// }

    // float mag = sqrtf((x-origX)*(x-origX)+(y-origY)*(y-origY));
    // float dot = cos(Heading)*(x-origX)/mag + sin(Heading)*(y-origY)/mag;
    // if(dot < 0)
    // {
    //   // Intersection is pointing away from Heading
    //   return(false);
    // }

      return(true);
  }



  
  inline float RelativeAngle(const float& x, const float& y, const float& Head) {
  	float yPoint[] = {(-x0*la-lc)/(lb), (-x1*la-lc)/(lb)}; // y1 , y2
    float Ymin, Xmin;
  	// Calculating the proper Y-value based on Heading (Lowest  Y-value directed towards Sensor)
    if (abs(yPoint[0]) < abs(yPoint[1]))
      { Ymin = yPoint[0]; Xmin = x0; }
    else
      { Ymin = yPoint[1]; Xmin = x1; }
  	// Assuming from the origin
    float mag = sqrtf(x*x+y*y);
    float magMax = sqrtf((Ymin-y)*(Ymin-y)+(Xmin-x)*(Xmin-x));

    // Finding the slope of the line
    float Slope = (Ymin-y)/(Xmin-x); 
    
    // Finding the Product of the two Vectors
    // V = <-x, -y>: Vector is pointing in oppisite direction
    // U = <Xmin - x, Ymin - y> 
    float dot = ((-x*(Xmin-x)-y*(Ymin-y))/(mag*magMax));    
    float Theta = acos(dot)*180/3.14159265358978f; // Turning into deg from Rad
      
    printf("SLOPE %f | DOT %f | THETA %f\n", Slope, dot, Theta);
    // finding the angles
    // All Angles are Relative to   
    /*if (Slope < 0){ // Positive Slope
      return 90-Theta;
    }
    else if (Slope > 0){ // Negative Slope
      return 90+Theta;
    }
    else*/ 
      return abs(Theta-90); // Horizontal Line, therefore, 90-degrees
  
  }
};
