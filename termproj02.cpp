#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <set>
#include <limits> //Correction
using namespace std;


// Function to check if a string is a number
bool isNumber(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

// Function to calculate the value of a word based on the provided letter values
int calculateWordValue(std::string word, std::map<char, int> letterValues) {
    int value = 0;
    for (char c : word) {
        c = std::tolower(c); // Convert to lowercase
        if (letterValues.find(c) != letterValues.end()) {
            value += letterValues[c];
        }
    }
    return value;
}

// Function to calculate Levenshtein distance between two words
int levenshteinDistance(const std::string& s1, const std::string& s2) {
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<int> > dp(len1 + 1, std::vector<int>(len2 + 1, 0));   
    // ---------> We have to edit this to make it O(n) or smaller, currently O(NM)
    for (size_t i = 0; i <= len1; i++) {
        for (size_t j = 0; j <= len2; j++) {
            if (i == 0)
                dp[i][j] = j;
            else if (j == 0)
                dp[i][j] = i;
            else
                dp[i][j] = std::min({ dp[i - 1][j - 1] + (s1[i - 1] == s2[j - 1] ? 0 : 1), dp[i - 1][j] + 1, dp[i][j - 1] + 1 });
        }
    }
    return dp[len1][len2];
}

// Function to find the most similar word from a list
std::string findMostSimilarWord(const std::string& target, const std::vector<std::string>& wordList) {
    std::string mostSimilarWord;
    int minDistance = std::numeric_limits<int>::max();
    for (const auto& word : wordList) {
        int distance = levenshteinDistance(target, word);
        if (distance < minDistance) {
            minDistance = distance;
            mostSimilarWord = word;
        }
    }
    return mostSimilarWord;
}

int main() {
    // Read words from the input file
    std::ifstream inputFile("input_words.txt");
    std::vector<std::string> words;
    std::vector<std::string> numbers;
    std::vector<std::string> mixedWords;
    std::string token;
    while (inputFile >> token) {
        if (isNumber(token)) {
            numbers.push_back(token); // Add to numbers list if it's a number
        }
        else if (std::any_of(token.begin(), token.end(), ::isdigit)) {
            mixedWords.push_back(token); // Add to mixed words list if it contains a digit
        }
        else {
            words.push_back(token); // Add to words list if it's a regular word
        }
    }
    inputFile.close();

    // Read letter values from the values file
    std::ifstream valuesFile("letter_values.txt");
    std::map<char, int> letterValues;
    std::string line;
    while (std::getline(valuesFile, line)) {
        std::istringstream iss(line);
        char letter;
        char equalSign;
        int value;
        if (iss >> letter >> equalSign >> value) {
            if (equalSign == '=') {
                letterValues[std::tolower(letter)] = value; // Convert letter to lowercase
                letterValues[std::toupper(letter)] = value; // Add the uppercase equivalent
            }
        }
    }
    valuesFile.close();

    // Calculate the values of the words and store unique values
    std::map<int, std::vector<std::string> > wordValueMap;
    std::map<std::string, int> wordOccurrenceMap;
    for (const auto& w : words) {
        int value = calculateWordValue(w, letterValues);
        wordValueMap[value].push_back(w);
        wordOccurrenceMap[w]++;
    }


    // CHANGED TO OPTAMIZE //
    // Write the numbers and corresponding words to the output file    
    //---------> We have to edit this to make it O(n) or smaller
    std::ofstream uniqueSortedOutputFile("output_sorted_words_with_values.txt");
    std::stringstream buffer; // USING A BUFFER TO WRITE MULTIPLE LINES
    for (const auto& entry : wordValueMap) {
        buffer << entry.first << " - ";
        // copy elements from the entries start to the entry end into the buffer ostream
        std::copy(entry.second.begin(), entry.second.end(), std::ostream_iterator<std::string>(buffer, " ")); 
        buffer << "\n"; // move to next line
    }
    uniqueSortedOutputFile << buffer.rdbuf(); // write the entire entry of the buffer into the output file
    uniqueSortedOutputFile.close();



    // Count occurrences of each number
    std::map<std::string, int> numberOccurrences;
    for (const auto& num : numbers) {
        if (numberOccurrences.find(num) != numberOccurrences.end()) {
            numberOccurrences[num]++;
        }
        else {
            numberOccurrences[num] = 1;
        }
    }

    // Sort numbers based on occurrences in descending order
    std::vector<std::pair<std::string, int> > sortedNumberOccurrences(numberOccurrences.begin(), numberOccurrences.end());
    std::sort(sortedNumberOccurrences.begin(), sortedNumberOccurrences.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
            return a.second > b.second;
        });

    // Write the sorted numbers with occurrences to the output file
    std::ofstream numbersFile("output_sorted_numbers.txt");
    for (const auto& entry : sortedNumberOccurrences) {
        numbersFile << entry.first << " - Occurrences: " << entry.second << "\n";
    }
    numbersFile.close();

    // Find and correct mixed words
    std::ofstream correctedMixedWordsFile("output_corrected_mixed_words.txt");
    for (const auto& mixedWord : mixedWords) {
        std::string correctedWord = findMostSimilarWord(mixedWord, words);
        correctedMixedWordsFile << mixedWord << " - " << correctedWord << "\n";
    }
    correctedMixedWordsFile.close();

    return 0;
}
