float tau=0.075;
float a=0.0;

// a=tau / (tau + loop time)
// newAngle = angle measured with atan2 using the accelerometer
// newRate =  angle measured using the gyro
// looptime = loop time in millis() 
//

float Complementary(float newAngle, float newRate,int looptime){
  float dtC = float(looptime)/1000.0;
  a=tau/(tau+dtC) ;
  x_angleC=/*高通*/ a* (x_angleC/*舊值*/ + newRate * dtC/*陀螺儀積分結果*/) + /*低通*/(1-a) * (newAngle/*加速度計*/);
//長期準確(如加速度計)=>低通
//短期準確(如陀螺儀)=>高通
  return x_angleC;
}
