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

// Hàm tính nghịch đảo mô-đun
long long modInverse(long long a, long long m) {
    a %= m;
    for (long long x = 1; x < m; x++) {
        if ((a * x) % m == 1) {
            return x;
        }
    }
    return 1;
}

long long randomInRange(long long min, long long max) {
    return min + rand() % (max - min + 1);
}

// Hàm sinh khóa ElGamal
void generateElGamalKeys(long long p, long long g, long long& h, long long& x) {
    srand(time(0));
    x = randomInRange(1, p-2); // tạo khóa riêng ngẫu nhiên từ 1 đến p-2

    // Khóa công khai
    h = modPow(g, x, p);
}

// Hàm tạo chữ ký
void signMessage(long long message, long long p, long long g, long long x, long long& s1, long long& s2) {
    long long k = 7; 

    s1 = modPow(g, k, p);
    s2 = modInverse(k, p-1) * (message - x*s1) % (p-1);
    s2 = (s2 + (p-1)) % (p-1); // Đảm bảo s dương
}

// Hàm xác minh chữ ký
bool verifySignature(long long message, long long s1, long long s2, long long p, long long g, long long h) {
    if (s1 <= 0 || s1 >= p || s2 <= 0 || s2 >= p-1) {
        return false;
    }

    long long v1 = modPow(g, message, p);
    long long v2 = modPow(h, s1, p) * modPow(s1, s2, p) % p;

    if (v1 == v2){
        return true;
    }
    else {
        return false;
    }
}

int main() {
    long long p = 23;
    long long g = 5;
    long long h, x, s1, s2;

    generateElGamalKeys(p, g, h, x);

    cout << "Public Key: (" << p << ", " << g << ", " << h << ")\n";
    cout << "Private Key: (" << x << ")\n";

    // Nhập tên file và ký nội dung
    string filename;
    cout << "Enter filename to sign: ";
    cin >> filename;
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    string line;
    stringstream signedContent;
    ofstream outFile("signed_" + filename);
    cout << "Signed content: " << endl;
    while (getline(inFile, line)) {
        stringstream ss(line);
        long long message;

        while (ss >> message) {
            signMessage(message, p, g, x, s1, s2);
            cout << message << " " << s1 << " " << s2 << endl;
            outFile << message << " " << s1 << " " << s2 << endl;
        }
    }

    inFile.close();
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
        bool isValid = true;
        while (getline(signedFile, line)) {
            stringstream ss(line);
            long long message, s1, s2;

            ss >> message >> s1 >> s2;

            isValid = verifySignature(message, s1, s2, p, g, h);
            cout << "Message: " << message << " - Signature: (" << s1 << ", " << s2 << ") - Valid: " << (isValid ? "Yes" : "No") << endl;
        }

        signedFile.close();

        if (isValid) {
            cout << "Signature is valid." << endl;
        } else {
            cout << "Signature is invalid." << endl;
        }
        cout <<"Enter word check again to verify again: ";
        cin >> choice;
    }
    cout << "Done!";
    return 0;
}
