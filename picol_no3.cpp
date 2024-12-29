#include <iostream>
#include <sstream>
#include <vector>
#include <tuple>

#ifdef _WINDOWS
#include <windows.h> // for GetFileType
#endif
#ifdef _LINUX
#include <unistd.h> // for isatty
#endif

/*
	（問題３）
	通常の数式（8桁までの整数値の四則演算のみ、括弧も使えるようにする）を入
	力し計算結果を出力するプログラムをC++あるいはJavaにより作成せよ。一般利
	用者が入力する可能性のある数式を出来る限り幅広く処理できるように仕様を明
	確に定義した上で、数式エラー時には分かりやすいエラーメッセージを出力でき
	るようにすること。
*/

using namespace std;

constexpr int BUFFERSIZE = 2048;
constexpr int MAXNUM = 99999999;
constexpr int MINNUM = -99999999;
constexpr const char* ERR_OVERFLOW = "Overflow";
constexpr const char* ERR_UNDERFLOW = "Underflow";
constexpr const char* ERR_NO_OPERATOR = "No operator";

constexpr const char* ERR_UNMATCHED_PARENTHESIS = "\" Unmatched parenthesis";
constexpr const char* ERR_ZERODIVIDE = "\" Zero divide";
constexpr const char* ERR_INVALID_PARAM = "\" Invalid parameter";
constexpr const char* ERR_INVALID_FORMULA = "\" Invalid formula";

enum class RESULT {
	OK,
	NG
};

enum class OPRTYPE {
	MUL,
	DIV,
	SUM,
	SUB,
	NUMBER
};

class OPERAND {
public:
	OPRTYPE type;
	int number;

	OPERAND(OPRTYPE type, int number) : type(type), number(number) {}
	OPERAND(OPRTYPE type) : type(type), number(0) {}
};

void check_opr_stack(vector<OPERAND> &opr_stack) {
	cout << "--------------------" << endl;
	int length = (int)opr_stack.size();
	for (int i = length - 1; -1 < i; --i) {
		string type_char;
		switch (opr_stack[i].type) {
		case OPRTYPE::MUL:
			type_char = "*";
			break;
		case OPRTYPE::DIV:
			type_char = "/";
			break;
		case OPRTYPE::SUM:
			type_char = "+";
			break;
		case OPRTYPE::SUB:
			type_char = "-";
			break;
		default:
			type_char = " ";
			break;
		}
		cout << type_char << " : " << opr_stack[i].number << endl;
	}
}

tuple<RESULT, int> execute_latest_three_stacks(vector<OPERAND>&opr_stack, string&errmsg)
{
	if (opr_stack.size() < 3) {
		errmsg += ERR_INVALID_FORMULA;
		return { RESULT::NG, 0 };
	}
	OPERAND opr3 = opr_stack.back(); opr_stack.pop_back();
	OPERAND opr2 = opr_stack.back(); opr_stack.pop_back();
	OPERAND opr1 = opr_stack.back(); opr_stack.pop_back();
	if (opr1.type != OPRTYPE::NUMBER) {
		errmsg += ERR_INVALID_FORMULA;
		return { RESULT::NG, 0 };
	}
	if (opr2.type == OPRTYPE::NUMBER) {
		errmsg += ERR_INVALID_FORMULA;
		return { RESULT::NG, 0 };
	}
	if (opr3.type != OPRTYPE::NUMBER) {
		errmsg += ERR_INVALID_FORMULA;
		return { RESULT::NG, 0 };
	}
	int num = 0;
	switch (opr2.type) {
	case OPRTYPE::MUL:
		num = opr1.number * opr3.number;
		break;
	case OPRTYPE::DIV:
		if (opr3.number == 0) {
			errmsg += ERR_ZERODIVIDE;
			return { RESULT::NG, num };
		}
		num = opr1.number / opr3.number;
		break;
	case OPRTYPE::SUM:
		num = opr1.number + opr3.number;
		break;
	case OPRTYPE::SUB:
		num = opr1.number - opr3.number;
		break;
	}
	opr_stack.emplace_back(OPRTYPE::NUMBER, num);
	if (num > MAXNUM) {
		errmsg += ERR_OVERFLOW;
		return { RESULT::NG, num };
	}
	else if (num < MINNUM) {
		errmsg += ERR_UNDERFLOW;
		return { RESULT::NG, num };
	}
	else {
		return { RESULT::OK, num };
	}
}

