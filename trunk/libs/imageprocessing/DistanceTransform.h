#ifndef DISTANCE_TRANSFORM_H
#define DISTANCE_TRANSFORM_H

template <typename T>
T square(const T &a)
{
    return a*a;
}

template <typename T>
Array2D<T> dt1(const Array2D<T> &f)
{
    const int n = f.size();

    Array2D<T> d(1, n);
    Array1D<int> v(n);
    Array1D<T> z(n+1);

    v[0] = 0;
    z[0] = -std::numeric_limits<T>::max();
    z[1] = +std::numeric_limits<T>::max();

    int k = 0;
    for (int q = 1; q <= n-1; q++)
    {
        T s = ((f(q)+square(q)) - (f(v[k])+square(v[k]))) / (2*q-2*v[k]);
        while (s <= z[k])
        {
            k--;
            s = ((f(q)+square(q)) - (f(v[k])+square(v[k]))) / (2*q-2*v[k]);
        }

        k++;
        v[k] = q;
        z[k] = s;
        z[k+1] = +std::numeric_limits<T>::max();
    }

    k = 0;
    for (int q = 0; q <= n-1; q++)
    {
        while (z[k+1] < q)
            k++;
        d(q) = square(q-v[k]) + f(v[k]);
    }

  return d;
}

// mat should have 0 at locations that are "on" and realmax everywhere else
template <typename T>
Array2D<T> dt2(const Array2D<T> &mat)
{
    Array2D<T> dist(mat.dim1(), mat.dim2());

    for(int i = 0; i < mat.dim1(); i++)
        setrow(dist, i, dt1(getrow(mat, i)));
    for(int i = 0; i < mat.dim2(); i++)
        setcol(dist, i, transpose(dt1(getcol(dist, i))));

    return dist;
}

#endif
