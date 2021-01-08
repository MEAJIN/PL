#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

enum class tokenType //토큰 타입을 열거 클래스로 정의
{
	none,
	eof,		// 문자열의 끝
	error,		// 비정상 토큰(에러)
	plus,		// +
	minus,		// -
	asterisk,	// *
	divide,		// /
	assign,		// =
	equal,		// ==
	greater,	// <
	greaterequal,// <=
	less,		// >
	lessequal,	// >=
	notequal,	// !=
	lbracket,	// {
	rbracket,	// }
	lparen,		// (
	rparen,		// )
	_if,		// if
	_else,		// else
	integer,	// 정수
	identifier	// 식별자
};

struct token	//토큰에 대한 정보를 가지는 구조체
{
	string str;		//토큰의 내용 (ex. >, >=, +  등등)
	tokenType type;	//토큰의 타입 (ex. tokenType::less, tokenType::lessequal, tokenType::plus 등등)
	int line;		//토큰이 발견된 라인 수
	token(string s = string(), tokenType t = tokenType::none, int _line = 0)
	{
		str = s;
		type = t;
		line = _line;
	}
};

struct Err
{
	token t;
	string msg;
	Err(token _t, string _msg)
	{
		t = _t; msg = _msg;
	}
};
//token 벡터의 상수 반복자가 자주 쓰이기에 Tokit으로 typedef을 이용하여 타입으로서 정의
typedef vector<token>::const_iterator Tokit;	

// 토큰이 무조건 잘려야하는 문자의 경우에만 true를 반환하는 함수
bool IsCutCharacter(wchar_t c);

//문자열을 입력받아서 token의 vector로 반환하는 함수(어휘분석기)
vector<token> lex(const string& str);


/*#######################################################################
아래 모든 함수들은 성공하면 일치한 토큰의 수를 반환, 실패하면 음수를 반환
#########################################################################*/

//<S>-><sentence>
int s(Tokit begin, Tokit end);

//<sentence> -> <if>
int sentence(Tokit begin, Tokit end);

/*
<if> -> "if" "(" <cmp_expr> ")" <block>
{ "else" "if" "(" <cmp_expr> ")" <block> }
[ "else" <block> ]
*/
int _if(Tokit begin, Tokit end);

//<block> -> "{"  "}"
int block(Tokit begin, Tokit end);

//<expr> -> <term> {(+|-) <term>}
int expr(Tokit begin, Tokit end);

//<term> -> <factor> {(*|/) <factor>}
int term(Tokit begin, Tokit end);

//<factor> -> id | int_constant
int factor(Tokit begin, Tokit end);

//<cmp_op> -> "==" | "!=" | ">" | "<" | ">=" | "<="
int cmp_op(Tokit begin, Tokit end);

//<cmp_expr> -> <expr> <cmp_op> <expr>
int cmp_expr(Tokit begin, Tokit end);

vector<token> Token;	//어휘분석기(lexr)가 결과를 저장할 벡터 변수
vector<Err> Error;		//에러를 저장할 벡터 변수
void main()
{
	ifstream in("source.txt");	//프로그램 소스코드 파일

	//프로그램 소스코드 파일 읽기
	string ss = "";
	char ch;
	while (in.get(ch))
	{
		ss += ch;
	}
	in.close();		//파일 닫기

	//어휘분석기 가동
	Token = lex(ss);

	//어휘분석 결과 출력(DEBUGING)
	for (int i = 0; i < Token.size(); i++)
	{
		if (Token[i].type == tokenType::eof)
		{
			cout << i << ", EOF DETECTED" << endl;
		}
		else
		{
			cout << i << "-LINE: " << Token[i].line << ", TOKEN: " << Token[i].str << endl;
		}
		
	}
	//시작기호 <s> 파싱
	int t=s(Token.begin(), Token.end());
	
	if (t == Token.size())	//시작함수 s가 반환한 수와 토큰의 수가 일치한다면 정상
	{
		cout << "정상적으로 파싱되었습니다." << endl;
	}
	/*시작함수 s가 반환한 수와 토큰의 수가 일치하지 않는다면
	어딘가에서 에러가 발생한 것이므로 Err 클래스를 담는 벡터 Error의 내용 출력*/
	else 
	{
		for (int i = 0; i < Error.size(); i++)
		{
			cout << Error[i].t.line << "번째 줄, " << Error[i].t.str << " 문자 검증중 에러 발생" << endl;
			cout << Error[i].msg << endl;
		}
	}
}

