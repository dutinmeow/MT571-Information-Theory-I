#include <array>
#include <string>

const std::size_t BOARD_SIZE = 10;

class KnownBoard {
	std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> board;
	// 's' is ship, 'e' is empty

public:
	KnownBoard() {
		for (auto &row : board)
			row.fill('e');
	}

	void checkEmpty(int row1, int col1, int row2, int col2) {
		if (row1 == row2) {
			for (int col = col1; col <= col2; col++)
				if (board[row1][col] == 's')
					return false;
		} else if (col1 == col2) {
			for (int row = row1; row <= row2; row++)
				if (board[row][col1] == 's')
					return false;
		} else {
			return false;
		}
		return true;
	}

	void addShip(int row1, int col1, int row2, int col2) {
		if (row1 == row2) {
			for (int col = col1; col <= col2; col++)
				board[row1][col] = 's';
		} else if (col1 == col2) {
			for (int row = row1; row <= row2; row++)
				board[row][col1] = 's';
		}
	}

	void removeShip(int row1, int col1, int row2, int col2) {
		if (row1 == row2) {
			for (int col = col1; col <= col2; col++)
				board[row1][col] = 'e';
		} else if (col1 == col2) {
			for (int row = row1; row <= row2; row++)
				board[row][col1] = 'e';
		}	
	}
};

std::string to_string(const unknownBoard &board, bool display_unknown = false) {
	std::string ret = "  1234567890\n +----------\n";
	for (int i = 0; i < BOARD_SIZE; i++) {
		ret += std::string(1, 'a' + i) + '|';
		for (int j = 0; j < BOARD_SIZE; j++) {
			if (board[i][j] == 'e' && !display_unknown)
				ret += ' ';
			else 
				ret += board[i][j];
		}
		ret += '\n';
	}
	return ret;
}

class UnknownBoard {
	std::array<std::array<char, BOARD_SIZE>, BOARD_SIZE> board;
	// 'h' is hit, 'm' is miss, 'u' is unknown, 's' is sunk

public:
	UnknownBoard() {
		for (auto &row : board)
			row.fill('u');
	}

	void setIndex(int i, int j, char c) { board[i][j] = c; }

	const std::array<char, BOARD_SIZE> &operator[](std::size_t index) const { 
		return board[index]; 
	}
};

std::string to_string(const unknownBoard &board, bool display_unknown = false) {
	const static std::string RED = "\033[31m";
	const static std::string GREEN = "\033[32m";
	const static std::string BLUE = "\033[34m";
	const static std::string RESET = "\033[0m";

	std::string ret = "  1234567890\n +----------\n";
	for (int i = 0; i < BOARD_SIZE; i++) {
		ret += std::string(1, 'a' + i) + '|';
		for (int j = 0; j < BOARD_SIZE; j++) {
			std::string col = "";
			switch (board[i][j]) {
			case 'h':
				col = GREEN;
				break;
			case 'm':
				col = RED;
				break;
			case 's':
				col = BLUE;
				break;
			default:
				col = "";
			}
			if (col == "" && !display_unknown)
				ret += col + ' ' + RESET;
			else 
				ret += col + board[i][j] + RESET;
		}
		ret += '\n';
	}
	return ret;
}