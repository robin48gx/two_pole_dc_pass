#include <stdio.h>
#include <math.h>

double xz[10]; //  = {0.0}; // Z transform numerator or zeros Z^0 + Z^1 + Z^2 etc. 
double yp[10]; // = {0.0}; // Z transform denominator or poles Z^0 + Z^1 + Z^2 etc.


typedef struct complex_number {
	double real;
	double imaginary;
} COMPLEX;

#define NYQUIST 1000
#define PI 3.14159265358979323844 

#define SAMPLE_RATE 0.5

double arg ( COMPLEX x) {
	return atan2(x.imaginary, x.real);
}

double mag ( COMPLEX x ) {
	return sqrt ( x.real * x.real + x.imaginary * x.imaginary );
}

// add x1 and x2 result in x1
void complex_add ( COMPLEX * x1, COMPLEX  x2 ) {
	x1->real = x1->real + x2.real;
	x1->imaginary = x1->imaginary + x2.imaginary;
}

void complex_print ( COMPLEX x ) {
	printf("x.r = %f x.i = %f ",x.real,x.imaginary);
}

void complex_scalar_mul ( double m, COMPLEX * x) {
	x->real = m * x->real;
	x->imaginary = m * x->imaginary;
}

void complex_pow ( double p, COMPLEX * x ) {
	double r  = arg(*x) + PI * 2.0;
	double m  = mag(*x);
	
	r *= p;
	while (r>360.0) r -= 360.0; // r %= ( PI * 2.0);
	m = pow(m,p);

	x->real = cos(r) * m;
        x->imaginary = sin(r) * m;	
}

COMPLEX complex_mul ( COMPLEX a, COMPLEX b ) {

	COMPLEX ans;

	ans.real = a.real * b.real - a.imaginary * b.imaginary;
	ans.imaginary = a.real * b.imaginary + b.real * a.imaginary;

	return ans;
}

COMPLEX complex_div ( COMPLEX n, COMPLEX d ) {
	COMPLEX ans;
	// find overall divisor
	//
	double oad;

	oad  = d.real * d.real + d.imaginary * d.imaginary;
	oad  = 1.0/oad;
	//printf("\noad == %f numerator ",oad);
	//complex_print (n);
	//printf("denominator ",oad);
	//complex_print (d);


	d.imaginary = - d.imaginary; // conjugate
	ans = complex_mul ( n, d );
	complex_scalar_mul ( oad, &ans);

	//printf("ans ");
	//complex_print (ans);
	//printf("\n");
	return ans;
}

