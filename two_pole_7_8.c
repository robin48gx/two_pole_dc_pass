

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

	static int32_t y1=0,y2=0;
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

	//res = (short int *)&y0;
	//res++; 
	//printf("y0=%X res should be %X res=%x\n",y0, y0>>16, res);
	//

	// the plus 3 divides by 8	
	return y0>>(BIN_FRACS+3); // divide back down for scaling and then divide by 8 filter gain // *res;
}

#define RAND_RANGE 500
#define DC_TERM 1500

int main () {

	int i;
	int16_t val,res, res34, res78, res78_2, rr;

	for (i=0;i<1000;i++) {

	
		val = sin ( (double)i*10.0  / (3.142 * 2.0)  ) * 10.0 + DC_TERM ;
                rr = rand() % (RAND_RANGE*2);
                  rr -= RAND_RANGE;

	        val += rr;

		// fast sine
		//val = sin ( ((double)i*10.0)  / (3.142 * 2.0)  ) * 10000;


		res = two_pole_7_8 ( val );

		res34 = (((res34<<2) - res34)>>2) + (val>>2);
		res78 = (((res78<<3) - res78)>>3) + (val>>3);
		res78_2 = (((res78_2<<3) - res78_2)>>3) + (res78>>3); // feed res78 into another should be the same as two pole

		printf ("%d val %d res %d res34 %d res78 %d res78_2 %d\n",i, val,res, res34, res78, res78_2);

	}

	return 0;
}
	

