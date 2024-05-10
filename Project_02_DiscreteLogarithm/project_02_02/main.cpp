#include <iostream>
#include <fstream>
#include <cmath>
#include <bitset>
#include <string>
#include <algorithm>

using namespace std;

#define BITSET_SIZE 512

class bignum {
private:
    bitset<BITSET_SIZE> bits;
    bool check;
public:
    bignum() {
        check = false;
    }

    bignum(int32_t value){
        if (value < 0) {
            check = true;
            value = -value;
        }
        else
            check = false;
        bits = bitset<BITSET_SIZE>(value);
    }

    bignum(const string& value){
         if (value[0] == '-'){
            check = true;
            bits = bitset<BITSET_SIZE>(value.substr(1));
        }
        else{
            check = false;
            bits = bitset<BITSET_SIZE>(value);
        }
    }

    int getLength() const{
        for (int32_t i = bits.size() - 1; i >= 0; i--)
        {
            if (bits[i])
            {
                return i + 1;
            }
        }
        return 0;
    }

    const bignum& operator+=(const bignum& big)
    {
        if (big.bits.none())
            return *this;

        if (check == big.check)
        {
            bignum temp = big;

            while (temp != 0)
            {
                bitset<BITSET_SIZE> carry = this->bits & temp.bits;
                this->bits ^= temp.bits;
                temp.bits = carry << 1;
            }
        }
        else
        {
            if (check)
            {
                check = false;
                if (*this < big)
                {
                    bignum temp = big;
                    temp -= *this;
                    *this = temp;
                }
                else
                {
                    *this -= big;
                    this->check = true;
                }
            }
            else
            {
                bignum temp = big;
                temp.check = false;

                if (*this < temp)
                {
                    temp -= *this;
                    *this = temp;
                    this->check = true;
                }
                else
                {
                    *this -= temp;
                }
            }
        }

        return *this;
    }

    bignum operator+(const bignum& other)
    {
        bignum result = *this;
        result += other;
        return result;
    }

    const bignum& operator-=(const bignum& big)
    {
        if (big.bits.none())
            return *this;

        if (check == big.check)
        {
            if (this == &big)
            {
                *this = 0;
                return *this;
            }
            bool flag = false;
            bignum temp1, temp2;

            if (*this < big)
            {
                flag = true;
                temp1 = big;
                temp2 = *this;
            }
            else
            {
                temp1 = *this;
                temp2 = big;
            }

            if (check)
            {
                swap(temp1, temp2);
            }

            while (temp2 != 0)
            {
                bitset<BITSET_SIZE> borrow = (~temp1.bits) & temp2.bits;
                temp1.bits ^= temp2.bits;
                temp2.bits = borrow << 1;
            }

            *this = temp1;
            check = flag;
        }
        else
        {
            if (check)
            {
                check = false;
                *this += big;
                check = true;
            }
            else
            {
                bignum temp = big;
                temp.check = false;
                *this += temp;
            }
        }

        return *this;
    }

    bignum operator-(const bignum& other)
    {
        bignum result = *this;
        result -= other;
        return result;
    }

    const bignum& operator*=(const bignum& big)
    {
        if (big.bits.none())
        {
            *this = 0;
            return *this;
        }

        bool tempCheck = (check != big.check);
        check = false;

        bignum temp1 = *this;
        bignum temp2 = big;
        temp1.check = tempCheck;
        temp2.check = tempCheck;

        bits.reset();

        while (temp2 != 0)
        {
            if (temp2.bits[0])
            {
                *this += temp1;
            }

            temp1 <<= 1;
            temp2 >>= 1;
        }

        return *this;
    }

    bignum operator*(const bignum& other)
    {
        bignum result = *this;
        result *= other;
        return result;
    }

    const bignum& operator/=(const bignum& big)
    {
        if (big.bits.none())
        {
            throw runtime_error("Divide by zero exception");
        }

        bool flag = (check != big.check);
        check = false;

        bignum temp1 = *this;
        bignum temp2 = big;
        temp1.check = false;
        temp2.check = false;

        if (temp1 < temp2)
        {
            *this = 0;
            return *this;
        }

        int32_t length = max(temp1.getLength(), temp2.getLength());

        bignum result = 0;
        for (int i = length - 1; i >= 0; i--)
        {
            bignum temp = temp2;
            temp <<= i;

            if (temp <= temp1)
            {
                temp1 -= temp;
                result.setBit(i, true);
            }
        }

        *this = result;
        check = flag;
        return *this;
    }

    bignum operator/(const bignum& other)
    {
        bignum result = *this;
        result /= other;
        return result;
    }

    const bignum& operator%=(const bignum& big)
    {
        if (big.bits.none())
        {
            throw runtime_error("Divide by zero exception");
        }

        bignum temp1 = *this;
        bignum temp2 = temp1 / big;

        *this -= temp2 * big;

        return *this;
    }

    bignum operator%(const bignum& other)
    {
        bignum result = *this;
        result %= other;
        return result;
    }

