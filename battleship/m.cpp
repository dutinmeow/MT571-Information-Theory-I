#include <bits/stdc++.h>
#include <local/random>
#include <local/debug>

namespace std {

template<class Fun>
class y_combinator_result {
	Fun fun_;
public:
	template<class T>
	explicit y_combinator_result(T &&fun): fun_(std::forward<T>(fun)) {}

	template<class ...Args>
	decltype(auto) operator()(Args &&...args) {
		return fun_(std::ref(*this), std::forward<Args>(args)...);
	}
};

template<class Fun>
decltype(auto) y_combinator(Fun &&fun) {
	return y_combinator_result<std::decay_t<Fun>>(std::forward<Fun>(fun));
}

} // namespace std

const bool FULL_AUTO = true;      // allows program to test itself
const bool CONFIRM_VALID = false; // user confirmation that input data is correct

const short N = 10;               // board size
const int MAX_ITR = 1'000'000;    // number of configurations to loop through (both valid and invalid)
const int MIN_VAL = 50;           // minimum number of valid configurations

const int DBG_FLT_LEN = 6;        // length of displayed decimal places
const int DBG_BOARD_NUM = 0;      // number of example boards to display

const short PARITY_OFFSET = 0.1;  // heuristic to boost probability of odd squares
const float HIT_BOOSTER = 0.95;   // heuristic to boost probability of hits
const float HIT_THESHOLD = 0.75;  // heuristic to shoot when hit probabilty exceeds value

const double EPS = 1e-9;

const std::string HIT_SYB = "✅";
const std::string MISS_SYB = "❌";
const std::string SUNK_SYB = "🤯";
const std::string KILL_SYB = "💀";
const std::string UNKNOWN_SYB = "🟦";

const char HIT_CHAR = 'H';
const char MISS_CHAR = 'M';
const char SUNK_CHAR = 'S';
const char KILL_CHAR = 'K';
const char UNKNOWN_CHAR = 'U';

const float HIT_VAL = 9.0;
const float MISS_VAL = 9.1;
const float SUNK_VAL = 9.2;
const float KILL_VAL = 9.3;
const float UNKNOWN_VAL = 9.4;

const short UNKNOWN_INT = -1;
const short KNOWN_INT = -2;

const int GRAD_SIZE = 5;
const std::array<std::string, GRAD_SIZE> GRAD_SYB = {"🖕🏿", "🖕🏾", "🖕🏽", "🖕🏼", "🖕🏻"};
const std::string OPT_SYB = "🖕";

short compressState(short x, short y, short dir) { return x * N + y + dir * N * N; }

std::tuple<short, short, short> uncompressState(short s) { 
	return {s / N % N, s % N, s / N / N}; 
}

void displayRawDistribution(const std::array<std::array<float, N>, N> &arr, std::string title = "") {
	if (title != "") 
		std::cerr << "#####\t\t\t" << title << "\t\t\t#####\n";
	std::cerr << "    ";
	for (int i = 0; i < N; i++) {
		std::cerr << i;
		for (int j = 0; j <= DBG_FLT_LEN + 1; j++)
			std::cerr << ' ';
	}
	std::cerr << '\n';
	std::cerr << "  +";
	for (int i = 0; i < N; i++)
		for (int j = 0; j <= DBG_FLT_LEN + 2; j++)
			std::cerr << '-';
	std::cerr << '\n';
	for (int i = 0; i < N; i++) {
		std::cerr << i << " | ";
		for (int j = 0; j < N; j++) {
			std::cerr << arr[i][j];
			if (j < N - 1)
				std::cerr << ' ';
		}
		std::cerr << '\n';
	}
}