vector<token> lex(const string& str)
{
	vector<token> ret;

	auto it = str.begin();
	auto marker = str.begin();

	/* 어떤 타입의 토큰을 작업하는지를 저장하는 변수.
	처음에는 tokenType::none 스테이지.*/
	tokenType stage = tokenType::none;
	int line = 1;	// 줄번호
	string s;		// 임시 문자열

	// it가 끝에 다다를때까지 반복
	for (; it != str.end(); it++)
	{
		switch (stage)
		{
		// none 스테이지 일 경우
		case tokenType::none:
			switch (*it)
			{
			case '\n':	// 줄바꿈 문자열일 경우는 line만 늘려줌
				line++;
			case ' ':	// 공백들이므로 모조리 무시
			case '\t':
			case '\r':
				break;
			case '0':	// 숫자인 경우는 스테이지를 integer로 설정하고 marker를 현재 지점으로 설정
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				stage = tokenType::integer;
				marker = it;
				break;	
			case '<':	// < 문자의 경우 < 이거나 <= 일 경우가 있기 때문에 다음 문자까지 참조
				if (it + 1 != str.end() && it[1] == '=')	// <= 의 경우
				{
					ret.push_back(token("<=", tokenType::lessequal, line));
					it++;
				}
				else		// < 의 경우
				{
					ret.push_back(token("<", tokenType::less, line));
				}
				break;
			case '>':	// > 문자의 경우 > 이거나 >= 일 경우가 있기 때문에 다음 문자까지 참조
				if (it + 1 != str.end() && it[1] == '=')	// >= 의 경우
				{
					ret.push_back(token(">=", tokenType::greaterequal, line));
					it++;
				}
				else		// > 의 경우
				{
					ret.push_back(token(">", tokenType::greater, line));
				}
				break;
			case '!':	// ! 문자의 경우 != 일 경우밖에 없으므로 다음 문자 참조
				if (it + 1 != str.end() && it[1] == '=')	// != 의 경우
				{
					ret.push_back(token("!=", tokenType::notequal, line));
					it++;
				}
				else // 뒷 문자가 = 가 아니면 무조건 에러 (ex. !a, !3 등
				{
					ret.push_back(token("!", tokenType::error, line));
				}
				break;
			case '=':	// = 문자의 경우 = 이거나 == 일 경우가 있기 때문에 다음 문자까지 참조
				if (it + 1 != str.end() && it[1] == '=')	// ==의 경우
				{
					ret.push_back(token("==", tokenType::equal, line));
					it++;
				}
				else	//=의 경우
				{
					ret.push_back(token("=", tokenType::assign, line));
				}
				break;
			case '+':
				ret.push_back(token(string(it, it + 1), tokenType::plus, line));
				break;
			case '-':
				ret.push_back(token(string(it, it + 1), tokenType::minus, line));
				break;
			case '*':
				ret.push_back(token(string(it, it + 1), tokenType::asterisk, line));
				break;
			case '/':
				ret.push_back(token(string(it, it + 1), tokenType::divide, line));
				break;
			case '{':
				ret.push_back(token(string(it, it + 1), tokenType::lbracket, line));
				break;
			case '}':
				ret.push_back(token(string(it, it + 1), tokenType::rbracket, line));
				break;
			case '(':
				ret.push_back(token(string(it, it + 1), tokenType::lparen, line));
				break;
			case ')':
				ret.push_back(token(string(it, it + 1), tokenType::rparen, line));
				break;
			default:	// 위 이외의 경우엔 다 식별자로 취급하기 때문에 marker를 시작지점에 찍어놓습니다
				stage = tokenType::identifier;
				marker = it;
				break;
			}
			break;
			// 스테이지가 integer인 경우 (숫자로 시작한 경우)
		case tokenType::integer:
			// 토큰을 자르는 문자를 만났으면 여기 지점 까지만 숫자로 판단
			// 스테이지를 기본으로 돌리고, 이 문자 전까지 토큰에 삽입
			if (IsCutCharacter(*it))
			{
				stage = tokenType::none;
				ret.push_back(token(string(marker, it--), tokenType::integer, line));
			}
			// 숫자가 아닌 놈이 뒤에 붙어있으면 에러
			else if (!isdigit(*it))
			{
				stage = tokenType::none;
				ret.push_back(token(string(marker, it), tokenType::error, line));
				--it;
			}
			break;
			// 스테이지가 식별자인 경우 (기타 일반적인 문자로 시작한경우, 특문시작X)
		case tokenType::identifier:
			// 토큰을 자르는 문자를 만났으면, 여기까지 문자열을 잘라서 키워드와 비교
			if (IsCutCharacter(*it))
			{
				stage = tokenType::none;
				string s(marker, it--);
				if (s == "if")
					ret.push_back(token(s, tokenType::_if, line));
				else if (s == "else")
					ret.push_back(token(s, tokenType::_else, line));
				// 아무 키워드에 해당안되면 그냥 일반 식별자로 결론
				else
				{
					ret.push_back(token(s, tokenType::identifier, line));
				}
			}
			break;
		}
	}
	// 다 돌고 난 뒤에 제일 마지막에 eof를 삽입
	ret.push_back(token("", tokenType::eof, line));
	return ret;
}

