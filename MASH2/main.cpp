#include "BigInteger.h"
#include "MASH2.h"
#include <iostream>
using std::cout;
using std::cin;

int main() {
	//string input;
	////cout << "Enter a prime: ";
	////cin >> input;
	//BigInteger top("11");
	//BigInteger bottom("-E");
	//BigInteger result = (top.InvertBits() + BigInteger("1"));
	//string neg = "Nah";
	//if (result.GetBit(result.BitLength() - 1) == 1)
	//	neg = "True!";
	//cout << neg << ' ' << (top.InvertBits() + BigInteger("1")).ToString() << std::endl;
	//top |= bottom;
	//cout << top << std::endl;
	//cout << (top - BigInteger("1")).InvertBits().ToString() << std::endl;
	///*if (p.RabinMillerTest(p, 5))
	//	cout << "Success";
	//else
	//	cout << "Fail";*/

	MASH2 mash2 = MASH2();
	mash2.Digest("Help");
	return 0;
}


