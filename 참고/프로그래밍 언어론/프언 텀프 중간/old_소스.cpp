#include <iostream>
#include <string>
#include <vector>
#include <fstream>
using namespace std;

enum class tokenType //��ū Ÿ���� ���� Ŭ������ ����
{
	none,
	eof,		// ���ڿ��� ��
	error,		// ������ ��ū(����)
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
	integer,	// ����
	identifier	// �ĺ���
};

struct token	//��ū�� ���� ������ ������ ����ü
{
	string str;		//��ū�� ���� (ex. >, >=, +  ���)
	tokenType type;	//��ū�� Ÿ�� (ex. tokenType::less, tokenType::lessequal, tokenType::plus ���)
	int line;		//��ū�� �߰ߵ� ���� ��
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
//token ������ ��� �ݺ��ڰ� ���� ���̱⿡ Tokit���� typedef�� �̿��Ͽ� Ÿ�����μ� ����
typedef vector<token>::const_iterator Tokit;	

// ��ū�� ������ �߷����ϴ� ������ ��쿡�� true�� ��ȯ�ϴ� �Լ�
bool IsCutCharacter(wchar_t c);

//���ڿ��� �Է¹޾Ƽ� token�� vector�� ��ȯ�ϴ� �Լ�(���ֺм���)
vector<token> lex(const string& str);


/*#######################################################################
�Ʒ� ��� �Լ����� �����ϸ� ��ġ�� ��ū�� ���� ��ȯ, �����ϸ� ������ ��ȯ
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

vector<token> Token;	//���ֺм���(lexr)�� ����� ������ ���� ����
vector<Err> Error;		//������ ������ ���� ����
void main()
{
	ifstream in("source.txt");	//���α׷� �ҽ��ڵ� ����

	//���α׷� �ҽ��ڵ� ���� �б�
	string ss = "";
	char ch;
	while (in.get(ch))
	{
		ss += ch;
	}
	in.close();		//���� �ݱ�

	//���ֺм��� ����
	Token = lex(ss);

	//���ֺм� ��� ���(DEBUGING)
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
	//���۱�ȣ <s> �Ľ�
	int t=s(Token.begin(), Token.end());
	
	if (t == Token.size())	//�����Լ� s�� ��ȯ�� ���� ��ū�� ���� ��ġ�Ѵٸ� ����
	{
		cout << "���������� �Ľ̵Ǿ����ϴ�." << endl;
	}
	/*�����Լ� s�� ��ȯ�� ���� ��ū�� ���� ��ġ���� �ʴ´ٸ�
	��򰡿��� ������ �߻��� ���̹Ƿ� Err Ŭ������ ��� ���� Error�� ���� ���*/
	else 
	{
		for (int i = 0; i < Error.size(); i++)
		{
			cout << Error[i].t.line << "��° ��, " << Error[i].t.str << " ���� ������ ���� �߻�" << endl;
			cout << Error[i].msg << endl;
		}
	}
}

