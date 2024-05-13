#ifndef INC_23_HOME_1_MATRIX_H
#define INC_23_HOME_1_MATRIX_H

#include <iostream>
#include <stdexcept>
#include <vector>

template <typename T>
class Matrix {
 private:
    unsigned long columns;
    unsigned long size;

    // We store lines each after each. l - line, c - column [l1c1, l1c2, /* new line elements next */ l2c1, l2c2]
    T *array;

    void requireSameDimensions(const Matrix &another) const {
        if (this->size != another.size || this->columns != another.columns) {
            throw std::invalid_argument("Matrix is not compatible for operation");
        }
    }

    void requireRow(int row) const {
        if (row < 0 || row >= (this->size / this->columns)) {
            throw std::invalid_argument("Matrix out of bounds");
        }
    }

    void requireColumn(int column) const {
        if (column >= this->columns || column < 0) {
            throw std::invalid_argument("Matrix out of bounds");
        }
    }

 public:
    Matrix(unsigned long rows, unsigned long columns) {
        this->array = new T[rows * columns];
        this->columns = columns;
        this->size = columns * rows;

        // reset memory after malloc
        for (int i = 0; i < size; ++i) {
            this->array[i] = (T) 0;
        }
    }

    explicit Matrix(std::istream &stream) {
        Matrix deserialized = deserialize(stream);
        this->array = deserialized.array;
        this->columns = deserialized.columns;
        this->size = deserialized.size;
    }

    // Copy constructor
    Matrix(const Matrix<T> &other) : columns(other.columns), size(other.size), array(new T[other.size]) {
        std::copy(other.array, other.array + other.size, array);
    }

    // Move constructor
    Matrix(Matrix<T> &&other) noexcept: columns(other.columns), size(other.size), array(other.array) {
        other.array = nullptr;
        other.size = 0;
        other.columns = 0;
    }

    ~Matrix() {
        delete[] this->array;
    }

    static Matrix<T> createZeroMatrix(int rows, int columns) {
        Matrix<T> matrix(rows, columns);
        return matrix;
    }

    static Matrix<T> createIdentityMatrix(int rows) {
        Matrix<T> matrix(rows, rows);
        for (int i = 0; i < rows; ++i) {
            matrix.setValue(i, i, (T) 1);
        }
        return matrix;
    }

    static Matrix<T> deserialize(std::istream &stream) {
        int columns, rows;
        stream >> rows;
        stream >> columns;

        Matrix<T> matrix(rows, columns);
        columns *= rows;
        for (int i = 0; i < columns; ++i) {
            stream >> matrix.array[i];
        }
        return matrix;
    }

    static void serialize(std::ostream &stream, const Matrix &matrix) {
        for (int i = 0; i < matrix.getSize(); ++i) {
            stream << matrix.getValue(i);
            if ((i + 1) % matrix.getColumns() == 0) {
                stream << std::endl;
            } else {
                stream << "\t";
            }
        }
    }

    friend std::ostream &operator<<(std::ostream &os, const Matrix &obj) {
        Matrix::serialize(os, obj);
        return os;
    }

    friend std::istream &operator>>(std::istream &is, Matrix &obj) {
        obj = Matrix::deserialize(is);
        return is;
    }

    [[nodiscard]] int getRows() const {
        return this->size / this->columns;
    }

    [[nodiscard]] int getColumns() const {
        return this->columns;
    }

    [[nodiscard]] int getSize() const {
        return this->size;
    }

    [[nodiscard]] unsigned long getInternalIndex(int row, int column) const {
        this->requireColumn(column);
        this->requireRow(row);

        return ((unsigned long) row) * this->columns + column;
    }

    /**
     * Force-insert value to matrix based on index. Index could be calculated by Matrix::getInternalIndex
     *
     * @param index Internal index calculated by Matrix::getInternalIndex
     * @param value The value
     */
    void setValue(int index, T value) {
        if (index < 0 || index >= this->size) {
            throw std::invalid_argument("Matrix out of bounds");
        }
        this->array[index] = value;
    }

    void setValue(int row, int column, T value) {
        this->array[this->getInternalIndex(row, column)] = value;
    }

