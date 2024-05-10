#include <iostream>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>

using namespace std;

// Hàm tính số mũ mô đun
long long modPow(long long base, long long exp, long long modulus) {
    base %= modulus;
    long long result = 1;
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % modulus;
        }
        base = (base * base) % modulus;
        exp /= 2;
    }
    return result;
}

// Hàm tính ước chung lớn nhất
long long gcd(long long a, long long b) {
    while (b != 0) {
        long long t = b;
        b = a % b;
        a = t;
    }
    return a;
}

// Hàm tính nghịch đảo mô-đun
long long modInverse(long long a, long long m) {
    a = a % m;
    for (long long x = 1; x < m; x++) {
        if ((a * x) % m == 1)
            return x;
    }
    return 1;
}

// Hàm sinh khóa RSA
void generateRSAKeys(long long p, long long q, long long& e, long long& d, long long& n) {
    n = p * q;
    long long phi = (p - 1) * (q - 1);

    // Chọn e
    e = 2;
    while (e < phi && gcd(e, phi) != 1) {
        e++;
    }

    d = modInverse(e,phi);
}

// Hàm tạo chữ ký
long long signMessage(long long message, long long d, long long n) {
    return modPow(message, d, n);
}

// Hàm xác minh chữ ký
bool verifySignature(long long message, long long signature, long long e, long long n) {
    return message == modPow(signature, e, n);
}

int main() {
    long long e, d, n, p, q;
    p = 19;
    q = 47;
    cout << "Generated primes: p = " << p << ", q = " << q << endl;

    generateRSAKeys(p, q, e, d, n);
    cout << "Suggested public exponent (e): " << e << endl;

    cout << "Public Key: (" << e << ", " << n << ")\n";
    cout << "Private Key: (" << d << ", " << n << ")\n";

   // Nhập tên file và ký nội dung
    string filename;
    cout << "Enter filename to sign: ";
    cin >> filename;
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    string number;
    stringstream signedContent;
    while (inFile >> number) {
        long long num = stoll(number);
        long long signature = signMessage(num, d, n);
        signedContent << signature << " ";
    }
    inFile.close();

    cout << "Signed content: " << signedContent.str() << endl;

    ofstream outFile("signed_" + filename);
    outFile << signedContent.str();
    outFile.close();


    cout <<"Enter word check to verify: ";
    string choice;
    cin >> choice;
    while (choice == "check"){
        // Nhập tên file chứa chữ ký để xác minh
        string signedFilename;
        cout << "Enter filename to verify: ";
        cin >> signedFilename;
        ifstream signedFile(signedFilename);
        if (!signedFile.is_open()) {
            cerr << "Error opening signed file!" << endl;
            return 1;
        }
    
        // Xác minh chữ ký
        inFile.open(filename);
        long long signature;
        bool isValid = true;
        while (inFile >> number && signedFile >> signature) {
            long long num = stoll(number);
            isValid = verifySignature(num, signature, e, n);
            cout << "Message: " << number << " - Signature: " << signature << " - Valid: " << (isValid ? "Yes" : "No") << endl;
        }

        inFile.close();
        signedFile.close();

        if (isValid) {
            cout << "Signature is valid." << endl;
        } else {
            cout << "Signature is invalid." << endl;
        }
        cout << "Enter word check again to verify again: ";
        cin >> choice;
    }

    cout<<"Done!";
    return 0;
}