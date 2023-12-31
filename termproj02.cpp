#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <set>
#include <thread>
#include <limits> //Correction
using namespace std;


// Function to check if a string is a number
bool isANumber(const std::string& s) {
    // CHECKS IF THE STRING IS NOT EMPTY OR IF IT HAS A NUMBER, RETURNS TRUE IS SO FALSE IF OTHERWISE
    if (s.empty()) {
        return false;
    }
    return std::all_of(s.begin(), s.end(), ::isdigit);
}

// Function to calculate the value of a word based on the provided letter values
int calculateWordValue(std::string word, std::map<char, int> letterValuesFromLetterValues_txt) {
    int valueTotalCount = 0; // add up the total value 
    for (char c : word) { // for every character in the string
        c = std::tolower(c); // Convert to lowercase

        if (letterValuesFromLetterValues_txt.find(c) != letterValuesFromLetterValues_txt.end()) { //find what number the char corresponds to

            valueTotalCount += letterValuesFromLetterValues_txt[c]; // add that to total
        }
    }
    return valueTotalCount; //return total value of the word
}

// CHANGED TO OPTAMIZE ----- NOT USED//
// WE USE DP PROGRAMMING TO REDUCE THE TIME COMPLEXITY BY REDUCING REDUNDANT CALCULATIONS
// RECURSIZE therefore solving by base cases
//  O(m * n), where m and n are the lengths of the input strings.
int LevDist(const std::string& a, const std::string& b, std::vector<std::vector<int>>& memo) {
    // Base cases
    if (a.size() == 0)
        return b.size();
    if (b.size() == 0)
        return a.size();

    // Check if the result is already calculated
    if (memo[a.size()][b.size()] != -1)
        return memo[a.size()][b.size()];

    // Calculate the indicator
    int indicator = (a[0] != b[0]) ? 1 : 0;

    // Recursive calls with memoization
    int result = std::min({ LevDist(a.substr(1), b, memo) + 1,
                            LevDist(a, b.substr(1), memo) + 1,
                            LevDist(a.substr(1), b.substr(1), memo) + indicator });

    // Memoize the result
    memo[a.size()][b.size()] = result;

    return result;
}


// CHANGED TO OPTAMIZE using RECURSION AND DP ----> NOT USED//
// Function to calculate Levenshtein distance between two words
int levenshteinDistance(const std::string& s1, const std::string& s2) {
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<int>> memo(len1 + 1, std::vector<int>(len2 + 1, -1)); 
    return LevDist(s1, s2, memo);
}

// CHANGED TO OPTAMIZE USING THREADING --> FINAL OPTAMIZATION
// Function to calculate Levenshtein distance between two words
int levenshteinDistance2(const std::string& s1, const std::string& s2) {
    const size_t len1 = s1.size();
    const size_t len2 = s2.size();

    std::vector<std::vector<int>> dp(len1 + 1, std::vector<int>(len2 + 1, 0));
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

// CHANGED TO OPTAMIZE USING THREADING --> FINAL OPTAMIZATION
// Function to find the most similar word from a list
std::string findMostSimilarWord(const std::string& target, const std::vector<std::string>& wordList) {
    std::string mostSimilarWord;
    int minDistance = std::numeric_limits<int>::max();

    // calculate Levenshtein distance for a range of words
    auto calculateDistances = [&](size_t start, size_t end) {

        for (size_t i = start; i < end; ++i) {
            int distanceBetween = levenshteinDistance2(target, wordList[i]);  // finds the distance using levenshtien

            if (distanceBetween < minDistance) {

                minDistance = distanceBetween; // when the distance = minDistance 
                mostSimilarWord = wordList[i]; // the most similar word is the word in the list at that number
            }
        }
    };
    const size_t numberOfThreads = std::thread::hardware_concurrency();  //get number of threads

    const size_t wordsPerEachThread = wordList.size() / numberOfThreads;  //find the words per thread

    std::vector<std::thread> threads;

    for (size_t i = 0; i < numberOfThreads - 1; ++i) {
        threads.emplace_back(calculateDistances, i * wordsPerEachThread, (i + 1) * wordsPerEachThread); 
        // creates a new thread and adds it to a vector of threads named threads.

    }

    // Calculate distances for the remaining words in the main thread
    calculateDistances((numberOfThreads - 1) * wordsPerEachThread, wordList.size());

    // Wait for all threads to finish
    for (auto& SingleThread : threads) {
        SingleThread.join(); //checking

    }

    return mostSimilarWord;
}

int main() {   /// JUST ADDED TIME TESTS -- FUNCTION UNCHANGED.
    // TESTING START TIMER
    auto start = std::chrono::high_resolution_clock::now();

    // Read words from the input file

    std::ifstream inputFile("input_words - Main.txt");
    std::vector<std::string> words;
    std::vector<std::string> numbers;
    std::vector<std::string> mixedWords;
    std::string token;
    while (inputFile >> token) {
        if (isANumber(token)) {
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
    auto endOptimize = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedOptimize = endOptimize - start;
    std::cout << "Optimization Time: " << elapsedOptimize.count() << " seconds\n";




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

    auto endNumbers = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedNumbers = endNumbers - endOptimize;
    std::cout << "Numbers Time: " << elapsedNumbers.count() << " seconds\n";

    // Sort numbers based on occurrences in descending order
    std::vector<std::pair<std::string, int> > sortedNumberOccurrences(numberOccurrences.begin(), numberOccurrences.end());
    std::sort(sortedNumberOccurrences.begin(), sortedNumberOccurrences.end(), [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
            return a.second > b.second;
        });
    auto endSort = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedSort = endSort - endNumbers;
    std::cout << "Sort Time: " << elapsedSort.count() << " seconds\n";

    // Write the sorted numbers with occurrences to the output file
    std::ofstream numbersFile("output_sorted_numbers.txt");
    for (const auto& entry : sortedNumberOccurrences) {
        numbersFile << entry.first << " - Occurrences: " << entry.second << "\n";
    }
    numbersFile.close();

    // Find and correct mixed words
    int count = 1000;
    std::ofstream correctedMixedWordsFile("output_corrected_mixed_words.txt");
    for (const auto& mixedWord : mixedWords) {
        std::string correctedWord = findMostSimilarWord(mixedWord, words);
        correctedMixedWordsFile << mixedWord << " - " << correctedWord << "\n";
        count -= 1;
        if (count == 0) {
            break;
        }
    }
    correctedMixedWordsFile.close();
    auto endMixedWords = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsedMixedWords = endMixedWords - endSort;
    std::cout << "Levenstein Time: " << elapsedMixedWords.count() << " seconds\n";


    // TESTING PRINT TIMER
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Total Time: " << elapsed.count() << " seconds\n";

    return 0;
}