vector<token> lex(const string& str)
{
	vector<token> ret;

	auto it = str.begin();
	auto marker = str.begin();

	/* � Ÿ���� ��ū�� �۾��ϴ����� �����ϴ� ����.
	ó������ tokenType::none ��������.*/
	tokenType stage = tokenType::none;
	int line = 1;	// �ٹ�ȣ
	string s;		// �ӽ� ���ڿ�

	// it�� ���� �ٴٸ������� �ݺ�
	for (; it != str.end(); it++)
	{
		switch (stage)
		{
		// none �������� �� ���
		case tokenType::none:
			switch (*it)
			{
			case '\n':	// �ٹٲ� ���ڿ��� ���� line�� �÷���
				line++;
			case ' ':	// ������̹Ƿ� ������ ����
			case '\t':
			case '\r':
				break;
			case '0':	// ������ ���� ���������� integer�� �����ϰ� marker�� ���� �������� ����
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
			case '<':	// < ������ ��� < �̰ų� <= �� ��찡 �ֱ� ������ ���� ���ڱ��� ����
				if (it + 1 != str.end() && it[1] == '=')	// <= �� ���
				{
					ret.push_back(token("<=", tokenType::lessequal, line));
					it++;
				}
				else		// < �� ���
				{
					ret.push_back(token("<", tokenType::less, line));
				}
				break;
			case '>':	// > ������ ��� > �̰ų� >= �� ��찡 �ֱ� ������ ���� ���ڱ��� ����
				if (it + 1 != str.end() && it[1] == '=')	// >= �� ���
				{
					ret.push_back(token(">=", tokenType::greaterequal, line));
					it++;
				}
				else		// > �� ���
				{
					ret.push_back(token(">", tokenType::greater, line));
				}
				break;
			case '!':	// ! ������ ��� != �� ���ۿ� �����Ƿ� ���� ���� ����
				if (it + 1 != str.end() && it[1] == '=')	// != �� ���
				{
					ret.push_back(token("!=", tokenType::notequal, line));
					it++;
				}
				else // �� ���ڰ� = �� �ƴϸ� ������ ���� (ex. !a, !3 ��
				{
					ret.push_back(token("!", tokenType::error, line));
				}
				break;
			case '=':	// = ������ ��� = �̰ų� == �� ��찡 �ֱ� ������ ���� ���ڱ��� ����
				if (it + 1 != str.end() && it[1] == '=')	// ==�� ���
				{
					ret.push_back(token("==", tokenType::equal, line));
					it++;
				}
				else	//=�� ���
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
			default:	// �� �̿��� ��쿣 �� �ĺ��ڷ� ����ϱ� ������ marker�� ���������� �������ϴ�
				stage = tokenType::identifier;
				marker = it;
				break;
			}
			break;
			// ���������� integer�� ��� (���ڷ� ������ ���)
		case tokenType::integer:
			// ��ū�� �ڸ��� ���ڸ� �������� ���� ���� ������ ���ڷ� �Ǵ�
			// ���������� �⺻���� ������, �� ���� ������ ��ū�� ����
			if (IsCutCharacter(*it))
			{
				stage = tokenType::none;
				ret.push_back(token(string(marker, it--), tokenType::integer, line));
			}
			// ���ڰ� �ƴ� ���� �ڿ� �پ������� ����
			else if (!isdigit(*it))
			{
				stage = tokenType::none;
				ret.push_back(token(string(marker, it), tokenType::error, line));
				--it;
			}
			break;
			// ���������� �ĺ����� ��� (��Ÿ �Ϲ����� ���ڷ� �����Ѱ��, Ư������X)
		case tokenType::identifier:
			// ��ū�� �ڸ��� ���ڸ� ��������, ������� ���ڿ��� �߶� Ű����� ��
			if (IsCutCharacter(*it))
			{
				stage = tokenType::none;
				string s(marker, it--);
				if (s == "if")
					ret.push_back(token(s, tokenType::_if, line));
				else if (s == "else")
					ret.push_back(token(s, tokenType::_else, line));
				// �ƹ� Ű���忡 �ش�ȵǸ� �׳� �Ϲ� �ĺ��ڷ� ���
				else
				{
					ret.push_back(token(s, tokenType::identifier, line));
				}
			}
			break;
		}
	}
	// �� ���� �� �ڿ� ���� �������� eof�� ����
	ret.push_back(token("", tokenType::eof, line));
	return ret;
}

