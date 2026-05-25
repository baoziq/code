#include <iostream>
#include <string>

using namespace std;

int strStr(string s1, string s2) {
	int len1 = s1.size();
	int len2 = s2.size();
	int k = 0;
	for (int i = 0; i < len1; i++) {
		int j = i;
		while (k < len2 && s1[j] == s2[k]) {
			cout << "s1: " << s1[j] << endl;
			cout << "s2: " << s2[k] << endl;
			j++;
			k++;
		}
		cout << "k: " << k << endl;
		if (k == len2) {
			return i;
		}
		k = 0;
	}
	return -1;
}

int main() {
	string s1 = "0abcde";
	string s2 = "aabc";
	cout << strStr(s1, s2) << endl;
}
