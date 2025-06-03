#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <iterator>
#include <cstdint>

using namespace std;

const string MASTER_PASSWORD = "admin123";
const string VAULT_FILE = "vault.txt";
const char XOR_KEY = 'K';

// XOR encryption/decryption
string encryptDecrypt(const string& input) {
    string output = input;
    for (char& c : output) {
        c ^= XOR_KEY;
    }
    return output;
}

// Base64 encode
string toBase64(const string& input) {
    static const string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    string result;
    int val = 0, valb = -6;
    for (uint8_t c : input) {
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            result.push_back(base64_chars[(val >> valb) & 0x3F]);
            valb -= 6;
        }
    }
    if (valb > -6) result.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
    while (result.size() % 4) result.push_back('=');
    return result;
}

// Base64 decode
string fromBase64(const string& input) {
    static const string base64_chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    vector<int> T(256, -1);
    for (int i = 0; i < 64; i++) T[base64_chars[i]] = i;

    string result;
    int val = 0, valb = -8;
    for (uint8_t c : input) {
        if (T[c] == -1) break;
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            result.push_back(char((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return result;
}

string encryptAndEncode(const string& msg) {
    return toBase64(encryptDecrypt(msg));
}

string decodeAndDecrypt(const string& encoded) {
    return encryptDecrypt(fromBase64(encoded));
}

void addPassword() {
    string site, username, password;
    cin.ignore();

    cout << "Enter site: ";
    getline(cin, site);
    cout << "Enter username: ";
    getline(cin, username);
    cout << "Enter password: ";
    getline(cin, password);

    ofstream file(VAULT_FILE, ios::app);
    file << encryptAndEncode(site) << "|" << encryptAndEncode(username) << "|" << encryptAndEncode(password) << endl;
    file.close();

    cout << "\u2705 Password saved!\n";
}

void viewPasswords() {
    ifstream file(VAULT_FILE);
    string line;

    cout << "\n--- STORED PASSWORDS ---\n";

    while (getline(file, line)) {
        size_t pos1 = line.find('|');
        size_t pos2 = line.find('|', pos1 + 1);
        if (pos1 == string::npos || pos2 == string::npos) continue;

        string siteEnc = line.substr(0, pos1);
        string userEnc = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string passEnc = line.substr(pos2 + 1);

        cout << "Site: " << decodeAndDecrypt(siteEnc)
             << ", Username: " << decodeAndDecrypt(userEnc)
             << ", Password: " << decodeAndDecrypt(passEnc) << endl;
    }

    file.close();
}

int main() {
    string enteredPassword;
    cout << "\U0001F510 Enter master password: ";
    cin >> enteredPassword;

    if (enteredPassword != MASTER_PASSWORD) {
        cout << "\u274C Access denied.\n";
        return 1;
    }

    int choice;
    do {
        cout << "\nPassword Manager\n";
        cout << "1. Add password\n";
        cout << "2. View passwords\n";
        cout << "3. Exit\n";
        cout << "Choice: ";
        cin >> choice;

        switch (choice) {
            case 1: addPassword(); break;
            case 2: viewPasswords(); break;
            case 3: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice.\n";
        }
    } while (choice != 3);

    return 0;
}
// TIP See CLion help at <a
// href="https://www.jetbrains.com/help/clion/">jetbrains.com/help/clion/</a>.
//  Also, you can try interactive lessons for CLion by selecting
//  'Help | Learn IDE Features' from the main menu.