// 토큰이 무조건 잘려야하는 문자의 경우에만 true를 반환하는 함수
bool IsCutCharacter(wchar_t c)
{
	switch (c)
	{
	case ' ':
	case '\t':
	case '\r':
	case '\n':
	case '<':
	case '>':
	case '=':
	case '!':
	case '-':
	case '*':
	case '/':
	case ';':
	case '{':
	case '}':
	case '(':
	case ')':
		return true;
	}
	return false;
}


int s(Tokit begin, Tokit end)
{
	cout << "ENTER <s>" << endl;
	int i = 0;	//사용한 토큰 수
	int t;
	// begin부터의 토큰이 <sentence> 규칙에 맞는지 확인
	if((t = sentence(begin, end)) >= 0)	//<sentence> 규칙에 맞음
	{
		i = t;
	}
	else //<sentence> 규칙에 맞지 않음
	{
		return -1;
	}

	//그리고 <sentence> 이후 eof 토큰이 존재하는지 확인
	if (begin[i].type == tokenType::eof)
	{
		i++;
	}
	cout << "EXIT <s>" << endl;
	return i;
}

//<sentence> -> <if>
int sentence(Tokit begin, Tokit end)
{
	cout << "ENTER <sentence>" << endl;
	int i = 0;	//사용한 토큰 수
	int t;
	// begin 부터의 토큰이 <if> 규칙에 맞는지 확인
	if ((t = _if(begin, end)) >= 0)
	{
		i = t;
	}
	else
	{
		return -1;
	}
	cout << "EXIT <sentence>" << endl;
	return i;
}

/*
<if> -> if "(" <cmp_expr> ")" <block>
(else if ( <cmp_expr> ) <block>)*
(else <block>)?
*/
int _if(Tokit begin, Tokit end)
{
	cout << "ENTER <if>" << endl;
	int i = 0;	//사용한 토큰 수
	int t;

	// 처음 시작 키워드 if가 나오는지 확인
	if (begin[i].type == tokenType::_if)
	{
		i++;
	}
	else //if가 나오지 않는다면 에러
	{
		Error.push_back(Err(begin[i], "if 키워드가 없습니다"));
		return -1;
	}

	// 왼쪽 괄호가 나오는지 확인
	if (begin[i].type == tokenType::lparen)
	{
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "여는 소괄호가 없습니다"));
		return -1;
	}

	// <cmp_expr> 이 나오는지 확인
	if ((t = cmp_expr(begin + i, end)) >= 0)
	{
		i+=t;
	}
	else
	{
		return -1;
	}

	// 오른쪽 괄호가 나오는지 확인
	if (begin[i].type == tokenType::rparen)
	{
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "닫는 소괄호가 없습니다"));
		return -1;
	}

	// <block> 이 나오는지 확인
	if ((t = block(begin + i, end)) >= 0)
	{
		i += t;
	}
	else
	{
		return -1;
	}
	/* 
	일단 여기까지 왔으면 성공, 뒤에 else if나 else의 등장은 선택사항임
	따라서 else가 발견되지 않았다 하더라도 실패라 간주하지 않음(return -1 없음)
	*/
	if (begin[i].type == tokenType::_else)
	{
		i++;
		// 그 다음에 if가 등장하면 <if> 인지 확인 (else if)
		if (begin[i].type == tokenType::_if)
		{
			if ((t = _if(begin + i, end)) >= 0)
			{
				i += t;
			}
			else
			{
				return -1;
			}
		}
		else // <block>과 일치 확인 (else <block>)	
		{
			if ((t = block(begin + i, end)) >= 0)
			{
				i += t;
			}
			else
			{
				return -1;
			}
		}
	}
	cout << "EXIT <if>" << endl;
	return i;
}