    /**
     * Force-get value from matrix based on index. Index could be calculated by Matrix::getInternalIndex
     *
     * @param index Internal index calculated by Matrix::getInternalIndex
     * @param value The value
     */
    [[nodiscard]] T getValue(int index) const {
        if (index < 0 || index >= this->size) {
            throw std::invalid_argument("Matrix out of bounds");
        }
        return this->array[index];
    }

    [[nodiscard]] T getValue(int row, int column) const {
        return this->array[this->getInternalIndex(row, column)];
    }

    /**
     * Get a matrix copy for further changes
     * @return A matrix with same dimensions and same elements
     */
    [[nodiscard]] Matrix<T> clone() const {
        Matrix<T> matrix(this->getRows(), this->columns);
        for (int i = 0; i < this->size; ++i) {
            matrix.array[i] = this->array[i];
        }
        return matrix;
    }

    /**
     * Get a matrix minor for element
     * @param row Removing row
     * @param column Removing column
     * @return
     */
    [[nodiscard]] Matrix<T> getMinor(int row, int column) const {
        requireRow(row);
        requireColumn(column);
        if (this->size == this->columns || this->columns == 1) {
            throw std::invalid_argument("Cannot extract minor from matrix of 1 dimension");
        }

        Matrix<T> matrix(this->getRows() - 1, this->columns - 1);
        int jumpIndex = this->columns * row;
        for (int i = 0, j = 0; i < matrix.size; ++i, ++j) {
            // skip row that we do not need
            if (j == jumpIndex) {
                j += this->columns;
            }
            // skip column that we do not need
            if ((j % this->columns) == column) {
                // here we use i-- not j++ because we may want to jump from new row that we got
                // (execute row skip check just here), it could be achieved if we forward j increment
                // to for loop (for loop increment both i and j, so we decrement i)
                i--;
                continue;
            }

            matrix.array[i] = this->array[j];
        }
        return matrix;
    }

    /**
     * Get determinate for cube matrix (Matrix::isSquare)
     * @return double determinant value
     * @throws invalid_argument if matrix is not cube
     */
    T getDeterminant() const { // NOLINT
        if (!isSquare()) {
            throw std::invalid_argument("Matrix is not cube");
        }

        if (this->size == 1) {
            return this->array[0];
        } else if (this->size == 4) {
            return this->array[0] * this->array[3] - this->array[1] * this->array[2];
        } else {
            T value;
            for (int i = 0; i < this->columns; ++i) {
                if (this->array[i] != 0) {
                    T middle = (i % 2 == 0 ? 1 : -1)
                               * this->array[i] * this->getMinor(0, i).getDeterminant();
                    if (i == 0) {
                        value = middle;
                    } else {
                        value += middle;
                    }
                }
            }
            return value;
        }
    }

