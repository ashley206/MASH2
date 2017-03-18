#include "BigInteger.h"
// Optimization for comparing against 0
const BigInteger ZERO("0");
const BigInteger ONE("1");

// Default ctor for the BigInteger class.
BigInteger::BigInteger() :m_negative(false) {
	m_bits.empty();
}

// Input is anticipated to be in hexadecimal format.
BigInteger::BigInteger(string hex) : m_negative(false)
{
	// Convert the hex to binary
	m_bits = HexToBinary(hex);
}

BigInteger::BigInteger(int num) :m_negative(false) {
	// Convert integer to hex string
	std::stringstream stream;
	stream << std::hex << num;
	std::string hex(stream.str());

	// Convert hex to binary
	m_bits = HexToBinary(hex);
}

// Overloaded operator= method to properly assign values in objects. 
BigInteger & BigInteger::operator=(const BigInteger & rhs) {
	if (this != &rhs) {
		this->m_bits = rhs.m_bits;
		this->m_negative = rhs.m_negative;
	}
	return *this;
}

// Copy constructor for the BigInteger class. 
BigInteger::BigInteger(const BigInteger & rhs) {
	this->m_bits = rhs.m_bits;
	this->m_negative = rhs.m_negative;
}

// Converts hex input into a dynamically sized binary number.
// 8 hexadecimal characters that will be converted into a binary notation.
vector<uint32_t> BigInteger::HexToBinary(string strHex) {
	vector<uint32_t> bits;
	try {
		if (strHex[0] == '-') {
			strHex = strHex.substr(1, strHex.length() - 1);
			m_negative = true;
		}
		// Reverse to force LSBs to be converted first
		reverse(strHex.begin(), strHex.end());
		for (int i = 0; i < ceil(strHex.length()) / 8; i++) {
			// Take 8hex (32bit) and re-reverse
			string sub = strHex.substr(i * 8, 8);
			reverse(sub.begin(), sub.end());
			if (sub.length() != 0)
			{
				// Convert substring into unsigned long from hex
				uint32_t binary = std::stoul(sub, 0, 16);
				bits.push_back(binary);
			}
		}
	}
	catch (...) {
		bits.clear();
		throw exception("Improper hex format.");
	}
	return bits;
}

// Overrides the + operator in order to add  one BigInteger to another. 
BigInteger BigInteger::operator+(const BigInteger & rhs) const {
	BigInteger ret = *this;
	return ret += rhs;
}

// Overrides the += operator in order to add and simultaneously
// assign one BigInteger to another. 
BigInteger & BigInteger::operator+=(const BigInteger & rhs) {
	int carry = 0;
	BigInteger longerNum;
	BigInteger shorterNum;

	if (rhs.m_bits.size() > this->m_bits.size()) {
		longerNum = rhs;
		shorterNum = *this;
	}
	else {
		longerNum = *this;
		shorterNum = rhs;
	}
	// First iterate over shorter number, then the remaining elements
	for (int i = 0; i < shorterNum.m_bits.size(); i++) {
		uint64_t sum = (uint64_t)longerNum.m_bits[i] + (uint64_t)shorterNum.m_bits[i] + carry;
		// Check carry and overflow
		carry = 0;
		if (sum >= UINT32_MAX) {
			carry = 1;
			sum -= (UINT32_MAX + 1);
		}
		// Put the "remainder" value in the current element
		longerNum.m_bits[i] = sum;
	}
	for (int i = shorterNum.m_bits.size(); i < longerNum.m_bits.size(); i++) {
		longerNum.m_bits[i] += carry;
		carry = 0;
		if (longerNum.m_bits[i] > UINT32_MAX) {
			carry = 1;
			longerNum.m_bits[i] -= (UINT32_MAX + 1);
		}
	}
	// Check carry once more -- push 1 at MSB
	if (carry) {
		longerNum.m_bits.push_back(1);
		carry = 0;
	}
	*this = longerNum;
	return *this;
}

// Overrides the - operator in order to subtract one BigInteger
// from another. 
BigInteger BigInteger::operator-(const BigInteger & rhs) const
{
	BigInteger ret = *this;
	return ret -= rhs;
}