int block(Tokit begin, Tokit end)
{
	cout << "ENTER <block>" << endl;
	int i = 0;	//사용한 토큰 수
	int t;

	// { 이 나오는지 확인
	if (begin[i].type == tokenType::lbracket)
	{
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "여는 중괄호가 없습니다"));
		return -1;
	}

	// } 이 나오는지 확인
	if (begin[i].type == tokenType::rbracket)
	{
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "닫는 중괄호가 없습니다"));
		return -1;
	}
	cout << "EXIT <block>" << endl;
	return i;
}

int expr(Tokit begin, Tokit end)
{
	cout << "ENTER <expr>" << endl;
	int i = 0;	//사용한 토큰 수
	int t;

	// <term>과 일치 시도
	if ((t = term(begin + i, end)) >= 0)
	{
		i += t;
	}
	else
	{
		return -1;
	}

	//여기까지만 와도 OK, 아래는 역시 선택사항
	// + 또는 -와 일치 시도
	if (begin[i].type == tokenType::plus || begin[i].type == tokenType::minus)
	{
		i++;
		// <term> 과 일치 시도
		if ((t = term(begin + i, end)) >= 0)
		{
			i += t;
		}
		else
		{
			return -1;
		}
	}
	cout << "EXIT <expr>" << endl;
	return i;

}

int term(Tokit begin, Tokit end)
{
	cout << "ENTER <term>" << endl;
	int i = 0;	//사용한 토큰 수
	int t;

	// <factor>와 일치 시도
	if ((t = factor(begin + i, end)) >= 0)
	{
		i += t;
	}
	else
	{
		return -1;
	}

	//여기까지만 와도 OK, 아래는 역시 선택사항
	// * 또는 /와 일치 시도
	if (begin[i].type == tokenType::asterisk || begin[i].type == tokenType::divide)
	{
		i++;
		// <factor>와 일치 시도
		if ((t = factor(begin + i, end)) >= 0)
		{
			i += t;
		}
	}
	cout << "EXIT <term>" << endl;
	return i;
}

int factor(Tokit begin, Tokit end)
{
	cout << "ENTER <factor>" << endl;
	int i = 0;

	//정수 또는 식별자와 일치 시도
	if ((begin[i].type == tokenType::integer) || (begin[i].type == tokenType::identifier))
	{
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "정수 또는 식별자가 위치해야 합니다"));
		return -1;
	}
	cout << "EXIT <factor>" << endl;
	return i;
}
int cmp_op(Tokit begin, Tokit end)
{
	cout << "ENTER <cmp_op>" << endl;
	int i = 0;

	// ==, !=, >, >=, <, <= 와 일치 시도
	if ((begin[i].type == tokenType::equal) || (begin[i].type == tokenType::notequal)
		|| (begin[i].type == tokenType::greater) || (begin[i].type == tokenType::greaterequal)
		|| (begin[i].type == tokenType::less) || (begin[i].type == tokenType::lessequal))
	{
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "여는 소괄호가 없습니다"));
		return -1;
	}
	cout << "EXIT <cmp_op>" << endl;
	return i;
}

int cmp_expr(Tokit begin, Tokit end)
{
	cout << "ENTER: <cmp_expr>" << endl;
	int i = 0;
	int t;

	// <expr>과 일치 시도
	if ((t = expr(begin + i, end)) >= 0)
	{
		i += t;
	}
	else
	{
		return -1;
	}

	// <cpm_op>와 일치 시도
	if ((t = cmp_op(begin + i, end)) >= 0)
	{
		i += t;
	}
	else
	{
		return -1;
	}

	// <expr>과 일치 시도
	if ((t = expr(begin + i, end)) >= 0)
	{
		i += t;
	}
	else
	{
		return -1;
	}
	cout << "EXIT: <cmp_expr>" << endl;
	return i;
}