// ��ū�� ������ �߷����ϴ� ������ ��쿡�� true�� ��ȯ�ϴ� �Լ�
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
	int i = 0;	//����� ��ū ��
	int t;
	// begin������ ��ū�� <sentence> ��Ģ�� �´��� Ȯ��
	if((t = sentence(begin, end)) >= 0)	//<sentence> ��Ģ�� ����
	{
		i = t;
	}
	else //<sentence> ��Ģ�� ���� ����
	{
		return -1;
	}

	//�׸��� <sentence> ���� eof ��ū�� �����ϴ��� Ȯ��
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
	int i = 0;	//����� ��ū ��
	int t;
	// begin ������ ��ū�� <if> ��Ģ�� �´��� Ȯ��
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
	int i = 0;	//����� ��ū ��
	int t;

	// ó�� ���� Ű���� if�� �������� Ȯ��
	if (begin[i].type == tokenType::_if)
	{
		i++;
	}
	else //if�� ������ �ʴ´ٸ� ����
	{
		Error.push_back(Err(begin[i], "if Ű���尡 �����ϴ�"));
		return -1;
	}

	// ���� ��ȣ�� �������� Ȯ��
	if (begin[i].type == tokenType::lparen)
	{
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "���� �Ұ�ȣ�� �����ϴ�"));
		return -1;
	}

	// <cmp_expr> �� �������� Ȯ��
	if ((t = cmp_expr(begin + i, end)) >= 0)
	{
		i+=t;
	}
	else
	{
		return -1;
	}

	// ������ ��ȣ�� �������� Ȯ��
	if (begin[i].type == tokenType::rparen)
	{
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "�ݴ� �Ұ�ȣ�� �����ϴ�"));
		return -1;
	}

	// <block> �� �������� Ȯ��
	if ((t = block(begin + i, end)) >= 0)
	{
		i += t;
	}
	else
	{
		return -1;
	}
	/* 
	�ϴ� ������� ������ ����, �ڿ� else if�� else�� ������ ���û�����
	���� else�� �߰ߵ��� �ʾҴ� �ϴ��� ���ж� �������� ����(return -1 ����)
	*/
	if (begin[i].type == tokenType::_else)
	{
		i++;
		// �� ������ if�� �����ϸ� <if> ���� Ȯ�� (else if)
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
		else // <block>�� ��ġ Ȯ�� (else <block>)	
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
	int i = 0;	//����� ��ū ��
	int t;

	// { �� �������� Ȯ��
	if (begin[i].type == tokenType::lbracket)
	{
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "���� �߰�ȣ�� �����ϴ�"));
		return -1;
	}

	// } �� �������� Ȯ��
	if (begin[i].type == tokenType::rbracket)
	{
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "�ݴ� �߰�ȣ�� �����ϴ�"));
		return -1;
	}
	cout << "EXIT <block>" << endl;
	return i;
}

int expr(Tokit begin, Tokit end)
{
	cout << "ENTER <expr>" << endl;
	int i = 0;	//����� ��ū ��
	int t;

	// <term>�� ��ġ �õ�
	if ((t = term(begin + i, end)) >= 0)
	{
		i += t;
	}
	else
	{
		return -1;
	}

	//��������� �͵� OK, �Ʒ��� ���� ���û���
	// + �Ǵ� -�� ��ġ �õ�
	if (begin[i].type == tokenType::plus || begin[i].type == tokenType::minus)
	{
		i++;
		// <term> �� ��ġ �õ�
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
	int i = 0;	//����� ��ū ��
	int t;

	// <factor>�� ��ġ �õ�
	if ((t = factor(begin + i, end)) >= 0)
	{
		i += t;
	}
	else
	{
		return -1;
	}

	//��������� �͵� OK, �Ʒ��� ���� ���û���
	// * �Ǵ� /�� ��ġ �õ�
	if (begin[i].type == tokenType::asterisk || begin[i].type == tokenType::divide)
	{
		i++;
		// <factor>�� ��ġ �õ�
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

	//���� �Ǵ� �ĺ��ڿ� ��ġ �õ�
	if ((begin[i].type == tokenType::integer) || (begin[i].type == tokenType::identifier))
	{
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "���� �Ǵ� �ĺ��ڰ� ��ġ�ؾ� �մϴ�"));
		return -1;
	}
	cout << "EXIT <factor>" << endl;
	return i;
}
int cmp_op(Tokit begin, Tokit end)
{
	cout << "ENTER <cmp_op>" << endl;
	int i = 0;

	// ==, !=, >, >=, <, <= �� ��ġ �õ�
	if ((begin[i].type == tokenType::equal) || (begin[i].type == tokenType::notequal)
		|| (begin[i].type == tokenType::greater) || (begin[i].type == tokenType::greaterequal)
		|| (begin[i].type == tokenType::less) || (begin[i].type == tokenType::lessequal))
	{
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "���� �Ұ�ȣ�� �����ϴ�"));
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

	// <expr>�� ��ġ �õ�
	if ((t = expr(begin + i, end)) >= 0)
	{
		i += t;
	}
	else
	{
		return -1;
	}

	// <cpm_op>�� ��ġ �õ�
	if ((t = cmp_op(begin + i, end)) >= 0)
	{
		i += t;
	}
	else
	{
		return -1;
	}

	// <expr>�� ��ġ �õ�
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