// Overrides the -= operator in order to subtract and simultaneously
// assign one BigInteger to another. 
BigInteger & BigInteger::operator-=(const BigInteger & rhs)  {
	int borrow = 0; 
	uint32_t diff = 0;
	BigInteger finalNum;
	BigInteger largerNum;
	BigInteger smallerNum;

	if (*this == rhs) {
		*this = ZERO;
		return *this;
	}
	//Determine the larger value
	if (*this > rhs) {
		largerNum = *this;
		smallerNum = rhs;
	}
	else{
		largerNum = rhs;
		smallerNum = *this;
	}
	// Check if the signs are not equal
	if (this->m_negative != rhs.m_negative) {
		finalNum = (largerNum.Abs() + smallerNum.Abs());
		if (this->m_negative) {
			finalNum.m_negative = true;
		}
	}
	else {
		// Equivalent numbers = 0
		if (*this == rhs) {
			finalNum = BigInteger("0");
		}
		// Make both numbers positive
		BigInteger l = largerNum.Abs();
		BigInteger s = smallerNum.Abs();

		// Iterate over the smaller number and then the larger number
		for (int i = 0; i < s.m_bits.size(); i++) {
			diff = (int32_t)l.m_bits[i] - (int32_t)s.m_bits[i] - borrow;
			// Check borrow and underflow
			if (borrow) {
				borrow = 0;
			}
			if (diff < 0) {
				diff += UINT32_MAX;
				borrow = 1;
			}
			l.m_bits[i] = diff;
		}
		for (int i = s.m_bits.size(); i < l.m_bits.size(); i++) {
			if (borrow) {
				diff = l.m_bits[i] - 1;
				borrow = 0;
			}
			if (diff < 0) {
				l.m_bits[i] = Mod(diff, UINT32_MAX);
				borrow = 1;
			}
		}
		// Final value is determined by original signs (+/-)
		if (this->m_negative) {
			if (*this > rhs) {
				l.m_negative = true;
			}
			else {
				l.m_negative = false;
			}
		}
		else {
			if (*this > rhs) {
				l.m_negative = false;
			}
			else {
				l.m_negative = true;
			}
		}
		finalNum = l;
	}
	// In case it requires less bits than before
	finalNum.Resize();
	*this = finalNum;
	return *this;
}

// Overrides the *= operator to multiply two BigIntegers
BigInteger BigInteger::operator*(const BigInteger & rhs) const {
	BigInteger ret = *this;
	return ret *= rhs;
}

// Overrides the *= operator to multiply and simultaneously assign
// One BigInteger to another. 
BigInteger & BigInteger::operator*=(const BigInteger & rhs)
{
	BigInteger longerNum;
	BigInteger shorterNum;
	BigInteger product;
	int carry = 0;

	if (rhs == ZERO || *this == ZERO) {
		*this = ZERO;
		return *this;
	}
	// Find the longer of two numbers
	if (rhs.m_bits.size() > this->m_bits.size()) {
		longerNum = rhs;
		shorterNum = *this;
	}
	else {
		longerNum = *this;
		shorterNum = rhs;
	}
	// Multiply single value of smaller # by each value of the larger #
	uint64_t prod = 0;
	for (int i = 0; i < shorterNum.m_bits.size(); i++) {
		for (int j = 0; j < longerNum.m_bits.size(); j++) {
			prod = (uint64_t)shorterNum.m_bits[i] * (uint64_t)longerNum.m_bits[j];
			// Check carry and overflow
			prod += carry;
			carry = 0;
			if (prod > UINT32_MAX) {
				carry = prod / ((uint64_t)UINT32_MAX + 1);
				prod -= carry * ((uint64_t)UINT32_MAX + 1);
			}
			// Push final product into final array
			product.m_bits.push_back(prod);
		}
		prod = 0;
	}
	if (carry > 0) {
		product.m_bits.push_back(carry);	// bring down the remaining carry value
	}
	// Final value relies on initial sign
	if (this->m_negative != rhs.m_negative) {
		product.m_negative = true;
	}
	*this = product;
	return *this;
}

BigInteger BigInteger::operator/(const BigInteger & rhs) const
{
	BigInteger ret = *this;
	return ret /= rhs;
}