tuple<RESULT, int> execute_oldest_three_stacks(vector<OPERAND>& opr_stack, string& errmsg)
{
	if (opr_stack.size() < 3) {
		errmsg += ERR_INVALID_FORMULA;
		return { RESULT::NG, 0 };
	}
	OPERAND opr1 = opr_stack.front(); opr_stack.erase(opr_stack.begin());
	OPERAND opr2 = opr_stack.front(); opr_stack.erase(opr_stack.begin());
	OPERAND opr3 = opr_stack.front(); opr_stack.erase(opr_stack.begin());
	if (opr1.type != OPRTYPE::NUMBER) {
		errmsg += ERR_INVALID_FORMULA;
		return { RESULT::NG, 0 };
	}
	if (opr2.type == OPRTYPE::NUMBER) {
		errmsg += ERR_INVALID_FORMULA;
		return { RESULT::NG, 0 };
	}
	if (opr3.type != OPRTYPE::NUMBER) {
		errmsg += ERR_INVALID_FORMULA;
		return { RESULT::NG, 0 };
	}
	int num = 0;
	switch (opr2.type) {
	case OPRTYPE::MUL:
		num = opr1.number * opr3.number;
		break;
	case OPRTYPE::DIV:
		if (opr3.number == 0) {
			errmsg += ERR_ZERODIVIDE;
			return { RESULT::NG, num };
		}
		num = opr1.number / opr3.number;
		break;
	case OPRTYPE::SUM:
		num = opr1.number + opr3.number;
		break;
	case OPRTYPE::SUB:
		num = opr1.number - opr3.number;
		break;
	}
	opr_stack.insert(opr_stack.begin(), { OPRTYPE::NUMBER, num });
	if (num > MAXNUM) {
		errmsg += ERR_OVERFLOW;
		return { RESULT::NG, num };
	}
	else if (num < MINNUM) {
		errmsg += ERR_UNDERFLOW;
		return { RESULT::NG, num };
	}
	else {
		return { RESULT::OK, num };
	}
}

tuple<RESULT, int> execute_all_stacks(vector<OPERAND>& opr_stack, string& errmsg)
{
	size_t count = opr_stack.size() / 2;
	RESULT isok;
	int num;
	for (size_t i = 0; i < count; i++) {
		std::tie(isok, num) = execute_oldest_three_stacks(opr_stack, errmsg);
		if (isok == RESULT::NG) {
			return { isok, num };
		}
	}
	return { isok, num };
}

tuple<RESULT, int> parse_formula(const string& formula, string& errmsg, bool require_close_parenthesis) {
	istringstream iss(formula);
	char ch;
	int num = 0;
	int sign = 1;
	bool immediately = false;
	vector<OPERAND> opr_stack;
	RESULT isok;
	iss >> std::noskipws;

	while(iss >> ch){
		errmsg += ch;

		switch (ch) {
		case '(':
			{
				int nestlevel = 1;
				string sub_formula;
				while (iss >> ch)
				{
					if (ch == '(')
						nestlevel++;
					else if (ch == ')')
						nestlevel--;
						if (nestlevel < 1)
							break;
						sub_formula += ch;
				}
				if (nestlevel > 0) {
					errmsg += sub_formula;
					errmsg += ERR_UNMATCHED_PARENTHESIS;
					return { RESULT::NG, 0 };
				}
				std::tie(isok, num) = parse_formula(sub_formula, errmsg, require_close_parenthesis);
				if (isok == RESULT::NG)
					return { RESULT::NG, 0 };
				opr_stack.emplace_back(OPRTYPE::NUMBER, num);
				num = 0;
				if (immediately) {
					immediately = false;
					std::tie(isok, num) = execute_latest_three_stacks(opr_stack, errmsg);
					if (isok == RESULT::NG) {
						return { RESULT::NG, 0 };
					}
					num = 0;
				}
				ch = ')';
			}
			break;

		case '+':
		case '-':
			if (opr_stack.size() == 0) {
				if (ch == '-') 
					sign = -1;
				else
					sign = 1;				
			}
			else {
				OPERAND operand = opr_stack.back();
				switch (operand.type) {
				case OPRTYPE::NUMBER:
					if (ch == '+')
						opr_stack.emplace_back(OPRTYPE::SUM);
					else
						opr_stack.emplace_back(OPRTYPE::SUB);
					immediately = false;
					break;
				case OPRTYPE::DIV:
				case OPRTYPE::MUL:
				case OPRTYPE::SUM:
				case OPRTYPE::SUB:
					if (ch == '-')
						sign = sign * -1;
					else
						sign = sign * 1;
				}
			}
			break;

		case '*':
			opr_stack.emplace_back(OPRTYPE::MUL);
			immediately = true;
			break;

		case '/':
			opr_stack.emplace_back(OPRTYPE::DIV);
			immediately = true;
			break;

		case ' ':
			break;

		default:
			if (isdigit(ch)) {
				errmsg.pop_back();
				do {
					errmsg += ch;
					if (isdigit(ch)) {
						num = num * 10 + ch - '0';
					}
					else {
						errmsg.pop_back();
						iss.seekg(-1, std::ios_base::cur);
						break;
					}
				} while (iss >> ch);
				num *= sign;
				sign = 1;
				opr_stack.emplace_back(OPRTYPE::NUMBER, num);
				
				num = 0;
				if (immediately) {
					immediately = false;
					std::tie(isok, num) = execute_latest_three_stacks(opr_stack, errmsg);
					if (isok == RESULT::NG) {
						return { RESULT::NG, 0 };
					}
					num = 0;
				}
			}
			else if (ch == ')') {
				errmsg += ERR_UNMATCHED_PARENTHESIS;
				return { RESULT::NG, 0 };
			}
			else {
				errmsg += ERR_INVALID_PARAM;
				return { RESULT::NG, 0 };
			}
			break;
		}
		//check_opr_stack(opr_stack);
	}

	std::tie(isok, num) = execute_all_stacks(opr_stack, errmsg);

	if (isok == RESULT::NG) {
		return { isok, num };
	}

	if (opr_stack.size() == 1 && opr_stack.back().type == OPRTYPE::NUMBER) {
		return { RESULT::OK, opr_stack.back().number };
	}
	else {
		errmsg += ERR_INVALID_FORMULA;
		return { RESULT::NG, 0 };
	}
}

