
#ifndef LETTER_MATH
#define LETTER_MATH

#include <string>

class LetterMath {
 private:
    std::string entry;

 public:
    LetterMath() {
        this->entry = "";
    }

    LetterMath(const std::string &origin) {
        this->entry = origin;
    }

    LetterMath(const LetterMath& other) : entry(other.entry) {
    }

    LetterMath& operator=(const LetterMath& other) {
        if (this != &other) {
            entry = other.entry;
        }
        return *this;
    }

    void operator+=(const LetterMath &another) {
        this->entry = (*this + another).entry;
    }

    LetterMath operator+(const LetterMath &another) const {
        return {this->entry.empty() ? another.entry : "(" + this->entry + ")+" + another.entry};
    }

    LetterMath operator-(const LetterMath &another) const {
        return *this + (another * -1);
    }

    LetterMath operator*(const LetterMath &another) const {
        return {this->entry.empty() ? "" : "(" + this->entry + ")*" + another.entry};
    }

    LetterMath operator/(const LetterMath &another) const {
        return {this->entry.empty() ? "" : "(" + this->entry + ")/" + another.entry};
    }

    LetterMath operator*(const int &another) const {
        return {this->entry.empty() ? "" : "(" + this->entry + ")*" + std::to_string(another)};
    }

    bool operator==(const LetterMath &another) const {
        return this->entry == another.entry;
    }

    bool operator!=(const LetterMath &another) const {
        return !(*this == another);
    }

    friend std::ostream &operator<<(std::ostream &ostream, const LetterMath &math) {
        ostream << math.entry;
        return ostream;
    }
};

#endif /* LETTER_MATH */
