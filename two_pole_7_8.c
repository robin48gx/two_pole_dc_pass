

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>

// Accuracy here matters.
// With a max 25k TDS (i.e. 16 bits input signal signed)
// this means 6 + 16 == 22.
// This fits within a 32 bit int
// and leaves ten bits of head room.
//
// For two pole the y0, y1, y2 registers must be 32 bit at least.
// The new rpi is 64 bit so these have had to have been deliberately set
// as int32_t to simulate the pic18 `long' type.
// The short int type on the PIC18 is `int'
//
#define BIN_FRACS 6

// this is a 64 bit machine (the pi)
//
int16_t /* squared version of LAG_7_8 */
two_pole_7_8 ( int16_t input ) {

	static int32_t y1=0,y2=0, x1,x2;
	int32_t * res;
	int32_t y0;

	int32_t x0 = input;

	// x0 times 0.125	
	// the minus 3 divides by 8	 : DOUBLE POLE
	x0 <<= (BIN_FRACS-3) ;   // now all calculations are done times BIN_FRACS^2

	// this works stabley but not well WHY?
	//y0 = x0 + y1-(y1>>3) - ((y2>>1) + (y2>>2) + (y2>>6));
	
        // should be 	
	//y0 = x0 + y1+y1-(y1>>2) - (y2>>1 + y2>>2 + y2>>6);
	//
	//
	// OK this works well 
	// y0 = x0 + (14.0 / 8.0) * (double) y1 - (49.0/64.0) * (double) y2; 



	y0 = x0 + 
	       //	(14.0 / 8.0) * (double) y1
	       //	this is 7/4
	       y1 + y1 - (y1>>2)  
	       //
	       //
	       //	- (49.0/64.0) * (double) y2; 
	       //
	       -  (    (y2>>1) +  // half
	               (y2>>2) +  // quarter
		       (y2>>6) ); // 64th      


	//
	// y0 = x0/64 + (y1 * 7) / 4 + (y2*49)/64;

	// see what happens when y2>>6 is left out. I theory d.c goes unstable
	// and yes it does! 25AUG2019
	// y0 = x0 + y1-(y1>>3) - (y2>>1 + y2>>2  /* + y2>>6*/);

	y2 = y1;
	y1 = y0;
	x2 = x1;
	x1 = x0;

	//res = (short int *)&y0;
	//res++; 
	//printf("y0=%X res should be %X res=%x\n",y0, y0>>16, res);
	//

	// the plus 3 divides by 8	
	return y0>>(BIN_FRACS+3); // divide back down for scaling and then divide by 8 filter gain // *res;
}


// Added zero guard. tried a pair of zeros near unit circle 
// 7/8 in ADJ, i.e hyp = 1 so on unit circle.
//
int16_t /* squared version of LAG_7_8 */
two_pole_7_8_zg ( int16_t input ) { // with zero guard

	static int32_t y1=0,y2=0, x1,x2,x3,x4,x5,x6,x7,x8;
	int32_t * res;
	int32_t y0;

	int32_t x0 = input;

	// x0 times 0.125	
	// the minus 3 divides by 8	 : DOUBLE POLE
	x0 <<= (BIN_FRACS-3) ;   // now all calculations are done times BIN_FRACS^2

	// this works stabley but not well WHY?
	//y0 = x0 + y1-(y1>>3) - ((y2>>1) + (y2>>2) + (y2>>6));
	
        // should be 	
	//y0 = x0 + y1+y1-(y1>>2) - (y2>>1 + y2>>2 + y2>>6);
	//
	//
	// OK this works well 
	// y0 = x0 + (14.0 / 8.0) * (double) y1 - (49.0/64.0) * (double) y2; 


        // (z-j)(z+j) zeros at half nyquist (Z^2 + 1) x0 + x2
	y0 =  //(x0>>1) + (x2>>1) + 
		(x0+x1+x2+x3) +	
		//(x0>>1) +  (x0>>2) + (x0>>6) + (x2>>1) +  // 49/64 * x0 + 1/2 * x2
	       //	(14.0 / 8.0) * (double) y1
	       //	this is 7/4
	       y1 + y1 - (y1>>2)  
	       //
	       //
	       //	- (49.0/64.0) * (double) y2; 
	       //
	       -  (    (y2>>1) +  // half
	               (y2>>2) +  // quarter
		       (y2>>6) ); // 64th      


	//
	// y0 = x0/64 + (y1 * 7) / 4 + (y2*49)/64;

	// see what happens when y2>>6 is left out. I theory d.c goes unstable
	// and yes it does! 25AUG2019
	// y0 = x0 + y1-(y1>>3) - (y2>>1 + y2>>2  /* + y2>>6*/);

	y2 = y1;
	y1 = y0;
	x4 = x3;
	x3 = x2;
	x2 = x1;
	x1 = x0;

	//res = (short int *)&y0;
	//res++; 
	//printf("y0=%X res should be %X res=%x\n",y0, y0>>16, res);
	//

	// the plus 3 divides by 8	
	return y0>>(BIN_FRACS+3); // divide back down for scaling and then divide by 8 filter gain // *res;
}


//
// At 20,000 TDS with 1500 noise this caused instability in the filter
// when BINFRACS was set to 9. As the input is not divided
// its already 8 bits (*256) high. Try BINFRACS at 2.
//
#define BIN_FRACS_15_16 2

