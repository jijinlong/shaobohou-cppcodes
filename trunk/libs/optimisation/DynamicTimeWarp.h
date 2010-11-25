#ifndef DYNAMIC_TIME_WARP_H
#define DYNAMIC_TIME_WARP_H


template <typename T>
int min_element_index(const std::vector<T> &v)
{
    if(v.size() == 0)
        return -1;

    int min_ind = 0;
    for(unsigned int i = 1; i < v.size(); i++)
        if(v[i] < v[min_ind])
            min_ind = i;

    return min_ind;
}

template <typename T>
int max_element_index(const std::vector<T> &v)
{
    if(v.size() == 0)
        return -1;

    int max_ind = 0;
    for(unsigned int i = 1; i < v.size(); i++)
        if(v[i] > v[max_ind])
            max_ind = i;

    return max_ind;
}


template <typename T>
class DynamicTimeWarp
{
public:
    DynamicTimeWarp()
    {
        nrows = 0;
        ncols = 0;
    }

    DynamicTimeWarp(const std::vector<std::vector<T> > &costMatrix, const std::vector<std::pair<int, int> > &legalMoves)
    {
        costs = costMatrix;
        moves = legalMoves;

        // check cost matrix
        nrows = costs.size();
        assert(nrows > 0);
        ncols = costs[0].size();
        assert(ncols > 0);
        for(int i = 0; i < nrows; i++)
            assert(static_cast<int>(costs[i].size()) == ncols);

        // dynamic time warp
        dists = std::vector<std::vector<T> >(nrows, std::vector<T>(ncols, 0));
        backs = std::vector<std::vector<int> >(nrows, std::vector<int>(ncols, -1));
        for(int r = 0; r < nrows; r++)
            for(int c = 0; c < ncols; c++)
            {
                dists[r][c] = costs[r][c];

                // find the move with minimum cost
                int bestMove = -1;
                T bestDist = 0;
                for(unsigned int k = 0; k < moves.size(); k++)
                {
                    const int nr = r + moves[k].first;
                    const int nc = c + moves[k].second;

                    if(nr >= 0 && nc >= 0)
                        if(bestMove < 0 || dists[nr][nc] < bestDist)
                        {
                            bestMove = k;
                            bestDist = dists[nr][nc];
                        }
                }

                // update grid
                backs[r][c] = bestMove;
                dists[r][c] += bestDist;
            }
    }

    T FindWarpPath(std::vector<int> &rows, std::vector<int> &cols, const bool constrainFirstRow, const bool constrainFirstCol, const bool constrainLastRow, const bool constrainLastCol) const
    {
        // initialise backtrack
        T minDist = dists[nrows-1][ncols-1];
        rows = std::vector<int>(1, nrows-1);
        cols = std::vector<int>(1, ncols-1);

        // if end point is NOT constrained to the last column
        if(!constrainLastCol)
        {
            int minDistCol = min_element_index(dists.back());
            if(dists[nrows-1][minDistCol] < minDist)
            {
                minDist = dists[nrows-1][minDistCol];
                rows.back() = nrows-1;
                cols.back() = minDistCol;
            }
        }
        
        // if end point is not constrained to the last row
        if(!constrainLastRow)
        {
            for(int r = 0; r < nrows; r++)
            {
                if(dists[r][ncols-1] < minDist)
                {
                    minDist = dists[r][ncols-1];
                    rows.back() = r;
                    cols.back() = ncols-1;
                }
            }
        }

        // backtrack
        T initDist = 0;
        while(backs[rows.front()][cols.front()] >= 0)
        {
            const int r = rows.front();
            const int c = cols.front();
            const int m = backs[r][c];

            const int nr = r + moves[m].first;
            const int nc = c + moves[m].second;

             // terminate backtrack if not constrained to start on the first row and already reached the first column
            if(!constrainFirstRow && cols.front() == 0)
            {
                initDist = dists[nr][nc];
                break;
            }

            // terminate backtrack if not constrained to start on the first column and already reached the first row
            if(!constrainFirstCol && rows.front() == 0)
            {
                initDist = dists[nr][nc];
                break;
            }

            rows.insert(rows.begin(), nr);
            cols.insert(cols.begin(), nc);
        }

        const T warpCost = dists[rows.back()][cols.back()] - initDist;

        return warpCost;
    }

private:
    int nrows, ncols;

    std::vector<std::vector<T> > costs;
    std::vector<std::pair<int, int> > moves;

    std::vector<std::vector<T> > dists;
    std::vector<std::vector<int> > backs;
};


#endif
