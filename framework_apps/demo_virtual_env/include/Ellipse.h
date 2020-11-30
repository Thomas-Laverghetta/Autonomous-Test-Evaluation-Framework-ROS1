
#include <math.h>

class Ellipse
{
 private:
  float H, K, A, B;
//  float minX, maxX, minY, maxY;
  
 public:
  Ellipse ( const float& centerX, const float& centerY, const float& axisX, const float& axisY) 
  {H=centerX;K=centerY;A=axisX;B=axisY;}

//  void SetBounds(const float& _minX, const float& _maxX, const float& _minY, const float& _maxY)
//  {minX=_minX; maxX=_maxX; minY=_minY; maxY=_maxY;}

  inline bool Intersect(const float& Heading, float& x, float& y)
  {
    // equation for xsin + ycos = 0
    float sine = sin(Heading);
    float cosine = cos(Heading);    

    if(cosine != 0)  // Sloped line case
    {

      // Equation from http://www.ambrsoft.com/TrigoCalc/Circles2/Ellipse/EllipseLine.htm
      float M = sine / cosine;
      float q = A*A*M*M + B*B;
      float dis = A*A*M*M + B*B - M*H*M*H - K*K + 2*M*H*K;
      printf("DIS %f | q %f\n", dis, q);
      // if(dis >= 0)    // real intersection
        //float sqr = sqrtf(dis);
        float sqr = sqrtf(abs(dis));

        float x1 = (H*B*B - M*A*A*(-K) + A*B*sqr) / q;
        float y1 = (B*B*M*H + K*A*A*M*M + A*B*M*sqr) / q;
        float sqMag1 = x1*x1 + y1*y1;      

        float x2 = (H*B*B - M*A*A*(-K) - A*B*sqr) / q;
        float y2 = (B*B*M*H + K*A*A*M*M - A*B*M*sqr) / q;
        float sqMag2 = x2*x2 + y2*y2;

        // Pick root with same sign component as heading
        //if (x1 == x2) // repeating roots
        //  x = x1;
        if(dis >= 0)
        {
          if (x1 < x2 && cosine > 0 || x1 > x2  && cosine < 0){
          //if ((x1 > 0 && cosine > 0 || x1 < 0 && cosine < 0)) {// x1 closest intersection in view
            x = x1;
            y = y1;
          }
          else{ //if ((x2 > 0 && cosine > 0 || x2 < 0 && cosine < 0)) { // x2 closest intersection in view
            y = y2; 
            x = x2;
          }
        }
        else
          return false;
        //}
        // else
        // {
        //   if(sqMag1 < sqMag2){
        //     x = x1;
        //     y = y1;
        //   }
        //   else{
        //     x = x2;
        //     y = y2;
        //   }
        // }
        //else
          //return(false);    // no intersection - case intersection not on same side as heading

        //y = M*x;    // compute Y from X
        float OUTRANGE = sqrt(y*y+x*x);

        // Testing the output (DEBUGGING)
        printf("X: %f | Y: %f | S: %f | C:  %f | X1: %f | X2: %f | M: %f | OUTRANGE: %f\n", x, y,sine,cosine,x1,x2,M,OUTRANGE);
      // else
      // {
      //   // no intersection
      //   return(false);
      // }
    }
    else
    {
      // Vertical line case
      //printf("Testing Testing Testing Testing IIII\n"); 

      float dis = B*B*A*A - B*B*H*H;

      if(dis > 0) // real intersection
      {
        //float y1 = K + sqrtf(B*B*A*A - B*B*H*H) / (A);
        //float y2 = K - sqrtf(B*B*A*A - B*B*H*H) / (A);

        float y1 = K + abs(B/A)*sqrtf(A*A-H*H);
        float y2 = K - abs(B/A)*sqrtf(A*A-H*H);

        if(y1 > 0 && sine > 0 || y1 < 0 && sine < 0)
          y = y1;
        else
          y = y2;

        x = 0;    // vertical line so x coordinate would be 0
      }
      else
      {
        return(false);  // no intersection with vertical line
      }

    }

// if(x < x0 || x > x1 || y < y0 || y > y1)
//       {
//   // intersection is out of bounds
//   return(false);
//       }
//     return(true);
//   }

    return(true);
  }
  
  
};
