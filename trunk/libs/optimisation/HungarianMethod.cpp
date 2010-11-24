#include "HungarianMethod.h"

#include <vector>
#include <numeric>
#include <algorithm>

using std::vector;

/**
 * \brief top level Munkres algorithm function
 */
vector<int> HungarianMethod::MunkresAlgorithm(const vector<vector<float> > &costs)
{
	const int N = costs.size();
	vector<vector<float> > C = costs;


	// step 1, subtract the minimum value of each row from each element in the row
	for(int i = 0; i < N; i++)
	{
		float minval = *std::min_element(C[i].begin(), C[i].end());
		for(int j = 0; j < N; j++)
			C[i][j] -= minval;
	}


	// step 2, star a zero in the cost matrix if there are no other zeros on the same row or column
	vector<vector<int> > M(N, vector<int>(N, 0));
	vector<int> rows_covered(N, 0);
	vector<int> cols_covered(N, 0);
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			if(abs(C[i][j]) < 1e-9 && rows_covered[i] == 0 && cols_covered[j] == 0)
			{
				M[i][j] = 1;
				rows_covered[i] = 1;
				cols_covered[j] = 1;
			}
	rows_covered = vector<int>(N, 0);
	cols_covered = vector<int>(N, 0);


	bool done = false;
	bool step3and5 = true;
	while(!done)
	{
		// step 3, cover all columns with starred zeros and terminate if solution is found
		if(step3and5)
		{
			for(int i = 0; i < N; i++)
				for(int j = 0; j < N; j++)
					if(M[i][j] == 1)
						cols_covered[j] = 1;

			// if the perfect solution found, terminate the algorithm
			int count = accumulate(cols_covered.begin(), cols_covered.end(), 0);
			if(count >= N)
			{
				done = true;
				break;
			}
		}


		// step 4, look for a noncovered zero and prime it
		int zero_row = -1;
		int zero_col = -1;
		step3and5 = Step4(C, M, rows_covered, cols_covered, zero_row, zero_col);
		int bah4 = 0;


		// step 5 or 6
		if(step3and5)
			Step5(M, rows_covered, cols_covered, zero_row, zero_col);
		else
			Step6(C, rows_covered, cols_covered);
	}


	// extract final mapping
	vector<int> cols(N, -1);
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			if(M[i][j] == 1)
			{
				cols[i] = j;
				break;
			}

	return cols;
}

/**
 * \brief look for a noncovered zero and prime it
 */
bool HungarianMethod::Step4(const vector<vector<float> > &C, vector<vector<int> > &M, vector<int> &rows_covered, vector<int> &cols_covered, int &zero_row, int &zero_col)
{
	bool done = false;
	while(!done)
	{
		zero_row = -1;
		zero_col = -1;
		FindZero(C, rows_covered, cols_covered, zero_row, zero_col);

		if(zero_row < 0)	// no uncovered zero found
		{
			done = true;
			return false;	// should go to step 6
		}
		else
		{
			M[zero_row][zero_col] = 2;	// prime the zero;

			int star_col = FindStar(M, zero_row);
			if(star_col >= 0)	// find an starred zero on row zero_row, cover row zero_row and uncover column star_col
			{
				rows_covered[zero_row] = 1;
				cols_covered[star_col] = 0;
			}
			else
				done = true;	// should go to step 5
		}
	}

	return true; // should go to step 5
}

/**
 * \brief construct alternating series of primed and starred zeros
 */
void HungarianMethod::Step5(vector<vector<int> > &M, vector<int> &rows_covered, vector<int> &cols_covered, int zero_row, int zero_col)
{
	const int N = M.size();

	vector<int> stars_in_col(1, zero_row);
	vector<int> primes_in_row(1, zero_col);

	bool done = false;
	while(!done)		// terminate at a primed zero with starred zero in its column
	{
		// find starred zero in the column
		int star_row = -1;
		int curr_col = primes_in_row.back();
		for(int i = 0; i < N; i++)
			if(M[i][curr_col] == 1)
			{
				star_row = i;
				break;
			}

		if(star_row >= 0)
		{
			stars_in_col.push_back(star_row);
			primes_in_row.push_back(primes_in_row.back());
		}
		else
			done = true;

		if(!done)
		{
			// find primed zero in the row
			int curr_row = stars_in_col.back();
			int prime_col = -1;
			for(int j = 0; j < N; j++)
				if(M[curr_row][j] == 2)
				{
					prime_col = j;
					break;
				}

			stars_in_col.push_back(stars_in_col.back());
			primes_in_row.push_back(prime_col);
		}
	}

	// convert path, unstar starred zeros, and star primed zeros
	for(unsigned int i = 0; i < stars_in_col.size(); i++)
	{
		int r = stars_in_col[i];
		int c = primes_in_row[i];
		if(M[r][c] == 1)
			M[r][c] = 0;
		else
			M[r][c] = 1;
	}
	// clear covers
	rows_covered = vector<int>(rows_covered.size(), 0);
	cols_covered = vector<int>(cols_covered.size(), 0);
	// unprime remaining primed zeros
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
			if(M[i][j] == 2)
				M[i][j] = 0;

	// should return to step 3
}

/**
 * \brief subtract the minimum uncovered value from covered rows and add it to uncovered columns
 */
void HungarianMethod::Step6(vector<vector<float> > &C, const vector<int> &rows_covered, const vector<int> &cols_covered)
{
	const int N = C.size();

	// find minimum uncovered value
	float minval = std::numeric_limits<float>::max();
	for(int j = 0; j < N; j++)
		if(cols_covered[j] == 0)
			for(int i = 0; i < N; i++)
				if(rows_covered[i] == 0)
					if(C[i][j] < minval)
						minval = C[i][j];

	// subtract it from covered rows and add it to uncovered columns
	for(int i = 0; i < N; i++)
		for(int j = 0; j < N; j++)
		{
			if(rows_covered[i] == 1)
				C[i][j] += minval;
			if(cols_covered[j] == 0)
				C[i][j] -= minval;
		}

	// should return to step 4
}


/**
 * \brief find a zero whose row and column are both uncovered, and record its index
 */
void HungarianMethod::FindZero(const vector<vector<float> > &C, const vector<int> &rows_covered, const vector<int> &cols_covered, int &zero_row, int &zero_col)
{
	const int N = C.size();

	zero_row = -1;
	zero_col = -1;
	bool found_zero = false;
	for(int j = 0; j < N; j++)
	{
		if(cols_covered[j] == 0)
			for(int i = 0; i < N; i++)
				if(rows_covered[i] == 0)
					if(abs(C[i][j]) < 1e-9)
					{
						zero_row = i;
						zero_col = j;
						found_zero = true;
						break;
					}
		if(found_zero)
			break;
	}
}


/**
 * \brief find and return the column index of starred zero in row zero_row
 */
int HungarianMethod::FindStar(const vector<vector<int> > &M, int zero_row)
{
	const int N = M.size();

	for(int j = 0; j < N; j++)
		if(M[zero_row][j] == 1)
			return j;

	return -1;
}