BigInteger & BigInteger::operator/=(const BigInteger & rhs) {
	// Check divide by zero condition
	if (rhs == ZERO) {
		throw new exception("ERROR: Cannot divide by 0");
	}

	BigInteger righths = rhs;
	BigInteger remainder = ZERO;
	BigInteger quotient = *this;
	// Quotient could be same size as original numerator -- clear out with 0s, preserve size
	for (int i = 0; i < this->m_bits.size(); i++) {
		quotient.m_bits[i] = 0;
	}
	for (int i = this->BitLength() - 1; i >= 0; i -= 1) {
		remainder <<= 1;
		remainder.SetBit(0, this->GetBit(i));
		// .Abs() Is used to "pretend" we're still working with positive numbers
		if (remainder.Abs() >= righths.Abs()) {
			remainder -= righths.Abs();
			// WAS just i
			quotient.SetBit(i, true);
		}
	}
	// Final value relies on initial sign
	if (this->m_negative != righths.m_negative) {
		quotient.m_negative = true;
	}
	quotient.Resize();
	*this = quotient;
	return *this;
}

BigInteger BigInteger::Abs(){
	BigInteger ret = *this;
	ret.m_negative = false;
	return ret;
}

BigInteger BigInteger::InvertBits() {
	BigInteger ret = *this;
	for (int i = 0; i < m_bits.size(); i++) {
		ret.m_bits[i] = ~ret.m_bits[i];
	}
	return ret;
}

BigInteger BigInteger::Pow(BigInteger base, BigInteger exp) {
	BigInteger TWO(2);
	if (exp == ZERO) {
		return ONE;
	}
	BigInteger temp = Pow(base, exp >> 1);
	if (exp % TWO == ZERO) {
		return temp * temp;
	}
	return temp * temp * base;
}

BigInteger BigInteger::operator%(const BigInteger & rhs) const {
	BigInteger ret = *this;
	return ret %= rhs;
}

BigInteger & BigInteger::operator%=(const BigInteger & rhs) {
	// Find the first value that is less than the divisor
	while (*this >= rhs) {
		*this -= rhs;
	}
	// Returning the remainder
	return *this;

}

uint32_t BigInteger::Mod(int32_t a, int32_t b) {
	return (a % b + b) % b;
}

BigInteger & BigInteger::Resize() {
	vector<uint32_t>::iterator it = m_bits.end();
	// Iterate from end to beginning of the vector -- compare against uint0
	for (int i = m_bits.size() - 1; i > 0 && m_bits[i] == 0; i--) {
		m_bits.pop_back();
	}
	return *this;
}

// Compares whether one BigInteger is greater than to another. 
bool BigInteger::operator>(const BigInteger & rhs) const {
	bool gt = false;
	// If they aren't the same sign, positive is obviously greater.
	if (this->m_negative != rhs.m_negative) {
		if (this->m_negative) {
			return false;
		}
		else {
			return true;
		}
	}
	else {
		// Check the vector size 
		if (this->m_bits.size() > rhs.m_bits.size()) {
			return true;
		}
		else if (this->m_bits.size() < rhs.m_bits.size()) {
			return false;
		}
		else {
			// if the sizes are equal, iterate over each element starting from MSB
			for (int i = this->m_bits.size()-1; i >=0; i--) {
				// Only check if numbers are unequal
				if (this->m_bits[i] != rhs.m_bits[i]) {
					// Compare values
					if (this->m_bits[i] > rhs.m_bits[i]) {
						return true;
					}
					else {
						return false;
					}
				}
			}
		}
	}
	return gt;
}

// Compares whether one BigInteger is greater than/equal to another. 
bool BigInteger::operator>=(const BigInteger & rhs)const {
	return ((*this > rhs) || (*this == rhs));
}

// Compares whether one BigInteger is equal to another. 
bool BigInteger::operator==(const BigInteger & rhs) const {
	return ((this->m_negative == rhs.m_negative) && (this->m_bits == rhs.m_bits));
}

// Compares whether one BigInteger is NOT equal to another. 
bool BigInteger::operator!=(const BigInteger & rhs) const {
	return !((this->m_negative == rhs.m_negative) && (this->m_bits == rhs.m_bits));
}

// Compares whether one BigInteger is less than another. 
bool BigInteger::operator<(const BigInteger & rhs)const {
	return !(*this >= rhs);
}

