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

template <typename T>
struct has_subscript_operator {
    // We use SFINAE to check if the expression decltype(std::declval<T>()[0]) is valid
    template <typename U>
    static std::true_type test(decltype(std::declval<U>()[0])*);

    // Fallback for invalid expressions
    template <typename U>
    static std::false_type test(...);

    // A boolean constant indicating whether T has the [] operator
    static constexpr bool value = decltype(test<T>(nullptr))::value;
};

template <typename T, unsigned int N>
class Matrix
{
private:
    T _values[N] = {};
    template <typename Other, typename BinaryOp>
    friend static void _transform(Matrix<T, N>& transformer, const Other& other, BinaryOp op) {
        for (unsigned int i = 0; i < N; i++) {
            if constexpr (has_subscript_operator<Other>::value) {
                // If other has operator[] and op can be invoked with T& and T::value_type
                transformer._values[i] = op(transformer._values[i], (const T&)other[i]);
            }
            else {
                // Fallback to non-operator[] case
                transformer._values[i] = op(transformer._values[i], (const T&)other);
            }
        }
    }


public:
    Matrix() {};
    // Matrix<float, N>() : _values({ 0.0 }) {};
    template <typename Other>
    Matrix(const Other& other)
    {
        *this = other;
    }
    template<typename... Args>
    Matrix(Args... args)
    {
        static_assert(sizeof...(args) == N, "Invalid number of arguments for constructor of matrix");
        // static_assert(std::is_same_v<T, Args...>, "Arguments must be of type T");
        T tempArray[N] = { T(args)... };
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
    template <typename Other>
    Matrix<T, N> operator/(const Other& divisor) const
    {
        Matrix<T, N> ret(*this);
        _transform(ret, divisor, [](const T& a, const T& b) {return a / b; });
        return ret;
    }
    template <typename Other>
    Matrix<T, N>& operator=(const Other& other)
    {
        _transform(*this, other, [](const T&, const T& other) {return other; });
        return *this;
    }
    Matrix<T, N> operator+(const Matrix<T, N>& addition) const
    {
        Matrix<T, N> ret(*this);
        _transform(ret, addition, std::plus<T>());
        return ret;
    }
    void operator+=(const Matrix<T, N>& addition)
    {
        _transform(*this, addition, std::plus<T>());
    }
    bool operator==(const Matrix<T, N>& other) const
    {
        for (int i = 0; i < N; i++)
        {
            if ((*this)[i] != other[i])
                return false;
        }
        return true;
    }
    bool operator!=(const Matrix<T, N>& other) const
    {
        for (int i = 0; i < N; i++)
        {
            if ((*this)[i] == other[i])
                return false;
        }
        return true;
    }
    template <typename Other>
    Matrix<T, N> operator*(const Other& other) const
    {
        Matrix<T, N> mat(*this);
        _transform(mat, other, std::multiplies<T>());
        return mat;
    }
    Matrix<T, N> operator*(const Matrix<Matrix<T, N>, N>& multiplyingMatrix) const
    {
        Matrix<T, N> mat;
        for (int i = 0; i < N; i++)
        {
            T sum = 0;
            for (int j = 0; j < N; j++)
            {
                sum += (*this)[j] * multiplyingMatrix[j][i];
            }
            mat[i] = sum;
        }
        return mat;
    }
    template <typename Other>
    Matrix<T, N> operator-(const Other& value) const
    {
        Matrix<T, N> mat(*this);
        _transform(mat, value, std::minus<T>());
        return mat;
    }
    template <typename Other>
    void operator*=(const Other& value)
    {
        _transform(*this, value, std::multiplies<T>());
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

typedef Matrix<float, 3> Point;
typedef Matrix<float, 3> Rotation;
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
    std::vector<Point> _vertices{ 0.0 };

public:
    Face(std::vector<Point> vertices) : _vertices(vertices) {}
    std::vector<Line> getLines() const
    {
        std::vector<Line> lines;
        for (unsigned int i = 0; i < this->_vertices.size(); i++)
        {
            Line line{ this->_vertices[i], this->_vertices[(i + 1) % this->_vertices.size()] };
            lines.push_back(line);
        }
        return lines;
    }
    const std::vector<Point> getVertices() const { return this->_vertices; };
};
