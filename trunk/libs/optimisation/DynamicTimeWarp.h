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
        // store and init variables
        costs = costMatrix;
        moves = legalMoves;

        // check cost matrix
        nrows = costs.size();
        assert(nrows > 0);
        ncols = costs[0].size();
        assert(ncols > 0);
        for(int i = 0; i < nrows; i++)
            assert(static_cast<int>(costs[i].size()) == ncols);

        // initialise distance matrix and backpointers
        dists = std::vector<std::vector<T> >(nrows, std::vector<T>(ncols, 0));
        backs = std::vector<std::vector<int> >(nrows, std::vector<int>(ncols, -1));

        // compute [0:N-1][0:M-1]
        for(int r = 0; r < nrows; r++)
        {
            for(int c = 0; c < ncols; c++)
            {
                backs[r][c] = FindBestMove(r, c);
                dists[r][c] = ComputeMoveCost(r, c, backs[r][c]);
            }
        }
    }

    T FindWarpPath(std::vector<int> &rows, std::vector<int> &cols, const bool constrainFirstRow, const bool constrainFirstCol, const bool constrainLastRow, const bool constrainLastCol) const
    {
        // initialise backtrack
        T minDist = dists[nrows-1][ncols-1];
        rows = std::vector<int>(1, nrows-1);
        cols = std::vector<int>(1, ncols-1);

        // backtrack from [N-1][M-1] to [0][0]
        while(backs[rows.front()][cols.front()] >= 0)
        {
            const int r = rows.front();
            const int c = cols.front();
            const int m = backs[r][c];

            const int nr = r + moves[m].first;
            const int nc = c + moves[m].second;

            rows.insert(rows.begin(), nr);
            cols.insert(cols.begin(), nc);
        }

        // find start of partial match
        int begInd = 0;
        for(int i = 1; i < static_cast<int>(rows.size()); i++)
        {
            if((constrainFirstRow || rows[i] != 0) && (constrainFirstCol || cols[i] != 0))
            {
                begInd = i-1;
                break;
            }
        }

        // find end of partial match
        int endInd = rows.size();
        for(int i = begInd; i < static_cast<int>(rows.size()); i++)
        {
            if((!constrainLastCol && rows[i] == nrows-1) || (constrainLastRow && cols[i] == ncols-1))
            {
                endInd = i+1;
                break;
            }
        }

        // extract (partial) warp path and compute its cost
        T warpCost = dists[rows[endInd-1]][cols[endInd-1]];
        if(begInd > 0)
            warpCost -= dists[rows[begInd-1]][cols[begInd-1]];
        rows = std::vector<int>(rows.begin()+begInd, rows.begin()+endInd);
        cols = std::vector<int>(cols.begin()+begInd, cols.begin()+endInd);

        return warpCost;
    }

private:
    int nrows, ncols;

    std::vector<std::vector<T> > costs;
    std::vector<std::pair<int, int> > moves;

    std::vector<std::vector<T> > dists;
    std::vector<std::vector<int> > backs;

    // find the best move for a particular cell,
    // assume relevant cells in the distance matrix has been filed.
    int FindBestMove(const int r, const int c) const
    {
        // find the move with minimum cost
        T bestDist = 0;
        int bestMove = -1;
        for(unsigned int k = 0; k < moves.size(); k++)
        {
            const int nr = r + moves[k].first;
            const int nc = c + moves[k].second;

            if(nr >= 0 && nc >= 0)
                if(bestMove < 0 || dists[nr][nc] < bestDist)
                {
                    bestDist = dists[nr][nc];
                    bestMove = k;
                }
        }

        return bestMove;
    }

    // compute the partial path cost of moving to the cell [r][c] using move m
    T ComputeMoveCost(const int r, const int c, const int m) const
    {
        T d = costs[r][c];
        if(m >= 0)
        {
            const int pr = r + moves[m].first;
            const int pc = c + moves[m].second;
            d += dists[pr][pc];
        }

        return d;
    }
};


#endif
