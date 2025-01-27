#ifndef FIMS_INTERFACE_RCPP_RCPP_OBJECTS_SHARED_PRIMITIVE_HPP
#define FIMS_INTERFACE_RCPP_RCPP_OBJECTS_SHARED_PRIMITIVE_HPP

#include <memory>

class SharedInt {
private:
    std::shared_ptr<int> value;

public:
    // Constructor

    SharedInt() : value(std::make_shared<int>(0)) {
    }

    SharedInt(int val) : value(std::make_shared<int>(val)) {
    }

    // Copy Constructor

    SharedInt(const SharedInt& other) : value(other.value) {
    }

    // Assignment Operator

    SharedInt& operator=(const SharedInt& other) {
        if (this != &other) {
            value = other.value;
        }
        return *this;
    }

    SharedInt& operator=(const int& other) {

        *value = other;

        return *this;
    }

    // Move Constructor

    SharedInt(SharedInt&& other) noexcept : value(std::move(other.value)) {
    }

    // Move Assignment Operator

    SharedInt& operator=(SharedInt&& other) noexcept {
        if (this != &other) {
            value = std::move(other.value);
        }
        return *this;
    }

    // Access the value

    int get() const {
        return *value;
    }

    void set(int val) {
        *value = val;
    }

    operator int() {
        return this->get();
    }

    // Overloaded operators for SharedPrimitive

    int operator*() const {
        return *value;
    } // Dereference

    int* operator->() {
        return value.get();
    } // Arrow operator

    const int* operator->() const {
        return value.get();
    } // Const arrow operator

    // Unary operators

    SharedInt& operator++() {
        ++(*value);
        return *this;
    } // Prefix increment

    SharedInt operator++(int) {
        SharedInt temp(*this);
        ++(*value);
        return temp;
    } // Postfix increment

    SharedInt& operator--() {
        --(*value);
        return *this;
    } // Prefix decrement

    SharedInt operator--(int) {
        SharedInt temp(*this);
        --(*value);
        return temp;
    } // Postfix decrement

    // Arithmetic operators with SharedPrimitive

    SharedInt operator+(const SharedInt& other) const {
        return SharedInt(*value + *other.value);
    }

    SharedInt operator-(const SharedInt& other) const {
        return SharedInt(*value - *other.value);
    }

    SharedInt operator*(const SharedInt& other) const {
        return SharedInt(*value * *other.value);
    }

    SharedInt operator/(const SharedInt& other) const {
        return SharedInt(*value / *other.value);
    }

    // Arithmetic operators with primitives

    SharedInt operator+(const int& other) const {
        return SharedInt(*value + other);
    }

    SharedInt operator-(const int& other) const {
        return SharedInt(*value - other);
    }

    SharedInt operator*(const int& other) const {
        return SharedInt(*value * other);
    }

    SharedInt operator/(const int& other) const {
        return SharedInt(*value / other);
    }

    // Compound assignment operators with SharedPrimitive

    SharedInt& operator+=(const SharedInt& other) {
        *value += *other.value;
        return *this;
    }

    SharedInt& operator-=(const SharedInt& other) {
        *value -= *other.value;
        return *this;
    }

    SharedInt& operator*=(const SharedInt& other) {
        *value *= *other.value;
        return *this;
    }

    SharedInt& operator/=(const SharedInt& other) {
        *value /= *other.value;
        return *this;
    }

    // Compound assignment operators with primitives

    SharedInt& operator+=(const int& other) {
        *value += other;
        return *this;
    }

    SharedInt& operator-=(const int& other) {
        *value -= other;
        return *this;
    }

    SharedInt& operator*=(const int& other) {
        *value *= other;
        return *this;
    }

    SharedInt& operator/=(const int& other) {
        *value /= other;
        return *this;
    }

    // Relational operators

    bool operator==(const SharedInt& other) const {
        return *value == *other.value;
    }

    bool operator!=(const SharedInt& other) const {
        return *value != *other.value;
    }

    bool operator<(const SharedInt& other) const {
        return *value < *other.value;
    }

    bool operator<=(const SharedInt& other) const {
        return *value <= *other.value;
    }

