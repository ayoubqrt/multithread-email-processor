#include <include/Vector.h>

vector<string> slicing(vector<string> &arr, int X, int Y)
{
	auto start = arr.begin() + X;
	auto end = arr.begin() + Y + 1;

	vector<string> result(Y - X + 1);

	copy(start, end, result.begin());

	return result;
}