// Compares whether one BigInteger is less than/equal to another. 
bool BigInteger::operator<=(const BigInteger & rhs)const {
	return !(*this > rhs);
}

// Function designed to interpret a BigInteger and return it as a String. 
string BigInteger::ToString() {
	std::stringstream stream;
	string result = "0x";

	if (this->m_negative) {
		result = "-0x";
	}
	// Iterate over bits, MSB -> LSB
	for (int i = this->m_bits.size() - 1; i >=0; i--) {
		stream.fill('0');
		stream.width(8);
		stream << hex << this->m_bits[i];
	}
	result += stream.str();
	result = result.erase(0, result.find_first_not_of('0'));
	return result;
}

// Overrides the ostream operator to better output the values of
// BigIntegers. 
ostream & operator<<(ostream & os, BigInteger & num) {
	os << num.ToString();
	return os;
}

uint32_t BigInteger::GetBit(uint32_t bitPos) {
	if (bitPos >= this->m_bits.size() * 32) {
		throw exception("Invalid bit position.");
	}
	uint32_t pos_in_vector = bitPos / 32;
	uint32_t pos_in_number = bitPos % 32;
	size_t number = (size_t)m_bits[pos_in_vector];

	return (number >> pos_in_number) & 1;
}

void BigInteger::SetBit(uint32_t bitPos, bool val) {
	if (bitPos >= this->m_bits.size() * 32) {
		throw exception("ERROR: Invalid bit position.");
	}
	uint32_t pos_in_vector = bitPos / 32;
	uint32_t pos_in_number = bitPos % 32;
	size_t value = (size_t)val;
	size_t number = (size_t)m_bits[pos_in_vector];

	m_bits[pos_in_vector] ^= (-((int)val) ^ number) & (1 << pos_in_number);
}

BigInteger BigInteger::operator<<(const BigInteger & rhs) const {
	BigInteger ret = *this;
	return ret <<= rhs;
}

BigInteger & BigInteger::operator<<=(const BigInteger & rhs) {
	uint32_t prevMSB = 0;
	uint32_t currMSB = 0;
	// Do process for as many times as rhs
	for (BigInteger i = ZERO; i < rhs; i++) {
		for (int j = 0; j < m_bits.size(); j++) {
			// Get value of bit 31
			if ((m_bits[j] & (1 << 31)) == 1) {
				currMSB = 1;
			}
			m_bits[j] <<= 1;
			// Set 0th bit to be 1 or 0
			m_bits[j] |= (prevMSB);
			prevMSB = currMSB;
			currMSB = 0;
		}
	}
	return *this;
}

BigInteger BigInteger::operator>>(const BigInteger & shift) const {
	BigInteger ret = *this;
	return ret >>= shift;
}

BigInteger & BigInteger::operator>>=(const BigInteger & rhs) {
	//uint32_t prevLSB = 0;
	//uint32_t currLSB = 0;
	// Do process for as many times as rhs
	for (BigInteger i = ZERO; i < rhs; i++) {
		uint32_t prevLSB = 0;
		uint32_t currLSB = 0;
		for (int j = this->m_bits.size() - 1; j >= 0; j--) {
			// Get value of bit 0
			if ((this->m_bits[j] & 1) == 1) {
				currLSB = 1;
			}
			// Shift the value
			this->m_bits[j] >>= 1;
			// Set 31st bit
			this->m_bits[j] |= (prevLSB << 31);
			// Retain the current LSB as now-previous LSB
			prevLSB = currLSB;
			currLSB = 0;
		}
	}
	return *this;
}

// Emulates a ModPow function for this BigInteger class
BigInteger BigInteger::ExpMod(BigInteger base, BigInteger exp, BigInteger mod) {
	BigInteger result = ONE;
	BigInteger TWO("2");

	while (exp > ZERO) {
		// Break down the base and keep it under mod value
		if (exp % TWO == ONE) {
			result = (result * base) % mod;
		}
		// Divide exponent by 2
		exp = exp >> ONE;
		// Update base 
		base = (base * base) % mod;
	}
	return result;
}


// This is POSTFIX operator
BigInteger & BigInteger::operator++(int)
{
	BigInteger temp = *this;
	*this += ONE;
	return temp;
}

// This is PREFIX operator
BigInteger & BigInteger::operator++()
{
	return *this += ONE;
}

