#pragma once
#include <exception>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ostream>
#include <random>
using std::exception;
using std::string;
using std::vector;
using std::ostream;
using std::hex;
using std::reverse;
using std::rand;

#define BITS_IN_INTEGER = 32;
#define HEX_IN_32_BITS = 8;


class BigInteger {
	// Ostream Operator
	friend ostream & operator<<(ostream & os, BigInteger & num);
public:
	// Constructors 
	BigInteger();
	BigInteger(string hex);
	BigInteger(int num);
	BigInteger(const BigInteger & rhs);
	BigInteger & operator=(const BigInteger & rhs);

	// Manipulation Operators
	BigInteger operator+(const BigInteger & rhs) const;
	BigInteger & operator+=(const BigInteger & rhs) ;
	BigInteger operator-(const BigInteger & rhs) const;
	BigInteger & operator-=(const BigInteger & rhs) ;
	BigInteger operator*(const BigInteger & rhs) const;
	BigInteger & operator*=(const BigInteger & rhs) ;
	BigInteger operator/(const BigInteger & rhs) const;
	BigInteger & operator/=(const BigInteger & rhs);
	BigInteger operator>>(const BigInteger & rhs) const;
	BigInteger & operator>>=(const BigInteger & num);
	BigInteger operator<<(const BigInteger & rhs) const;
	BigInteger & operator<<=(const BigInteger & num);
	BigInteger operator%(const BigInteger & rhs) const;
	/// Added ampersand
	BigInteger & operator%=(const BigInteger & rhs);
	BigInteger operator|(const BigInteger & rhs) const;
	BigInteger & operator|=(const BigInteger & rhs);
	BigInteger operator^(const BigInteger & rhs) const;
	BigInteger & operator^=(const BigInteger & rhs);


	BigInteger & operator++(int);
	BigInteger & operator++();

	// Helper Functions
	BigInteger Abs();
	BigInteger Pow(BigInteger base, BigInteger exp);
	uint32_t Mod(int32_t a, int32_t b);
	vector<uint32_t> HexToBinary(string hex);
	string ToString();
	void SetBit(uint32_t bitPos, bool value);
	uint32_t GetBit(uint32_t bitPos);
	BigInteger & Resize();
	BigInteger ExpMod(BigInteger base, BigInteger exp, BigInteger mod);
	uint32_t BitLength();
	BigInteger InvertBits();

	// Comparison Operators
	bool operator>(const BigInteger & rhs) const;
	bool operator>=(const BigInteger & rhs) const;
	bool operator<(const BigInteger & rhs) const;
	bool operator<=(const BigInteger & rhs) const;
	bool operator==(const BigInteger & rhs) const;
	bool operator!=(const BigInteger & rhs) const;

	BigInteger GenerateLargePrimes(int lower, int upper);
	bool RabinMillerTest(BigInteger probPrime, int iterations);

private:
	vector<uint32_t> m_bits;
	bool m_negative;
};