    const bignum& operator^=(const bignum& big)
    {
        if (big.bits.none() && bits.none())
            throw runtime_error("Exponent zero by zero exception");

        if (big.bits.none()){
            *this = 1;
            return *this;
        }

        if (big.check)
            runtime_error("Exponent negative exception");

        bool flag = false;

        if (check && big.bits[0])
            flag = true;

        bignum result = 1;
        bignum temp1 = *this;
        bignum temp2 = big;

        while (!temp2.bits.none())
        {
            if (temp2.bits[0])
                result *= temp1;

            temp1 *= temp1;
            temp2 >>= 1;
        }

        *this = result;
        this->check = flag;
        return *this;
    }

    bignum operator^(const bignum& other)
    {
        bignum result = *this;
        result ^= other;
        return result;
    }

    bignum& operator>>=(int32_t shift) {
        bits >>= shift;
        return *this;
    }

    bignum& operator<<=(int32_t shift)
    {
        bits <<= shift;
        return *this;
    }

    bool operator<(const bignum& other){
        if (check != other.check)
            return check;

        int length = max(getLength(), other.getLength());

        for (int32_t i = length - 1; i >= 0; i--)
        {
            if (bits[i] != other.bits[i])
            {
                return bits[i] < other.bits[i];
            }
        }
        return false;
    }

    bool operator<=(const bignum& other)
    {
        return *this < other || *this == other;
    }

    bool operator>(const bignum& other)
    {
        return !(*this <= other);
    }

    bool operator>=(const bignum& other)
    {
        return !(*this < other);
    }

    bool operator==(const bignum& other){
        if (other.bits.none())
            return bits == other.bits;
        return bits == other.bits && check == other.check;
    }

    bool operator!=(const bignum& other){
        if (other.bits.none())
            return bits != other.bits;
        return bits != other.bits || check != other.check;
    }

    bool getBit(int32_t index){
        return bits.test(index);
    }

    void setBit(int32_t index, bool value){
        bits.set(index, value);
    }

    string toHexLittleEndian(){
        string hexString;
        int length = getLength();

        for (int i = 0; i < length; i += 4) {
            int temp = 0;
            for (int j = 0; j < 4; j++) {
                if (getBit(i + j)) {
                    temp |= (1 << j);
                }
            }
            hexString += ToHex(temp);
        }

        return hexString;
    }

    string ToHex(int temp){
        if (temp < 10) {
            return to_string(temp);
        } else {
            return string(1, 'A' + (temp - 10));
        }
    }
};

bignum Power_mod(bignum x, bignum y, bignum p)
{
    bignum result(1); 
    x %= p;           
    while (y > 0)
    {
        if (y.getBit(0))
        {
            result *= x;
            result %= p;
        }
        y >>= 1;
        x *= x;
        x %= p;

    }

    return result;
}

string hexToBin(const string& hex) {
    string bin = "";
    for (char c : hex) {
        c = toupper(c); 
        int value;
        if (c >= '0' && c <= '9') {
            value = c - '0';
        }
        else {
            value = 10 + c - 'A';
        }
        string temp = "";
        for (int i = 3; i >= 0; --i) {
            temp += (value >> i) & 1 ? '1' : '0';
        }
        bin += temp;
    }

    size_t firstNonZero = bin.find_first_not_of('0');
    if (firstNonZero != string::npos) {
        bin = bin.substr(firstNonZero);
    }
    else {
        bin = "0";
    }

    return bin;
}

string reverseHex(const string& hex) {
    string reversedHex = hex;
    reverse(reversedHex.begin(), reversedHex.end());
    return reversedHex;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        return 1;
    }

    ifstream inputFile(argv[1]);
    if (!inputFile) {
        cout << "Error opening input file." << endl;
        return 1;
    }

    ofstream outputFile(argv[2]);
    if (!outputFile) {
        cout << "Error opening output file." << endl;
        return 1;
    }
    
    string hex;
    getline(inputFile,hex);
    string reversedHex1 = reverseHex(hex);
    string bin1 = hexToBin(reversedHex1);
    bignum p(bin1);

    getline(inputFile,hex);
    string reversedHex2 = reverseHex(hex);
    string bin2 = hexToBin(reversedHex2);
    bignum g(bin2);

    getline(inputFile,hex);
    string reversedHex3 = reverseHex(hex);
    string bin3 = hexToBin(reversedHex3);
    bignum a(bin3);

    getline(inputFile,hex);
    string reversedHex4 = reverseHex(hex);
    string bin4 = hexToBin(reversedHex4);
    bignum b(bin4);

    bignum A = Power_mod(g, a, p);
    bignum B = Power_mod(g, b, p);
    bignum K = Power_mod(B, a, p);

    outputFile << A.toHexLittleEndian() << endl;
    outputFile << B.toHexLittleEndian() << endl;
    outputFile << K.toHexLittleEndian() << endl;

    inputFile.close();
    outputFile.close();

    return 0;
}