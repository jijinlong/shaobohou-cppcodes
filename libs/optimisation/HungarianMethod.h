#ifndef HUNGARIAN_METHOD_H
#define HUNGARIAN_METHOD_H

// Implements the Kuhn-Munkres algorithm for optimal assignment problem
// Based on the course notes at http://csclab.murraystate.edu/bob.pilgrim/445/munkres.html
// 
// Shaobo Hou
// 11/10/2010

#include <vector>

class HungarianMethod
{
private:
	static bool Step4(const std::vector<std::vector<float> > &C, std::vector<std::vector<int> > &M, std::vector<int> &rows_covered, std::vector<int> &cols_covered, int &zero_row, int &zero_col);
	static void Step5(std::vector<std::vector<int> > &M, std::vector<int> &rows_covered, std::vector<int> &cols_covered, int zero_row, int zero_col);
	static void Step6(std::vector<std::vector<float> > &C, const std::vector<int> &rows_covered, const std::vector<int> &cols_covered);

	// find a zero whose row and column are both uncovered, and record its index
	static void FindZero(const std::vector<std::vector<float> > &C, const std::vector<int> &rows_covered, const std::vector<int> &cols_covered, int &zero_row, int &zero_col);
	// find and return the column index of starred zero in row zero_row
	static int FindStar(const std::vector<std::vector<int> > &M, int zero_row);

public:
	static std::vector<int> MunkresAlgorithm(const std::vector<std::vector<float> > &costs);
};

#endif
