#include "MASH2.h"
#include <iostream>

MASH2::MASH2() : ZERO("0"), ONE("1"), TWO("2"), TEN("A"), FIFTEEN("F"), SIXTEEN("10"), TWOFIFTYSEVEN("101") {
	//firstPrime = firstPrime.GenerateLargePrimes(MIN_STRONG_PRIME, 1000);
	firstPrime = BigInteger(MIN_STRONG_PRIME);
	secPrime = BigInteger("209");
	//secPrime = secPrime.GenerateLargePrimes(MIN_STRONG_PRIME, 1000);
	m_modulus = firstPrime * secPrime;
}

string MASH2::MessageToHex(const string & msg) {
	std::stringstream stream;

	for (string::size_type i = 0; i < msg.length(); ++i) {
		stream.fill('0');
		stream.width(2);
		stream << std::hex << (int)msg[i];
	}

	return stream.str();
}

string MASH2::Digest(string msg) {
	// Convert message and message length into integers
	BigInteger message(MessageToHex(msg));
	// Passes uint32_t
	BigInteger messageBitLength(message.BitLength());

	// Get the next multiple of 16 from the modulus
	BigInteger FOUR("4");
	BigInteger largest16Multiple = (m_modulus.BitLength() >> 4) * 16;

	// 2 ^ (largest16Multiple / 2)
	BigInteger temp = TWO;
	BigInteger limit = (largest16Multiple >> ONE) - ONE;
	for (BigInteger i = ZERO; i < limit; i++) {
		temp *= TWO;
	}

	if (messageBitLength > temp) {
		throw new exception("ERROR: The message is too long. Please try again.");
	}

	BigInteger shift = messageBitLength % (largest16Multiple >> 1) == ZERO ? 
		ZERO 
		: 
		(largest16Multiple >> ONE) - messageBitLength % (largest16Multiple >> ONE);
	
	message <<= shift;

	BigInteger twoPowMessage;
	twoPowMessage = twoPowMessage.Pow(TWO, largest16Multiple);

	BigInteger initVector = ZERO;
	
	BigInteger A = FIFTEEN * (TWO.Pow(TWO, largest16Multiple - FOUR));

	BigInteger t = messageBitLength / (largest16Multiple >> ONE);
	BigInteger prevInitVector;
	for (BigInteger i = ZERO; i < t; i++) {
		prevInitVector = initVector;
		prevInitVector = ZERO;
		BigInteger rem;
		for (BigInteger j = (largest16Multiple / TWO) - FOUR; j >= ZERO; j -= FOUR) {
			initVector = (initVector << FOUR) | FIFTEEN;
			rem = (message >> (j + (largest16Multiple >> ONE) * (t - ONE - i))) % SIXTEEN;
			if (rem < ZERO)
				string halp = "HELP";
			initVector = (initVector << FOUR) | rem;
		}
		initVector ^= prevInitVector;
		initVector |= A;
		initVector = initVector.ExpMod(initVector, TWOFIFTYSEVEN, m_modulus);
		initVector %= twoPowMessage;
		initVector ^= prevInitVector;
	}
	// Assign the new previous initVector and reset initVector
	prevInitVector = initVector;
	initVector = ZERO;

	BigInteger rem;
	for (BigInteger j = (largest16Multiple / TWO) - FOUR; j >= ZERO; j -= FOUR) {
		initVector = (initVector << FOUR) | TEN;
		rem = (messageBitLength >> j) % SIXTEEN;
		if (rem < ZERO)
			string halp = "HELP";
		initVector <<= FOUR;
		initVector |= rem;
	}
	if (A < ZERO)
		string halp = "HELP";
	initVector ^= prevInitVector;
	initVector |= A;
	initVector = initVector.ExpMod(initVector, TWOFIFTYSEVEN, m_modulus) % twoPowMessage;
	initVector ^= prevInitVector;

	return initVector.ToString();
}