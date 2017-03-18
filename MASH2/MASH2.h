#pragma once
#include "BigInteger.h"
#define MIN_STRONG_PRIME 513

class MASH2 {
public:
	MASH2();
	string Digest(string message);
	string MessageToHex(const string & message);
private:
	BigInteger ZERO;
	BigInteger ONE;
	BigInteger TWO;
	BigInteger TEN;
	BigInteger FIFTEEN;
	BigInteger SIXTEEN;
	BigInteger TWOFIFTYSEVEN;
	
	BigInteger m_modulus;
	BigInteger firstPrime;
	BigInteger secPrime;
};
