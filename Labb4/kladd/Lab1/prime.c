#include "include/lcd.h"


int is_prime(long i) {
	long n = 2;
	while (n < i) {
		if (i % n == 0) {return 0;}
		n++;
	}
	return 1;
}


void primes() {
	long count = 2;
	while (1) {
		if (is_prime(count)) {writeTwoDigits(count, 1);}
		count++;
	}
}


void primes4(long count) {
	if (is_prime(count)) {writeLong(count);}
	count++;
}