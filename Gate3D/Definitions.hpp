#pragma once
#include <stdint.h>
#include <vector>
#include <stdexcept>
#include <Windows.h>
#include <sstream>
#include <ostream>
#include <tuple>
#include <algorithm>

typedef uint32_t Color;
#define Debug OutputDebugStringA

template <typename T, unsigned int N>
class Matrix
{
private:
    T _values[N] = { 0 };

public:
    Matrix() {};
    Matrix(T values[N])
    {
        for (int i = 0; i < N; i++)
            this->_values[i] = values[i];
    };
    Matrix(const Matrix<T, N>& other)
    {
        for (int i = 0; i < N; i++)
            this->_values[i] = other[i];
    };
    template<typename... Args>
    Matrix(Args... args)
    {
        if (sizeof...(args) != N)
            throw std::out_of_range("Invalid number of arguments for constructor of matrix");

        T tempArray[N] = { args... };
        for (unsigned int i = 0; i < N; i++)
        {
            this->_values[i] = tempArray[i];
        }
    }

    const T& operator[](int index) const
    {
        if (index >= N)
            throw std::out_of_range("blah");
        else
            return this->_values[index];
    }
    T& operator[](int index)
    {
        if (index >= N)
            throw std::out_of_range("blah");
        else
            return this->_values[index];
    }
    Matrix<T, N> operator+(const Matrix<T, N>& addition) const
    {
        T vals[N] = { 0 };
        for (int i = 0; i < N; i++)
        {
            vals[i] = (*this)[i] + addition[i];
        }
        return Matrix<T, N>(vals);
    }
    void operator+=(const Matrix<T, N>& addition)
    {
        for (int i = 0; i < N; i++)
            this->_values[i] += addition[i];
    }
    bool operator==(const Matrix<T, N>& other) const
    {
        for (int i = 0; i < N; i++)
        {
            if (!((*this)[i] == other[i]))
                return false;
        }
        return true;
    }
    Matrix<T, N> operator*(const T& value) const
    {
        Matrix<T, N> mat;
        for (int i = 0; i < N; i++)
        {
            mat[i] = (*this)[i] * value;
        }
        return mat;
    }
    Matrix<T, N> operator-(const Matrix<T, N>& value) const
    {
        Matrix<T, N> mat;
        for (int i = 0; i < N; i++)
            mat[i] = (*this)[i] - value[i];
        return mat;
    }
    void operator*=(const T& value)
    {
        for (int i = 0; i < N; i++)
        {
            (*this)[i] = (*this)[i] * value;
        }
    }
    friend std::ostream& operator<<(std::ostream& os, const Matrix<T, N>& mat)
    {
        os << "(";
        for (int i = 0; i < N; i++)
            os << mat[i] << ", ";
        os << ")";
        return os;
    }
};

typedef Matrix<int, 3> Point;
typedef Matrix<int, 2> PixelCoordinate;
typedef std::vector<std::vector<Point>> ShapeData;

typedef struct Line
{
    Point a;
    Point b;
} Line;


class Face
{
private:
    std::vector<Point> _vertices{ 0 };

public:
    Face(std::vector<Point> vertices) : _vertices(vertices) {}
    std::vector<Line> getLines() const
    {
        std::vector<Line> lines;
        for (int i = 0; i < this->_vertices.size(); i++)
        {
            Line line{ this->_vertices[i], this->_vertices[(i + 1) % this->_vertices.size()] };
            lines.push_back(line);
        }
        return lines;
    }
    const std::vector<Point> getVertices() const { return this->_vertices; };
};