/// <summary>
/// 入力式を整形（処理しやすいように中間データに変換するような考え方）
/// </summary>
/// 対応内容
/// * 括弧の前に*が無くても乗算として認識する
/// 
enum class CHARTYPE
{
	LINETOP,
	OPEN_PARENTHESIS,
	CLOSE_PARENTHESIS,
	NUMBER,
	OPERATOR,
	UNKNOWN
};

CHARTYPE chartype(char ch)
{
	static char _ch = 0;
	static CHARTYPE _cache = CHARTYPE::UNKNOWN;

	if (_ch == ch)
		return _cache;

	_ch = ch;

	if (ch == 0)
		return _cache = CHARTYPE::LINETOP;
	else if (ch == '(')
		return _cache = CHARTYPE::OPEN_PARENTHESIS;
	else if (ch == ')')
		return _cache = CHARTYPE::CLOSE_PARENTHESIS;
	else if (isdigit(ch))
		return _cache = CHARTYPE::NUMBER;
	else {
		switch (ch) {
		case '+':
		case '-':
		case '*':
		case '/':
			return _cache = CHARTYPE::OPERATOR;
		default:
			return _cache = CHARTYPE::UNKNOWN;
		}
	}
}

string reformat(string& formula)
{
	istringstream iss(formula);
	char ch, prev_ch = 0;
	string formed;

	iss >> std::noskipws;
	while (iss >> ch) {
		switch (chartype(ch)) {
		case CHARTYPE::OPEN_PARENTHESIS:
			switch (chartype(prev_ch)) {
			case CHARTYPE::CLOSE_PARENTHESIS:
			case CHARTYPE::NUMBER:
				formed += '*';
				break;
			default:
				break;
			}
			break;

		case CHARTYPE::NUMBER:
			if (chartype(prev_ch) == CHARTYPE::CLOSE_PARENTHESIS)
				formed += '*';
			break;

		default:
			break;
		}
		formed += ch;
		if (ch != ' ')
			prev_ch = ch;
	}
	return formed;
}

bool is_redirect()
{
#ifdef _WINDOWS
	if (GetFileType(GetStdHandle(STD_INPUT_HANDLE)) == FILE_TYPE_CHAR)
#elif  _LINUX
	if (isatty(fileno(stdin)))
#endif
		return false;
	else
		return true;
}

int main(int argc, char* argv[]) {
	string formula;
	bool interative_mode = false;
	string errmsg;
	RESULT err;
	bool redirect = is_redirect();

	do {
		if (argc < 2) {
		REPEAT:
			interative_mode = true;

			if(!redirect)
				cout << "PICO> ";

			string line;
			if (std::getline(std::cin, line)) {
				if (line == "quit") {
					return 0;
				}

				formula += line;

				if (formula.size() == 0)
					goto REPEAT;
			}
			else
				return 0;
		}
		else {
			for (int i = 1; i < argc; i++) {
				formula += argv[i];
				formula += ' ';
			}
		}

		int num;

		errmsg += "\"";

		std::tie(err, num) = parse_formula(reformat(formula), errmsg, false);

		if (num > MAXNUM) {
			err = RESULT::NG;
			errmsg = ERR_OVERFLOW;
		}

		if (num < MINNUM) {
			err = RESULT::NG;
			errmsg = ERR_UNDERFLOW;
		}

		if (err == RESULT::NG) {
			if (interative_mode && !redirect)
				cout << " [ERROR: " << errmsg << "]" << endl;
			else
				cout << formula << " [ERROR: " << errmsg << "]" << endl;
		}
		else {
			if (interative_mode && !redirect)
				cout << num << endl;
			else
				cout << formula << " = " << num << endl;
		}

		formula.clear();
		errmsg.clear();

	} while (interative_mode);

	return 0;
}