BigInteger BigInteger::operator|(const BigInteger & rhs) const {
	BigInteger ret = *this;
	return ret |= rhs;
}

BigInteger & BigInteger::operator|=(const BigInteger & rhs)
{
	// TODO: What if a number is negative ????
	BigInteger shorterNum, longerNum;
	// Find the longer of two numbers
	if (rhs.m_bits.size() > this->m_bits.size()) {
		longerNum = rhs;
		shorterNum = *this;
	}
	else {
		longerNum = *this;
		shorterNum = rhs;
	}

	for (int i = 0; i < shorterNum.m_bits.size(); i++) {
		longerNum.m_bits[i] |= shorterNum.m_bits[i];
	}
	*this = longerNum;
	return *this;
}

BigInteger BigInteger::operator^(const BigInteger & rhs) const {
	BigInteger ret = *this;
	return ret ^= rhs;
}

BigInteger & BigInteger::operator^=(const BigInteger & rhs) {
	// TODO: What if a number is negative ????
	BigInteger shorterNum, longerNum;
	// Find the longer of two numbers
	if (rhs.m_bits.size() > this->m_bits.size()) {
		longerNum = rhs;
		shorterNum = *this;
	}
	else {
		longerNum = *this;
		shorterNum = rhs;
	}

	for (int i = 0; i < shorterNum.m_bits.size(); i++) {
		longerNum.m_bits[i] ^= shorterNum.m_bits[i];
	}
	*this = longerNum;
	return *this;
}

// Generates a random number within the bounds provided. Uses the Rabin-Miller
// primality test to determine how probable the prime found is to actually be prime. 
BigInteger BigInteger::GenerateLargePrimes(int lower, int upper) {
	BigInteger random;
	do {
		// Generate a prime in an interval
		random = lower + (rand() % (upper - lower + 1));
	} while (!RabinMillerTest(random, 5)); // 5 is arbitrary
	return random;
}

// Algorithm credit: http://www.sanfoundry.com/cpp-program-implement-miller-rabin-primality-test/
bool BigInteger::RabinMillerTest(BigInteger probPrime, int iterations) {
	BigInteger TWO("2");
	if (probPrime < TWO) {
		return false;
	}
	if (probPrime != TWO && probPrime % TWO == ZERO) {
		return false;
	}

	BigInteger oneLess = probPrime - ONE;
	// Find the first non-even residue  
	while (oneLess % TWO == ZERO) {
		oneLess >>= 1;	// divide by two
	}
	// Iterate over the prime x times to determine if it is prime. More iterations = more accuracy
	for (int i = 0; i < iterations; i++) {
		BigInteger random = rand();
		random = random % (probPrime - ONE) + ONE;
		BigInteger temp = oneLess;

		BigInteger modulus = ExpMod(random, temp, probPrime);

		while (temp != probPrime - ONE && modulus != ONE && modulus != probPrime - ONE) {
			modulus = (modulus * modulus) % probPrime;
			temp *= 2;	
		}
		if (modulus != probPrime - ONE && temp % TWO == ZERO) {
			return false;
		}
	}
	return true;
}

uint32_t BigInteger::BitLength() {
	uint32_t result = 0;
	uint32_t alreadyFull = 0;
	
	static const int MultiplyDeBruijnBitPosition[32] =
	{
		0, 9, 1, 10, 13, 21, 2, 29, 11, 14, 16, 18, 22, 25, 3, 30,
		8, 12, 20, 28, 15, 17, 24, 7, 19, 27, 23, 6, 26, 5, 4, 31
	};

	if (this->m_bits.size() > 1) {
		// These are "full" entries in the vector and can have their bits counted beforehand
		alreadyFull = (this->m_bits.size() - 1) * 32;
	}
	uint32_t temp = this->m_bits[this->m_bits.size() - 1];
	temp |= temp >> 1; // first round down to one less than a power of 2 
	temp |= temp >> 2;
	temp |= temp >> 4;
	temp |= temp >> 8;
	temp |= temp >> 16;

	// Pull out the factor -- already in hex
	result = MultiplyDeBruijnBitPosition[(uint32_t)(temp * 0x07C4ACDDU) >> 27];
	result += alreadyFull + 1;
	return result;
}