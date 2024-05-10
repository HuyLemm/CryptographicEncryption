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
    a = a % m;
    for (long long x = 1; x < m; x++) {
        if ((a * x) % m == 1)
            return x;
    }
    return 1;
}

long long findGenerator(long long p, long long q) {
    long long h = 2;
    long long g = modPow(h, (p - 1) / q, p);

    // Kiểm tra xem g có phải là phần tử sinh không
    while (g == 1 || g == 0) {
        h++;
        g = modPow(h, (p - 1) / q, p);
    }

    return g;
}

// Hàm sinh khóa DSA
void generateDSAKeys(long long p, long long q, long long g, long long& y, long long& x) {
    srand(time(0));
    x = rand() % (q - 1) + 1; // x là khóa bí mật, chọn ngẫu nhiên
    y = modPow(g, x, p); // khóa công khai
}

// Hàm tạo chữ ký
void signMessageDSA(long long message, long long p, long long q, long long g, long long x, long long& r, long long& s) {
    long long k = rand() % (q - 1) + 1; // Chọn k ngẫu nhiên
    r = modPow(g, k, p) % q;
    s = (modInverse(k, q) * (message + x * r)) % q;
}

bool verifySignatureDSA(long long message, long long r, long long s, long long p, long long q, long long g, long long y) {
    long long w = modInverse(s, q);
    long long u1 = (message * w) % q;
    long long u2 = (r * w) % q;
    long long v = (modPow(g, u1, p) * modPow(y, u2, p)) % p % q;
    return v == r;
}

int main() {
    long long g, y, x, p, q, r, s;
    p = 23;
    q = 11;
    g = findGenerator(p,q);
    cout << "Generated primes: p = " << p << ", q = " << q << endl;
    cout << "Generator: " << g << endl;

    generateDSAKeys(p, q, g, y, x);

    cout << "Public Key: (p=" << p << ", g=" << g << ", y=" << y << ")\n";
    cout << "Private Key: x=" << x << "\n";

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
            signMessageDSA(message, p, q, g, x, r, s);
            cout << message << " " << r << " " << s << endl;
            outFile << message << " " << r << " " << s << endl;
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
            long long message, r, s;

            ss >> message >> r >> s;

            isValid = verifySignatureDSA(message, r, s, p, q, g, y);
            cout << "Message: " << message << " - Signature: (" << r << ", " << s << ") - Valid: " << (isValid ? "Yes" : "No") << endl;
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