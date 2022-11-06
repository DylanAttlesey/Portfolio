/* Conlang Word Generator 1.cpp : This file contains the 'main' function. Program execution begins and ends there.
To check:
1. Input files
2. Likelihood of word having successive syllables.
3. Whether to delete letter or redo word if a disallowed sequence is found

To implement:
1. Old dictionary
2. Function for max syllable count
3. Function for non-geometric syllable # liklihoods.
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()
using namespace std;

string genSyll(ifstream& onsetF, int onsetSum, ifstream& nucleusF, int nucleusSum, ifstream& codaF, int codaSum);
string chooseSegment(ifstream& srcF, int onsetSum);
bool isProblematic(vector<string>& newWords, vector<string>& disVector, string& syll);
int getProbabilitySum(ifstream& file);
void buildDisallowVector(vector<string>& disVector, ifstream& disallowedF);

int main()
{
	ifstream engF, onsetF, nucleusF, codaF, disallowedF, oldDictF;
	ofstream dictF;
	string line, syll;
	int onsetSum, nucleusSum, codaSum;
	vector<string> prevWords, disVector;
	bool problematic = false;
	
	srand(time(NULL));

	dictF.open("OutputDict.txt");
	engF.open("English Basic Vocab List.txt");
	onsetF.open("Onsets.txt");
	nucleusF.open("Nuclei.txt");
	codaF.open("Codas.txt");
	disallowedF.open("DisallowedSegments.txt");

	onsetSum = getProbabilitySum(onsetF);
	nucleusSum = getProbabilitySum(nucleusF);
	codaSum = getProbabilitySum(codaF);
	buildDisallowVector(disVector, disallowedF);

	onsetF.close();
	nucleusF.close();
	codaF.close();

	onsetF.open("Onsets.txt");
	nucleusF.open("Nuclei.txt");
	codaF.open("Codas.txt");

	while (getline(engF, line)) {
		dictF << line;
		dictF << '\t' << '\t' << '\t';
		do {
			syll = genSyll(onsetF, onsetSum, nucleusF, nucleusSum, codaF, codaSum);
			problematic = false;
			problematic = isProblematic(prevWords, disVector ,syll); /////////////////////////////////////////

		} while (problematic == true);
		problematic = false;
		prevWords.push_back(syll);
		dictF << syll << endl;
	}
}

void buildDisallowVector(vector<string>& disVector, ifstream& disallowedF) {
	string line;
	while (getline(disallowedF, line)) {
		disVector.push_back(line);
		line.clear();
	}
}

bool isProblematic(vector<string>& newWords, vector<string>& disVector, string& syll) {
	
	for (auto it = syll.begin(); it != syll.end() - 1; it++) {
		if (*it == *(it + 1)) { //if a letter exists twice in a row. comment out if not a problem.
			//syll.erase(it); 
		}
	}
	
	for (auto it = disVector.begin(); it != disVector.end() - 1; it++) {
		if (syll.find(*it)!= string::npos) { //if disallowed segment exists in the word, delete the first 
			//letter and see if it works. Check with each new lang if it is appropriate. 
			syll.erase(syll.find(*it), 1);
			//return true;
		}
	}
	
	for (auto it = newWords.begin(); it != newWords.end(); it++) {
		if (*it == syll) {
			return true;
		}
	}
	return false;
}

string genSyll(ifstream& onsetF, int onsetSum, ifstream& nucleusF, int nucleusSum, ifstream& codaF, int codaSum) {
	string syll;
	int r; 
	do {
		syll += chooseSegment(onsetF, onsetSum);
		syll += chooseSegment(nucleusF, nucleusSum);
		syll += chooseSegment(codaF, codaSum);
		r = (rand() % 3);// Change this number to change likelihood of word having more syllables;
		//the smaller the number, the higher the likelihood.
	} while (r == 0);
	return syll;
}

string chooseSegment(ifstream& file, int fileSum) {
	string seg, line;
	int r = (rand() % fileSum);
	int currentProbSum = 0;
	int currentProb = 0;
	while (getline(file, line)) {
		istringstream lineS(line);
		lineS >> seg;
		lineS >> currentProb;
		currentProbSum += currentProb;
		if (currentProbSum >= r) {
			break;
		}
		currentProb = 0;
		seg.clear();
	}
	file.seekg(0);
	if (seg == "0")
		return "";
	return seg;
}


int getProbabilitySum(ifstream& file) { 
	//calculates the sum of probability values, essentially the denominator
	//of the likelihood fraction of each segment
	string line, temp;
	int currentProb = 0; 
	int sum = 0;

	while (getline(file, line)) {
		istringstream lineS(line);
		lineS >> temp;
		lineS >> currentProb;
		sum += currentProb;
		currentProb = 0;
	}
	file.seekg(0);
	return sum;
}

