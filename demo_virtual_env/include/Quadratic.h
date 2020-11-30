
#include <math.h>

class Quadratic
{
 private:
  float lS,lT,lU,lV;
  float x0, x1, y0, y1;  
 public:
  Quadratic(const float& S, const float& T, const float& U, const float& V) { lS=S; lT=T; lU=U; lV=V; }

  void SetBounds(const float& minX, const float& maxX, const float& minY, const float& maxY)
  {x0=minX; x1=maxX; y0=minY; y1=maxY;}

  inline bool Intersect(const float& heading, float& x, float& y)
  {
	// equation for y = sin/cos * x
	float sine = sin(heading);
	float cosine = cos(heading);

	if(cosine != 0)
	{
		float m = sine/cosine;	// Slope
	   	
	   	// ax^2 + bx + c = 0
	   	float a = (lS);	
	    float b = (lT+m);
	    float c = (lV);

	    float dis = (b*b)-4*a*c;

		// testing for complex numbers	
		if(dis >= 0)
		{
			// Real roots
			float x1 = (-b + sqrt(dis))/(2*a);
			float x2 = (-b - sqrt(dis))/(2*a);
			
			// Pick root with same sign component as heading
			if (x1 > 0 && cosine > 0 || x1 < 0  && cosine < 0)
			// CHANGED: Finding the lowest X in the direction of heading
			// BEFORE: you were saying that x1 was the lowest hit on the Quadratic
			//if (x1 < x2 && cosine > 0 || x1 > x2  && cosine < 0) 
				x = x1; 
			else
				x = x2;

			y = m*x;
			float OUTRANGE = sqrt(y*y+x*x);
			
			// Testing the output (DEBUGGING)
			printf("X: %f | Y: %f | S: %f | C:  %f | X1: %f | X2: %f | M: %f | OUTRANGE: %f\n", x, y,sine,cosine,x1,x2,m,OUTRANGE);
		}
		else
		{
			// No real roots (no hits in real numbers)
			return(false);
		}
	}
	else
	{
		if(lV > 0 && sine > 0 || lV < 0 && sine < 0)
		{

			// Vertical line from heading
			x = 0;
			y = lV;
		}
		else
		{
			// no intersection
			return(false);
		}
	}
	if(x < x0 || x > x1 || y < y0 || y > y1)
	{
		// intersection is out of bounds
		return(false);
	}
	    
	return(true);
  }
  
};