    /**
     * Get inverted matrix for current matrix
     * @return Matrix inverted ( Matrix^-1 )
     * @throws invalid_argument if matrix determinant is zero
     */
    Matrix<T> getInverted() const {
        double determinant = this->getDeterminant();
        if (determinant == 0) {
            throw std::invalid_argument("Matrix cannot be inverted");
        }

        int rows = this->getRows();
        Matrix<T> additions(rows, this->columns);
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < additions.columns; ++j) {
                additions.setValue(i, j, ((i + j) % 2 == 0 ? 1 : -1)
                                         * this->getMinor(i, j).getDeterminant());
            }
        }
        return additions.transposed() / determinant;
    }

    bool isSquare() const {
        return this->columns * this->columns == this->size;
    }

    Matrix<T> operator+(const Matrix<T> &another) const {
        this->requireSameDimensions(another);
        Matrix<T> matrix(this->getRows(), this->columns);
        for (int i = 0; i < this->size; ++i) {
            matrix.array[i] = this->array[i] + another.array[i];
        }
        return matrix;
    }

    Matrix<T> operator-() const {
        return (*this) * ((T) -1);
    }

    Matrix<T> operator-(const Matrix<T> &another) const {
        Matrix<T> copy = another.clone();
        return *this + (-copy);
    }

    Matrix<T> &operator=(const Matrix<T> &another) {
        // self-assignment check
        if (this == &another) {
            return *this;
        }

        requireSameDimensions(another);
        for (int i = 0; i < this->size; ++i) {
            this->array[i] = another.array[i];
        }
        return *this;
    }

    bool operator==(const Matrix<T> &another) const {
        if (this->size != another.size || this->columns != another.columns) {
            return false;
        }

        for (int i = 0; i < this->size; ++i) {
            if (another.array[i] != this->array[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const Matrix<T> &another) const {
        return !(*this == another);
    }

    bool operator==(const T &number) const {
        if (!this->isSquare()) {
            return false;
        }

        for (int i = 0; i < this->size; ++i) {
            if ((i / columns == i % columns ? number : 0) != this->array[i]) {
                return false;
            }
        }
        return true;
    }

    bool operator!=(const T &number) const {
        return !(*this == number);
    }

    Matrix<T> operator*(const Matrix<T> &another) const {
        if (this->columns != another.getRows()) {
            throw std::invalid_argument("Matrix cannot be multiplied (col1 != row2)");
        }

        Matrix<T> matrix(this->getRows(), another.columns);
        for (int i = 0; i < matrix.getSize(); ++i) {
            int column = i % matrix.columns;
            int row = i / matrix.columns;

            T value;
            for (int j = 0; j < this->columns; ++j) {
                T middle = this->getValue(row, j) * another.getValue(j, column);
                if (j == 0) {
                    value = middle;
                } else {
                    value += middle;
                }
            }
            matrix.array[i] = value;
        }
        return matrix;
    }

    Matrix<T> operator*(const T &another) const {
        Matrix<T> matrix(this->getRows(), this->columns);
        for (int i = 0; i < this->size; ++i) {
            matrix.array[i] = this->array[i] * another;
        }
        return matrix;
    }

    Matrix<T> operator/(const T &another) const {
        Matrix<T> matrix(this->getRows(), this->columns);
        for (int i = 0; i < this->size; ++i) {
            matrix.array[i] = this->array[i] / another;
        }
        return matrix;
    }

    Matrix operator!() const {
        return this->getInverted();
    }

    /**
     * Change rows for current matrix
     * @param originRow first row
     * @param targetRow second row
     */
    void changeRows(int originRow, int targetRow) {
        this->requireRow(originRow);
        this->requireRow(targetRow);

        int lastIndex = this->columns * (originRow + 1);
        int offsetIndex = (targetRow - originRow) * this->columns;
        for (int i = originRow * this->columns; i < lastIndex; ++i) {
            int j = i + offsetIndex;
            double originValue = this->array[j];
            this->array[j] = this->array[i];
            this->array[i] = originValue;
        }
    }

    /**
     * Add one row to other multiplied by a value for current matrix
     * @param originRow Row to be added
     * @param targetRow Row where we add first row
     * @param multiplier The multiplier
     */
    void addRowToAnother(int originRow, int targetRow, T multiplier) {
        this->requireRow(originRow);
        this->requireRow(targetRow);

        int lastIndex = this->columns * (targetRow + 1);
        int offsetIndex = (originRow - targetRow) * this->columns;
        for (int i = targetRow * this->columns; i < lastIndex; ++i) {
            this->array[i] += multiplier * this->array[i + offsetIndex];
        }
    }

    /**
     * Multiply row in current matrix
     * @param row Row to be multiplier
     * @param multiplier The multiplier
     */
    void multiplyRow(int row, T multiplier) {
        this->requireRow(row);

        int lastIndex = this->columns * (row + 1);
        for (int i = row * this->columns; i < lastIndex; ++i) {
            this->array[i] *= multiplier;
        }
    }

    [[nodiscard]] Matrix<T> transposed() const {
        Matrix<T> matrix(this->columns, this->getRows()); // NOLINT
        for (int i = 0; i < this->columns; ++i) {
            for (int j = 0; j < matrix.columns; ++j) {
                matrix.setValue(i, j, this->getValue(j, i));
            }
        }
        return matrix;
    }
};

#endif //INC_23_HOME_1_MATRIX_H