int main () {

	COMPLEX x,x_num,x_den, x_ans_den, x_ans_num, ccc;
	int i,j,k; // counters
	double r; // angle in radians
	double sixteenth = 1.0/16.0;
	double frequency, tau;

	for (k=0;k<10;k++) {  // zero all the Z parameters
		xz[k] = 0.0;
		yp[k] = 0.0;
	}

	// simple lag filter
	xz[0] = 0.125;
	yp[0] = 1.0;
	yp[1] = -0.875;


	// simple 7/8th lag filter squared
	//
	// xz[0] = (0.125*0.125);
	// yp[0] = 1.0;
	// yp[1] = -(2.0*0.875);
	// yp[2] = (0.875*0.875); 
	//
	// simple 15/16 lag filter squared
	//
	//xz[0] = (sixteenth*sixteenth);
	//yp[0] = 1.0;
	//yp[1] = -(2.0*(1.0-sixteenth));
	//yp[2] = ((1.0-sixteenth)*(1.0-sixteenth));


	// simple bandpass
	// (Z-1)(Z+1) = Z^2-1
	// xz[0] = 1.0;
	// xz[1] = 0.0;
	// xz[2] = -1.0;
	// yp[0] = 1.0;

        // peaky x4 bandpass, no d.c. no high freq near nyquist.	
	// (Z^2-1)^2
	// 
	// xz[0] = 1.0;
	// xz[1] = 0.0;
	// xz[2] = -2.0;
	// xz[3] = 0.0;
	// xz[4] = 1.0;
	// yp[0] = 1.0;
	// yp[1] = 0.0;
	// yp[2] = 0.0;
	//
	
	

	for (i=0;i<NYQUIST;i++) { 

		r = (double)i/NYQUIST * PI;

		x.real = cos(r);
		x.imaginary = sin(r);

		x_ans_num.real = x_ans_num.imaginary = 0.0;
		x_ans_den.real = x_ans_den.imaginary = 0.0;
                
		// now take all the z transforms to their powers and multiple by coeffecients 
		// to get the denominator and numerator for the frequency under inspection
		for (j=0;j<10;j++) {

			x_num = x;
			complex_pow((double)-j,&x_num);
			complex_scalar_mul(xz[j],&x_num);
			complex_add ( &x_ans_num, x_num);

			x_den = x;
			complex_pow((double)-j,&x_den);
			complex_scalar_mul(yp[j],&x_den);
			complex_add ( &x_ans_den, x_den);

		}

		// OK now perform the division to obtain the frequency response 
		//
		ccc = complex_div ( x_ans_num, x_ans_den) ;

		//ccc = x;
		//complex_pow(2.0,&ccc);

		// now knowing the sampling rate the T constant and 
		// frequency can be displayed

		if ( i ) frequency = ( ((double)i)/(NYQUIST*SAMPLE_RATE*2.0) );
		if ( frequency > 0.0 )
			tau = 1.0/(frequency*2.0*PI);

		// where tau is the time constant,i.e. where mag is 0.707
                // then that would be the time constant, RC, for that filter
		//
		//
		printf(" %d arg = %f mag %f maglog10 %f f %f tau %f \n", 
                     i, arg(ccc), mag(ccc), log(mag(ccc))*10.0,frequency,tau);
	}

	//----------------------------------------------------------------------------------------------------------
	// Test pow and scalar multiply
	/*
	x.imaginary = 0.0;
	x.real = 2.0;
	complex_pow(3.0,&x);
	printf(" x.r %f  x.i %f\n", x.real, x.imaginary);
	complex_scalar_mul(2.1,&x);
	printf(" x.r %f  x.i %f\n", x.real, x.imaginary);
	printf(" after scalar * 2.1 x.r %f  x.i %f\n", x.real, x.imaginary);

	x.imaginary = 1.0;
	x.real = 0.0;
	complex_pow(3.0,&x);
	printf(" x.r %f  x.i %f\n", x.real, x.imaginary);
	complex_scalar_mul(-2.1,&x);
	printf(" after scalar * -2.1 x.r %f  x.i %f\n", x.real, x.imaginary);
	*/
	//----------------------------------------------------------------------------------------------------------
	//
	x_ans_num.real =  1.0; x_ans_num.imaginary = 10.0;
	x_ans_den.real = 2.0; x_ans_den.imaginary = 80.0;
	x = complex_div ( x_ans_num, x_ans_den );
	printf(" after divide  x.r %f  x.i %f\n", x.real, x.imaginary);

	

}

#include <stdio.h>
#include <math.h>

double xz[10]; //  = {0.0}; // Z transform numerator or zeros Z^0 + Z^1 + Z^2 etc. 
double yp[10]; // = {0.0}; // Z transform denominator or poles Z^0 + Z^1 + Z^2 etc.


typedef struct complex_number {
	double real;
	double imaginary;
} COMPLEX;

#define NYQUIST 1000
#define PI 3.14159265358979323844 

double arg ( COMPLEX x) {
	return atan2(x.imaginary, x.real);
}

double mag ( COMPLEX x ) {
	return sqrt ( x.real * x.real + x.imaginary * x.imaginary );
}

// add x1 and x2 result in x1
void complex_add ( COMPLEX * x1, COMPLEX  x2 ) {
	x1->real = x1->real + x2.real;
	x1->imaginary = x1->imaginary + x2.imaginary;
}

void complex_print ( COMPLEX x ) {
	printf("x.r = %f x.i = %f ",x.real,x.imaginary);
}

void complex_scalar_mul ( double m, COMPLEX * x) {
	x->real = m * x->real;
	x->imaginary = m * x->imaginary;
}

void complex_pow ( double p, COMPLEX * x ) {
	double r  = arg(*x) + PI * 2.0;
	double m  = mag(*x);
	
	r *= p;
	while (r>360.0) r -= 360.0; // r %= ( PI * 2.0);
	m = pow(m,p);

	x->real = cos(r) * m;
        x->imaginary = sin(r) * m;	
}

COMPLEX complex_mul ( COMPLEX a, COMPLEX b ) {

	COMPLEX ans;

	ans.real = a.real * b.real - a.imaginary * b.imaginary;
	ans.imaginary = a.real * b.imaginary + b.real * a.imaginary;

	return ans;
}