void displayGraphicGradient(const std::array<std::array<float, N>, N> &arr, std::string title = "") {
	if (title != "") 
		std::cerr << "#####\t\t\t" << title << "\t\t\t#####\n";
	float amx = 0.0, amn = 1.0;
	for (int i = 0; i < N; i++) {
		for (int j = 0; j < N; j++) {
			if (arr[i][j] >= 2.0)
				continue;
			amx = std::max(amx, arr[i][j]);
			amn = std::min(amn, arr[i][j]);
		}
	}
	float sep = (amx - amn) / (float)GRAD_SIZE;
	std::cerr << "    0 1 2 3 4 5 6 7 8 9\n  +---------------------\n";
	for (int i = 0; i < N; i++) {
		std::cerr << i << " | ";
		for (int j = 0; j < N; j++) {
			if (arr[i][j] == MISS_VAL) {
				std::cerr << MISS_SYB;
			} else if (arr[i][j] == HIT_VAL) {
				std::cerr << HIT_SYB;
			} else if (arr[i][j] == KILL_VAL) {
				std::cerr << KILL_SYB;
			} else if (arr[i][j] == SUNK_VAL) {
				std::cerr << SUNK_SYB;
			} else {
				if (arr[i][j] == amx) {
					std::cerr << OPT_SYB;
				} else {
					int ind = (arr[i][j] - amn + EPS) / sep;
					if (ind == -1)
						ind++;
					if (ind == GRAD_SIZE)
						ind--;
					assert(0 <= ind && ind < GRAD_SIZE);
					std::cerr << GRAD_SYB[ind];
				}
			}
		}
		std::cerr << '\n';
	}
}

void displayGraphicGradient(const std::array<std::array<char, N>, N> &arr, std::string title = "") {
	if (title != "") 
		std::cerr << "#####\t\t\t" << title << "\t\t\t#####\n";
	std::cerr << "    0 1 2 3 4 5 6 7 8 9\n  +---------------------\n";
	for (int i = 0; i < N; i++) {
		std::cerr << i << " | ";
		for (int j = 0; j < N; j++) {
			if (arr[i][j] == MISS_CHAR) {
				std::cerr << MISS_SYB;
			} else if (arr[i][j] == HIT_CHAR) {
				std::cerr << HIT_SYB;
			} else if (arr[i][j] == SUNK_CHAR) {
				std::cerr << SUNK_SYB;
			} else if (arr[i][j] == KILL_CHAR) {
				std::cerr << KILL_SYB;
			} else {
				std::cerr << UNKNOWN_SYB;
			}
		}
		std::cerr << '\n';
	}
}