    bool operator>(const SharedInt& other) const {
        return *value > *other.value;
    }

    bool operator>=(const SharedInt& other) const {
        return *value >= *other.value;
    }

    // Relational operators with primitives

    bool operator==(const int& other) const {
        return *value == other;
    }

    bool operator!=(const int& other) const {
        return *value != other;
    }

    bool operator<(const int& other) const {
        return *value < other;
    }

    bool operator<=(const int& other) const {
        return *value <= other;
    }

    bool operator>(const int& other) const {
        return *value > other;
    }

    bool operator>=(const int& other) const {
        return *value >= other;
    }

    friend std::ostream& operator<<(std::ostream& os, const SharedInt& sp) {
        os << *sp.value;
        return os;
    }
};


// Non-member operators for primitive + SharedPrimitive

SharedInt operator+(const int& lhs, const SharedInt& rhs) {
    return SharedInt(lhs + rhs.get());
}

SharedInt operator-(const int& lhs, const SharedInt& rhs) {
    return SharedInt(lhs - rhs.get());
}

SharedInt operator*(const int& lhs, const SharedInt& rhs) {
    return SharedInt(lhs * rhs.get());
}

SharedInt operator/(const int& lhs, const SharedInt& rhs) {
    return SharedInt(lhs / rhs.get());
}

bool operator<(const int& lhs, const SharedInt& rhs) {
    return (lhs < rhs.get());
}

bool operator<=(const int& lhs, const SharedInt& rhs) {
    return (lhs <= rhs.get());
}

bool operator>(const int& lhs, const SharedInt& rhs) {
    return (lhs > rhs.get());
}

bool operator>=(const int& lhs, const SharedInt& rhs) {
    return (lhs >= rhs.get());
}

class SharedReal {
private:
    std::shared_ptr<int> value;

public:
    // Constructor

    SharedReal() : value(std::make_shared<int>(0)) {
    }

    SharedReal(int val) : value(std::make_shared<int>(val)) {
    }

    // Copy Constructor

    SharedReal(const SharedReal& other) : value(other.value) {
    }

    // Assignment Operator

    SharedReal& operator=(const SharedReal& other) {
        if (this != &other) {
            value = other.value;
        }
        return *this;
    }

    SharedReal& operator=(const int& other) {

        *value = other;

        return *this;
    }

    // Move Constructor

    SharedReal(SharedReal&& other) noexcept : value(std::move(other.value)) {
    }

    // Move Assignment Operator

    SharedReal& operator=(SharedReal&& other) noexcept {
        if (this != &other) {
            value = std::move(other.value);
        }
        return *this;
    }

    // Access the value

    int get() const {
        return *value;
    }

    void set(int val) {
        *value = val;
    }

    operator int() {
        return this->get();
    }

    // Overloaded operators for SharedPrimitive

    int operator*() const {
        return *value;
    } // Dereference

    int* operator->() {
        return value.get();
    } // Arrow operator

    const int* operator->() const {
        return value.get();
    } // Const arrow operator

    // Unary operators

    SharedReal& operator++() {
        ++(*value);
        return *this;
    } // Prefix increment

    SharedReal operator++(int) {
        SharedReal temp(*this);
        ++(*value);
        return temp;
    } // Postfix increment

    SharedReal& operator--() {
        --(*value);
        return *this;
    } // Prefix decrement

    SharedReal operator--(int) {
        SharedReal temp(*this);
        --(*value);
        return temp;
    } // Postfix decrement

    // Arithmetic operators with SharedPrimitive

    SharedReal operator+(const SharedReal& other) const {
        return SharedReal(*value + *other.value);
    }

    SharedReal operator-(const SharedReal& other) const {
        return SharedReal(*value - *other.value);
    }

    SharedReal operator*(const SharedReal& other) const {
        return SharedReal(*value * *other.value);
    }

    SharedReal operator/(const SharedReal& other) const {
        return SharedReal(*value / *other.value);
    }

    // Arithmetic operators with primitives

    SharedReal operator+(const int& other) const {
        return SharedReal(*value + other);
    }

    SharedReal operator-(const int& other) const {
        return SharedReal(*value - other);
    }