COMPLEX complex_div ( COMPLEX n, COMPLEX d ) {
	COMPLEX ans;
	// find overall divisor
	//
	double oad;

	oad  = d.real * d.real + d.imaginary * d.imaginary;
	oad  = 1.0/oad;
	//printf("\noad == %f numerator ",oad);
	//complex_print (n);
	//printf("denominator ",oad);
	//complex_print (d);


	d.imaginary = - d.imaginary; // conjugate
	ans = complex_mul ( n, d );
	complex_scalar_mul ( oad, &ans);

	//printf("ans ");
	//complex_print (ans);
	//printf("\n");
	return ans;
}

main () {

	COMPLEX x,x_num,x_den, x_ans_den, x_ans_num, ccc;
	int i,j,k; // counters
	double r; // angle in radians
	double sixteenth = 1.0/16.0;

	for (k=0;k<10;k++) {  // zero all the Z parameters
		xz[k] = 0.0;
		yp[k] = 0.0;
	}

	// simple lag filter
	// xz[0] = 0.125;
	// yp[0] = 1.0;
	// yp[1] = -0.875;


	// simple 7/8th lag filter squared
	//
	// xz[0] = (0.125*0.125);
	// yp[0] = 1.0;
	// yp[1] = -(2.0*0.875);
	// yp[2] = (0.875*0.875); 
	//
	// simple 15/16 lag filter squared
	//
	xz[0] = (sixteenth*sixteenth);
	yp[0] = 1.0;
	yp[1] = -(2.0*(1.0-sixteenth));
	yp[2] = ((1.0-sixteenth)*(1.0-sixteenth));


	// simple bandpass
	// (Z-1)(Z+1) = Z^2-1
	// xz[0] = 1.0;
	// xz[1] = 0.0;
	// xz[2] = -1.0;
	// yp[0] = 1.0;

        // peaky x4 bandpass, no d.c. no high freq near nyquist.	
	// (Z^2-1)^2
	// 
	// xz[0] = 1.0;
	// xz[1] = 0.0;
	// xz[2] = -2.0;
	// xz[3] = 0.0;
	// xz[4] = 1.0;
	// yp[0] = 1.0;
	// yp[1] = 0.0;
	// yp[2] = 0.0;
	//
	
	

	for (i=0;i<NYQUIST;i++) { 

		r = (double)i/NYQUIST * PI;

		x.real = cos(r);
		x.imaginary = sin(r);

		x_ans_num.real = x_ans_num.imaginary = 0.0;
		x_ans_den.real = x_ans_den.imaginary = 0.0;
                
		// now take all the z transforms to their powers and multiple by coeffecients 
		// to get the denominator and numerator for the frequency under inspection
		for (j=0;j<10;j++) {

			x_num = x;
			complex_pow((double)-j,&x_num);
			complex_scalar_mul(xz[j],&x_num);
			complex_add ( &x_ans_num, x_num);

			x_den = x;
			complex_pow((double)-j,&x_den);
			complex_scalar_mul(yp[j],&x_den);
			complex_add ( &x_ans_den, x_den);

		}

		// OK now perform the division to obtain the frequency response 
		//
		ccc = complex_div ( x_ans_num, x_ans_den) ;

		//ccc = x;
		//complex_pow(2.0,&ccc);
		printf(" %d arg = %f mag %f maglog10 %f\n", i, arg(ccc), mag(ccc), log(mag(ccc))*10.0 );
	}

	//----------------------------------------------------------------------------------------------------------
	// Test pow and scalar multiply
	/*
	x.imaginary = 0.0;
	x.real = 2.0;
	complex_pow(3.0,&x);
	printf(" x.r %f  x.i %f\n", x.real, x.imaginary);
	complex_scalar_mul(2.1,&x);
	printf(" x.r %f  x.i %f\n", x.real, x.imaginary);
	printf(" after scalar * 2.1 x.r %f  x.i %f\n", x.real, x.imaginary);

	x.imaginary = 1.0;
	x.real = 0.0;
	complex_pow(3.0,&x);
	printf(" x.r %f  x.i %f\n", x.real, x.imaginary);
	complex_scalar_mul(-2.1,&x);
	printf(" after scalar * -2.1 x.r %f  x.i %f\n", x.real, x.imaginary);
	*/
	//----------------------------------------------------------------------------------------------------------
	//
	x_ans_num.real =  1.0; x_ans_num.imaginary = 10.0;
	x_ans_den.real = 2.0; x_ans_den.imaginary = 80.0;
	x = complex_div ( x_ans_num, x_ans_den );
	printf(" after divide  x.r %f  x.i %f\n", x.real, x.imaginary);

	

}

