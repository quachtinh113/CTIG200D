#include "adc.h"
#include "kalman.h"
//float KalmanFilter( float inData )
//	{
//			static float prevData = 0;                                 //previous data
//			static float p = 10, q = 0.001, r = 0.001, kGain = 0;      // q Control error r Control response speed
//			p = p + q;
//			kGain = p / ( p + r );                                      //calculate kalman gain
//			inData = prevData + ( kGain * ( inData - prevData ) );      //calculate the estimated value of this filter
//			p = ( 1 - kGain ) * p;                                      //Update measurement variance
//			prevData = inData;
//			return inData;                                             //return an estimate
//	}
	 static double kalman_filter(double previous )
	{
			double  LastData;
			double NowData;
			double kalman_adc;
			static double kalman_adc_old = 0;
			static double P1,Q=0.0003,R=5,Kg=0,P=1;
			NowData = previous;
			LastData = kalman_adc_old;
			P = P1 + Q;
			Kg = P / ( P + R );
			kalman_adc = LastData + Kg *(NowData-kalman_adc_old);
			P1 = ( 1 - Kg ) * P;
			P = P1;
			kalman_adc_old = kalman_adc;
			return ( double)( kalman_adc);
	}
	