    SharedReal operator*(const int& other) const {
        return SharedReal(*value * other);
    }

    SharedReal operator/(const int& other) const {
        return SharedReal(*value / other);
    }

    // Compound assignment operators with SharedPrimitive

    SharedReal& operator+=(const SharedReal& other) {
        *value += *other.value;
        return *this;
    }

    SharedReal& operator-=(const SharedReal& other) {
        *value -= *other.value;
        return *this;
    }

    SharedReal& operator*=(const SharedReal& other) {
        *value *= *other.value;
        return *this;
    }

    SharedReal& operator/=(const SharedReal& other) {
        *value /= *other.value;
        return *this;
    }

    // Compound assignment operators with primitives

    SharedReal& operator+=(const int& other) {
        *value += other;
        return *this;
    }

    SharedReal& operator-=(const int& other) {
        *value -= other;
        return *this;
    }

    SharedReal& operator*=(const int& other) {
        *value *= other;
        return *this;
    }

    SharedReal& operator/=(const int& other) {
        *value /= other;
        return *this;
    }

    // Relational operators

    bool operator==(const SharedReal& other) const {
        return *value == *other.value;
    }

    bool operator!=(const SharedReal& other) const {
        return *value != *other.value;
    }

    bool operator<(const SharedReal& other) const {
        return *value < *other.value;
    }

    bool operator<=(const SharedReal& other) const {
        return *value <= *other.value;
    }

    bool operator>(const SharedReal& other) const {
        return *value > *other.value;
    }

    bool operator>=(const SharedReal& other) const {
        return *value >= *other.value;
    }

    // Relational operators with primitives

    bool operator==(const int& other) const {
        return *value == other;
    }

    bool operator!=(const int& other) const {
        return *value != other;
    }

    bool operator<(const int& other) const {
        return *value < other;
    }

    bool operator<=(const int& other) const {
        return *value <= other;
    }

    bool operator>(const int& other) const {
        return *value > other;
    }

    bool operator>=(const int& other) const {
        return *value >= other;
    }

    friend std::ostream& operator<<(std::ostream& os, const SharedReal& sp) {
        os << *sp.value;
        return os;
    }
};

SharedReal operator+(const double& lhs, const SharedReal& rhs) {
    return SharedReal(lhs + rhs.get());
}

SharedReal operator-(const double& lhs, const SharedReal& rhs) {
    return SharedReal(lhs - rhs.get());
}

SharedReal operator*(const double& lhs, const SharedReal& rhs) {
    return SharedReal(lhs * rhs.get());
}

SharedReal operator/(const double& lhs, const SharedReal& rhs) {
    return SharedReal(lhs / rhs.get());
}


class SharedString {
private:
    std::shared_ptr<std::string> value;

public:
    // Constructor

    SharedString() : value(std::make_shared<std::string>()) {
    }

    SharedString(std::string val) : value(std::make_shared<std::string>(val)) {
    }

    // Copy Constructor

    SharedString(const SharedString& other) : value(other.value) {
    }

    // Assignment Operator

    SharedString& operator=(const SharedString& other) {
        if (this != &other) {
            value = other.value;
        }
        return *this;
    }

    SharedString& operator=(const std::string& other) {

        *value = other;

        return *this;
    }

    // Move Constructor

    SharedString(SharedString&& other) noexcept : value(std::move(other.value)) {
    }

    // Move Assignment Operator

    SharedString& operator=(SharedString&& other) noexcept {
        if (this != &other) {
            value = std::move(other.value);
        }
        return *this;
    }

    // Access the value

    std::string get() const {
        return *value;
    }

    void set(std::string val) {
        *value = val;
    }

    operator std::string() {
        return this->get();
    }

    // Overloaded operators for SharedPrimitive

    std::string operator*() const {
        return *value;
    } // Dereference

    std::string* operator->() {
        return value.get();
    } // Arrow operator

    const std::string* operator->() const {
        return value.get();
    } // Const arrow operator

   

    friend std::ostream& operator<<(std::ostream& os, const SharedString& sp) {
        os << *sp.value;
        return os;
    }
};

typedef SharedInt fims_int;
typedef SharedReal fims_double;
typedef SharedString fims_string;


#endif