int main() {
	std::cerr << std::fixed << std::setprecision(6);

	int numShips = 5;                              // number of ships
	std::vector<short> shipSizes{2, 3, 3, 4, 5};   // sizes of active ships
	std::array<std::array<char, N>, N> board;      // known board so far

	int moves = 0;                                 // moves so far
	short numHits = 0;                             // number of hits/sink verdicts recieved
	short totalNumHits;                            // sum of ship sizes

	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			board[i][j] = UNKNOWN_CHAR;
	totalNumHits = std::accumulate(shipSizes.begin(), shipSizes.end(), 0);

	bool automaticEnabled;
	if (FULL_AUTO) {
		automaticEnabled = true;
	} else {
		std::string userResponse;
		std::cout << "Enable automatic tester?\n";
		std::cin >> userResponse;
		bool automaticEnabled = userResponse == "Y" || userResponse == "YES";
	}

	std::array<std::array<short, N>, N> answerBoard;
	std::array<std::array<char, N>, N> displayAnswerBoard;
	if (automaticEnabled) {
		std::cerr << CYAN << BOLD << "#####\t\t\tAutomatic Mode is Currently Enabled\t\t\t#####\n" << RESET;

		for (int i = 0; i < N; i++) {
			for (int j = 0; j < N; j++) {
				answerBoard[i][j] = UNKNOWN_INT;
				displayAnswerBoard[i][j] = UNKNOWN_CHAR;
			}
		}
		for (int i = 0; i < numShips; i++) {
			short len = shipSizes[i];
			while (true) {
				int x = local::rng::rint(0, N - 1);
				int y = local::rng::rint(0, N - 1);
				int d = local::rng::rint(0, 1);
				if (d == 0) {
					if (x + len - 1 < N) {
						bool another = false;
						for (int xx = x; xx < x + len; xx++) {
							if (answerBoard[xx][y] != -1) {
								another = true;
								break;
							}
						}
						if (another)
							continue;
						for (int xx = x; xx < x + len; xx++) {
							answerBoard[xx][y] = i;
							displayAnswerBoard[xx][y] = HIT_CHAR;
						}
						break;
					}
				} else {
					if (y + len - 1 < N) {
						bool another = false;
						for (int yy = y; yy < y + len; yy++) {
							if (answerBoard[x][yy] != -1) {
								another = true;
								break;
							}
						}
						if (another)
							continue;
						for (int yy = y; yy < y + len; yy++) {
							answerBoard[x][yy] = i;
							displayAnswerBoard[x][yy] = HIT_CHAR;
						}
						break;
					}
				}
			}
		}

		displayGraphicGradient(displayAnswerBoard, "Randomly Generated Board Configuration:");
		
		if (CONFIRM_VALID) {
			std::string valid;
			std::cout << "Confirm that the generated board is valid: ";
			while (true) {
				std::cin >> valid;
				if (valid == "NO") {
					std::cout << CYAN << BOLD << "Test Data Invalid, Aborting" << RESET;
					return 0;
				} else if (valid == "YES") {
					break;
				} else {
					std::cout << "Invalid answer, please enter again: ";
				}
			}
		}

	}

	for (; numHits < totalNumHits; moves++) {

		// Generate all possible locations for each ship

		std::cerr << YELLOW << "Step 1: Generate all possible locations for each ship\n" << RESET;
		std::vector<std::vector<short>> shipPositions(numShips);
		for (short i = 0; i < numShips; i++) {
			short len = shipSizes[i];
			for (short x = 0; x < N; x++) {
				for (short y = 0; y < N; y++) {
					if (x + len - 1 < N) {
						short numSunk = 0;
						bool seenUnknown = false, seenMiss = false;
						for (int xx = x; xx < x + len; xx++) {
							if (board[xx][y] == SUNK_CHAR)
								numSunk++;
							else if (board[xx][y] == UNKNOWN_CHAR)
								seenUnknown = true;
							else if (board[xx][y] == MISS_CHAR || board[xx][y] == KILL_CHAR)
								seenMiss = true;
						}
						if (!seenMiss && numSunk <= 1 && !(numSunk == 1 && seenUnknown)) 
							shipPositions[i].push_back(compressState(x, y, 0));
					}
					if (y + len - 1 < N) {
						short numSunk = 0;
						bool seenUnknown = false, seenMiss = false;
						for (int yy = y; yy < y + len; yy++) {
							if (board[x][yy] == SUNK_CHAR)
								numSunk++;
							else if (board[x][yy] == UNKNOWN_CHAR)
								seenUnknown = true;
							else if (board[x][yy] == MISS_CHAR || board[x][yy] == KILL_CHAR)
								seenMiss = true;
						}
						if (!seenMiss && numSunk <= 1 && !(numSunk == 1 && seenUnknown)) 
							shipPositions[i].push_back(compressState(x, y, 1));
					}
				}
			}
		}

		// Generate some combinations of ships

		std::cerr << YELLOW << "Step 2: Generate random configurations\n" << RESET;
		int validConfigs = 0;
		std::array<std::array<short, N>, N> curFreq;
		std::array<std::array<int, N>, N> freqHit;
		for (short x = 0; x < N; x++)
			for (short y = 0; y < N; y++)
				freqHit[x][y] = 0;
		for (int itr = 0; itr < MAX_ITR || validConfigs < MIN_VAL; itr++) {
			for (int x = 0; x < N; x++)
				for (int y = 0; y < N; y++)
					curFreq[x][y] = 0;
			bool invalid = false;
			for (int i = 0; i < numShips; i++) {
				int ind = local::rng::rint(0, (int)shipPositions[i].size() - 1);
				auto [x, y, d] = uncompressState(shipPositions[i][ind]);
				if (d == 0) {
					for (int xx = x; xx < x + shipSizes[i]; xx++) {
						if (curFreq[xx][y] > 0) {
							invalid = true;
							break;
						}
						curFreq[xx][y]++;
					}
				} else {
					for (int yy = y; yy < y + shipSizes[i]; yy++) {
						if (curFreq[x][yy] > 0) {
							invalid = true;
							break;
						}
						curFreq[x][yy]++;
					}
				}
				if (invalid)
					break;
			}
			
			if (invalid) 
				continue;
			for (int x = 0; x < N; x++) {
				for (int y = 0; y < N; y++) {
					if (board[x][y] == HIT_CHAR && curFreq[x][y] == 0) {
						invalid = true;
						break;
					}
				}
				if (invalid)
					break;
			}

			if (invalid) 
				continue;
			for (int x = 0; x < N; x++) 
				for (int y = 0; y < N; y++) 
					if (curFreq[x][y] == 1)
						freqHit[x][y]++;
			validConfigs++;

			if (validConfigs < DBG_BOARD_NUM) {
				std::cerr << "  0123456789\n +----------\n";
				for (int x = 0; x < N; x++) {
					std::cerr << x << '|';
					for (int y = 0; y < N; y++) {
						std::cerr << (curFreq[x][y] == 0 ? ' ' : '*') << RESET;
					}
					std::cerr << '\n';
				}
				std::cerr << '\n';
			}
		}

		std::cerr << PURPLE << "Number of Valid Configurations are " << validConfigs << RESET << '\n';

		// calculate entropy of each cell

		std::cerr << YELLOW << "Step 3: Calculate the (adjusted) entropy of shooting each square\n" << RESET;
		float maxEntropy = 0.0;
		short shotX, shotY;
		std::array<std::array<float, N>, N> entropy;
		for (short x = 0; x < N; x++) {
			for (short y = 0; y < N; y++) {
				if (board[x][y] == MISS_CHAR) {
					entropy[x][y] = MISS_VAL;
				} else if (board[x][y] == HIT_CHAR) {
					entropy[x][y] = HIT_VAL;
				} else if (board[x][y] == SUNK_CHAR) {
					entropy[x][y] = SUNK_VAL;
				} else if (board[x][y] == KILL_CHAR) {
					entropy[x][y] = KILL_VAL;
				} else {
					double hit = (double)freqHit[x][y] / validConfigs;
					double miss = 1.0 - hit;
					if (hit > HIT_THESHOLD) {
						maxEntropy = 1.0;
						shotX = x, shotY = y;
					} else {
						entropy[x][y] = (hit < EPS || miss > 1.0 - EPS ? 0.0 : hit * log2(1.0 / hit) * HIT_BOOSTER) + 
							(miss < EPS || miss > 1.0 - EPS ? 0.0 : miss * log2(1.0 / miss) * (1.0 - HIT_BOOSTER));
						if ((x + y) % 2 == 0)
							entropy[x][y] += PARITY_OFFSET;
						if (entropy[x][y] > maxEntropy) {
							maxEntropy = entropy[x][y];
							shotX = x, shotY = y;
						}
					}
				}
			}
		}

		displayRawDistribution(entropy, "Entropy Distribution");

		displayGraphicGradient(entropy, "Entropy Heat Map");

		std::cerr << PURPLE << "We've hit " << numHits << " out of " << totalNumHits << " spots so far\n";
		std::cerr << "Currently at Step " << moves << '\n' << RESET;

		std::cerr << GREEN << BOLD << "\n\n\n\n\n\nMove " << moves + 1 << ": Guess at (" << shotX << ", " << shotY << ")\n" << RESET;

		std::cout << "Response: ";
		std::string verdict;
		if (automaticEnabled) {
			assert(answerBoard[shotX][shotY] != KNOWN_INT);
			if (answerBoard[shotX][shotY] == UNKNOWN_INT) {
				verdict = "MISS";
			} else {
				int tem = answerBoard[shotX][shotY];
				answerBoard[shotX][shotY] = KNOWN_INT;
				bool anyLeft = false;
				for (int i = 0; i < N; i++) {
					for (int j = 0; j < N; j++) {
						if (answerBoard[i][j] == tem) {
							anyLeft = true;
							break;
						}
					}
					if (anyLeft)
						break;
				}
				if (anyLeft)
					verdict = "HIT";
				else
					verdict = "SUNK";
			}
			std::cout << verdict << '\n';
		} else {
			while (true) {
				std::cin >> verdict;
				if (verdict == "STOP" || verdict == "BREAK" || verdict == "RETURN") {
					std::cout << CYAN << "Process killed, aborting now\n" << RESET;
					return 0;
				} else if (verdict != "HIT" && verdict != "MISS" && verdict != "SUNK") {
					std::cout << "Invalid response, enter again: ";
				} else {
					break;
				}
			}
		}

		if (verdict == "HIT") {
			board[shotX][shotY] = HIT_CHAR;
			numHits++;
		} else if (verdict == "MISS") {
			board[shotX][shotY] = MISS_CHAR;
		} else if (verdict == "SUNK") {
			board[shotX][shotY] = SUNK_CHAR;
			numHits++;

			std::cerr << YELLOW << "Step 0: Removing sunk ships\n" << RESET;

			std::cerr << YELLOW << "--> Step 0.1: Generate ship positions\n" << RESET;

			std::vector<std::vector<short>> shipPositions(numShips);
			for (short i = 0; i < numShips; i++) {
				short len = shipSizes[i];
				for (short x = 0; x < N; x++) {
					for (short y = 0; y < N; y++) {
						if (x + len - 1 < N) {
							short numSunk = 0;
							bool seenUnknown = false, seenMiss = false, seenHit = false;
							for (short xx = x; xx < x + len; xx++) {
								if (board[xx][y] == SUNK_CHAR)
									numSunk++;
								else if (board[xx][y] == UNKNOWN_CHAR)
									seenUnknown = true;
								else if (board[xx][y] == MISS_CHAR || board[xx][y] == KILL_CHAR)
									seenMiss = true;
								else if (board[xx][y] == HIT_CHAR)
									seenHit = true;
							}
							if (seenHit && numSunk <= 1 && !seenMiss && !(numSunk == 1 && seenUnknown)) 
								shipPositions[i].push_back(compressState(x, y, 0));
						}
						if (y + len - 1 < N) {
							short numSunk = 0;
							bool seenUnknown = false, seenMiss = false, seenHit = false;
							for (int yy = y; yy < y + len; yy++) {
								if (board[x][yy] == SUNK_CHAR)
									seenSunk++;
								else if (board[x][yy] == UNKNOWN_CHAR)
									seenUnknown = true;
								else if (board[x][yy] == MISS_CHAR || board[x][yy] == KILL_CHAR)
									seenMiss = true;
								else if (board[x][yy] = HIT_CHAR)
									seenHit = true;
							}
							if (seenHit && numSunk <= 1 && !seenMiss && !(numSunk == 1 && seenUnknown)) 
								shipPositions[i].push_back(compressState(x, y, 1));
						}
					}
				}
			}

			std::cerr << YELLOW << "--> Step 0.2: Generate ship masks\n" << RESET;

			std::array<std::array<short, N>, N> visBoard; 
			std::array<std::array<short, N>, N> mask;
			for (int i = 0; i < N; i++) {
				for (int j = 0; j < N; j++) {
					visBoard[i][j] = UNKNOWN_INT;
					mask[i][j] = 0;
				}
			}

			std::y_combinator([&](auto self, int i) -> void {
				if (i == numShips) {
					return;
				}

				short len = shipSizes[i];
				self(i + 1);
			})(0);
		}
	}

	std::cout << BOLD << CYAN << "\n\nNumber of Total Moves is " << moves << RESET << "\n\n";
	displayGraphicGradient(board, "Final Board");

	if (automaticEnabled) {
		std::cerr << "\n\n";
		displayGraphicGradient(displayAnswerBoard, "Automatically Generated Initial Board");
	}
}