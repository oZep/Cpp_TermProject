# Cpp_TermProject

**Optimization Strategy and Approach**

**Project Overview:**

The C++ file in question processes a large input file containing scrambled words with random characters. The goal is to decode the text by assigning values to each letter based on a predefined set of rules.

**Function Overview:**

1. **isNumber Function:**
   - Determines if a given string is a number or not.
   - Returns false if the string is empty or if the input is a number; otherwise, it returns true.

2. **calculateWordValue Function:**
   - Calculates the value of a word by iterating through each character and summing up their values based on a predefined set of rules.

3. **findMostSimilarWord Function:**
   - Utilizes the Levenshtein distance algorithm to find the most similar word from a list.
   - Compares the distance of each word to its minimum, determined by numeric_limits.

**Optimization Approaches:**

1. **Dynamic Programming:**
   - Initially attempted a dynamic programming approach for optimization.
   - However, found that it did not provide the desired level of speed improvement.

2. **Threading:**
   - Implemented a threading approach for optimization.
   - This approach involves parallelizing the computation of Levenshtein distances using multiple threads.
   - The threading implementation significantly improved the performance of the program.

**Results:**

- After implementing the threading approach, the program is now capable of processing 1000 lines of the main text file and calculating Levenshtein distances within a mere 3 seconds.
- Considering that the text file contains 1 million lines, extrapolating from this result indicates a significantly reduced overall processing time.
- In comparison to the original implementation, which by rough estimations would take a year to compile, the optimization has proven to be highly effective.

**Conclusion:**

In summary, my optimization strategy involved exploring dynamic programming initially, but upon realizing its limitations in terms of speed, I transitioned to a threading approach. The threading implementation, as evidenced by the remarkable reduction in processing time, has proven to be the optimal solution for this particular project.

If you have any further questions or if there are specific aspects you'd like me to elaborate on, please feel free to let me know.