// this is a 64 bit machine (the pi)
//
int16_t /* squared version of LAG_15_16 */
two_pole_15_16 ( int16_t input ) {

	static int32_t y1=0,y2=0, x1, x2;
	int32_t * res;
	int32_t y0;

	int32_t x0 = input;


	x0 <<= (BIN_FRACS_15_16) ;   // now all calculations are done times BIN_FRACS^2


	y0 = x0  +
	       //	
	       //	this is 7/8 which is 2 * 15/16...
	       y1 + y1 - (y1>>3)  
	       //
	       //
	       //	- (49.0/64.0) * (double) y2; 
	       //
	       ;


	       //-  (    (y2>>1) +  // half
	       //        (y2>>2) +  // quarter
		//	       (y2>>3) +   // eighth
		//	       (y2>>8)     // 256th 
		//	  );      
		//
		// try to do the shfts in a way effecient for the HYTEC PIC18 compiler
		//
		y2>>=1;
		y0 -= y2;
		y2>>=1;
		y0 -= y2;
		y2>>=1;
		y0 -= y2;
		y2>>=5;
		y0 -= y2;
		


	y2 = y1;
	y1 = y0;


	// gain of the filter is 256 ((1/16)^2) if x0 is allowed in without pre-dividing
	//
	y0 >>= 8;

	return y0>>(BIN_FRACS_15_16); // divide back down for scaling and then divide by 8 filter gain // *res;
}



int zero_only_2 ( int input )
{

	static int x0,x1,x2,x3,x4;

	int res=1500*4;

	// (z^2+1)^2 ZEROS AT HALF NYQUIST
	//    z^4    z^2     1
	//res = input + 2*x2 + x4;


	x4 = x3; 
	x3 = x2;
	x2 = x1;
	x1 = input;

	return res/4;
}



int zero_only_3 ( int input )
{

	static int x0,x1,x2,x3,x4;
	static int x5,x6,x7,x8,x9;

	int res=1500;;

	// (Z+j)(Z-j) == (Z^2+1) // zero at nyquist 
	// (z^2+1)^3 ZEROS AT HALF NYQUIST
	//    z^6    z^4  2z^3   z^2  2*z   1
	//res = input + x2 +2*x3 + x4 +2*x2 + x6;

	// (Z^2 - 1)(Z+1) is zero at half nyquist and zero at nyquist
	//  z^3 +z^2 + z + 1
	res = input + x1 + x2 + x3;
	x8 = x7; 
	x7 = x6; 
	x6 = x5; 
	x5 = x4; 
	x4 = x3; 
	x3 = x2;
	x2 = x1;
	x1 = input;

	return res/4;
}

int zero_only_4 ( int input )
{

	static int x0,x1,x2,x3,x4;
	static int x5,x6,x7,x8,x9;

	int res=1500*16;
	// (Z^2+1)^4 ZEROS AT HALF NYQUIST
	//      z^8   2z^6  2z^5  2z^4  4z^3   2z^2   2z^1  + 1
	//res = input + 2*x2 +2*x3 + x4 + 4*x5 + 2*x6 + 2*x7 + x8;

	// (Z+1)(Z-1 // zero at nyquist zero at half nyquist
	// (Z^2 - 1)
	//
	x8 = x7; 
	x7 = x6; 
	x6 = x5; 
	x5 = x4; 
	x4 = x3; 
	x3 = x2;
	x2 = x1;
	x1 = input;

	return res/16;
}


// This was initially tested at 1500 TDS with \pm 500 noise
// Tests at 15000 with \pm 1500 noise at BINFRACS 9 caused instability in the two pole 15 16
// BINFRACS down to 2 for that filter.

// #define RAND_RANGE 500
// #define DC_TERM    1500

#define RAND_RANGE 500
#define DC_TERM 1000


int main () {

	int i, zo2,zo3,zo4;
	int16_t val,res, res34, res78, res78_2, rr, res_1516;

	for (i=0;i<1000;i++) {

	
		val = sin ( (double)i*10.0  / (3.142 * 2.0)  ) * 10.0 + DC_TERM ;
                rr = rand() % (RAND_RANGE*2);
                  rr -= RAND_RANGE;

	        val += rr;

		// fast sine
		//val = sin ( ((double)i*10.0)  / (3.142 * 2.0)  ) * 10000;


		res = two_pole_7_8 ( val );
		res_1516 = two_pole_15_16 ( val );




		zo2 = two_pole_7_8_zg(val); // zero_only_2 (val);
		zo3 = zero_only_3 (val);
		zo4 =  zero_only_4 (val);

		res34 = (((res34<<2) - res34)>>2) + (val>>2);
		res78 = (((res78<<3) - res78)>>3) + (val>>3);
		res78_2 = (((res78_2<<3) - res78_2)>>3) + (res78>>3); // feed res78 into another should be the same as two pole
               
		//        1      3     5        7        9          11          13     15     17     19
		printf ("%d val %d res %d res34 %d res78 %d res78_2 %d res_1516 %d zo2 %d zo3 %d zo4 %d EOL\n",
				i, val,res, res34, res78, res78_2, res_1516, zo2, zo3, zo4);

	}

	return 0;
}
	



