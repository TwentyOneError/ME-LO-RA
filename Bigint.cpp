#include <Arduino.h>
#include <vector>
#include <string>

class BigInt {
private:
    std::vector<int> digits;
    static const int BASE = 1000000000; // 10^9

    void removeLeadingZeros() {
        while (digits.size() > 1 && digits.back() == 0) {
            digits.pop_back();
        }
    }

public:
    BigInt() : digits({0}) {}

    BigInt(unsigned long long n) {
        if (n == 0) {
            digits.push_back(0);
        } else {
            while (n > 0) {
                digits.push_back(n % BASE);
                n /= BASE;
            }
        }
    }

    BigInt(const String& s) {
        if (s.length() == 0) {
            digits.push_back(0);
            return;
        }

        int start = 0;
        while (start < s.length() && s[start] == '0') {
            start++;
        }

        if (start == s.length()) {
            digits.push_back(0);
            return;
        }

        // ������������ ��������� �� �����
        for (int i = s.length(); i > start; ) {
            int end = max(start, i - 9);
            String block = s.substring(end, i);
            i = end;

            unsigned long num = 0;
            for (int j = 0; j < block.length(); j++) {
                num = num * 10 + (block[j] - '0');
            }
            digits.push_back(num);
        }
        removeLeadingZeros();
    }

    BigInt(const std::vector<int>& d, bool removeZeros = true) : digits(d) {
        if (removeZeros) {
            removeLeadingZeros();
        }
    }

    String toString() const {
        if (digits.empty()) return "0";

        String result;
        char buffer[10];

        // ������ ���� ��� ������� �����
        snprintf(buffer, sizeof(buffer), "%d", digits.back());
        result += buffer;

        // ��������� ����� � ����������� ������
        for (int i = (int)digits.size() - 2; i >= 0; i--) {
            // ������������ ��������������
            snprintf(buffer, sizeof(buffer), "%09d", digits[i]);
            result += buffer;
        }
        return result;
    }

    bool operator<(const BigInt& rhs) const {
        if (digits.size() != rhs.digits.size()) {
            return digits.size() < rhs.digits.size();
        }
        for (int i = digits.size() - 1; i >= 0; i--) {
            if (digits[i] != rhs.digits[i]) {
                return digits[i] < rhs.digits[i];
            }
        }
        return false;
    }

    bool operator==(const BigInt& rhs) const {
        if (digits.size() != rhs.digits.size()) return false;
        for (size_t i = 0; i < digits.size(); i++) {
            if (digits[i] != rhs.digits[i]) return false;
        }
        return true;
    }

    bool operator<=(const BigInt& rhs) const {
        return *this < rhs || *this == rhs;
    }

    bool operator>(const BigInt& rhs) const {
        return !(*this <= rhs);
    }

    bool operator>=(const BigInt& rhs) const {
        return !(*this < rhs);
    }

    BigInt operator+(const BigInt& rhs) const {
        std::vector<int> res;
        int carry = 0;
        size_t maxSize = max(digits.size(), rhs.digits.size());

        for (size_t i = 0; i < maxSize || carry; ++i) {
            long sum = carry;  // ������������� long ��� �����
            if (i < digits.size()) sum += digits[i];
            if (i < rhs.digits.size()) sum += rhs.digits[i];

            carry = sum >= BASE;
            if (carry) sum -= BASE;
            res.push_back(sum);
        }
        return BigInt(res);
    }

    BigInt operator-(const BigInt& rhs) const {
        if (*this < rhs) {
            Serial.println("Error: Subtraction would result in negative");
            return BigInt(0);
        }

        std::vector<int> res;
        int borrow = 0;
        for (size_t i = 0; i < digits.size(); ++i) {
            long sub = borrow;  // ������������� long
            borrow = 0;

            if (i < rhs.digits.size())
                sub += rhs.digits[i];

            if (digits[i] < sub) {
                res.push_back(BASE + digits[i] - sub);
                borrow = 1;
            } else {
                res.push_back(digits[i] - sub);
            }
        }
        return BigInt(res);
    }

    BigInt multiplyByDigit(int digit) const {
        if (digit == 0) return BigInt(0);
        if (digit == 1) return *this;

        std::vector<int> res;
        long long carry = 0;  // ������������� long long
        for (size_t i = 0; i < digits.size() || carry; ++i) {
            long long product = carry;
            if (i < digits.size())
                product += static_cast<long long>(digits[i]) * digit;

            carry = product / BASE;
            res.push_back(static_cast<int>(product % BASE));
        }
        return BigInt(res);
    }

    BigInt operator*(const BigInt& rhs) const {
        BigInt result(0);
        for (size_t i = 0; i < digits.size(); ++i) {
            BigInt temp = rhs.multiplyByDigit(digits[i]);
            std::vector<int> shifted(i, 0);
            shifted.insert(shifted.end(), temp.digits.begin(), temp.digits.end());
            result = result + BigInt(shifted, false);
        }
        result.removeLeadingZeros();
        return result;
    }

    std::pair<BigInt, BigInt> divmod(const BigInt& rhs) const {
        if (rhs == BigInt(0)) {
            Serial.println("Error: Division by zero");
            return {BigInt(0), BigInt(0)};
        }
        if (*this < rhs) {
            return {BigInt(0), *this};
        }

        BigInt current;
        std::vector<int> quotientDigits;

        for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
            current.digits.insert(current.digits.begin(), digits[i]);
            current.removeLeadingZeros();

            int left = 0, right = BASE;
            while (left < right) {
                int mid = (left + right) / 2;
                BigInt product = rhs.multiplyByDigit(mid);

                if (product <= current) {
                    left = mid + 1;
                } else {
                    right = mid;
                }
            }
            int digit = left - 1;
            quotientDigits.push_back(digit);

            BigInt product = rhs.multiplyByDigit(digit);
            current = current - product;
        }

        std::reverse(quotientDigits.begin(), quotientDigits.end());
        BigInt quotient(quotientDigits);
        quotient.removeLeadingZeros();
        return {quotient, current};
    }

    BigInt operator/(const BigInt& rhs) const {
        return divmod(rhs).first;
    }

    BigInt operator%(const BigInt& rhs) const {
        return divmod(rhs).second;
    }
};
