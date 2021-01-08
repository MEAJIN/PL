#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
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

struct Message
{
	int useToken;
	string message;
	tokenType type;
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

//<sentence> -> <if> | <assign>
int sentence(Tokit begin, Tokit end);

//<assign> -> id "=" int_constant
int assign(Tokit begin, Tokit end);

/*
<if> -> "if" "(" <cmp_expr> ")" <block>
{ "else" "if" "(" <cmp_expr> ")" <block> }
[ "else" <block> ]
*/
int _if(Tokit begin, Tokit end);

//<block> -> "{" <sentence> "}"
int block(Tokit begin, Tokit end);

//<expr> -> <term> {(+|-) <term>}
Message expr(Tokit begin, Tokit end);

//<term> -> <factor> {(*|/) <factor>}
Message term(Tokit begin, Tokit end);

//<factor> -> id | int_constant
Message factor(Tokit begin, Tokit end);

//<cmp_op> -> "==" | "!=" | ">" | "<" | ">=" | "<="
Message cmp_op(Tokit begin, Tokit end);

//<cmp_expr> -> <expr> <cmp_op> <expr>
Message cmp_expr(Tokit begin, Tokit end);

vector<token> Token;	//���ֺм���(lexr)�� ����� ������ ���� ����
vector<Err> Error;		//������ ������ ���� ����
map<string, int> variable;	//���� �����ϴ� ��
void main()
{
	ifstream in("source.txt");	//���α׷� �ҽ��ڵ� ����

								//���α׷� �ҽ��ڵ� ���� �б�
	string ss;
	/*char ch;
	while (in.get(ch))
	{
	ss += ch;
	}*/
	while (getline(in, ss))
	{
		//cout << ss << endl;
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
		int t = s(Token.begin(), Token.end());

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
		cout << "===== ���� ���̺� =====" << endl;
		map<string, int>::iterator iter;
		for (iter = variable.begin(); iter != variable.end(); iter++)
		{
			cout << "memory[" << (*iter).first << "]: " << (*iter).second << endl;
		}
		cout << "======================" << endl;
	}

	in.close();		//���� �ݱ�
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
	if ((t = sentence(begin, end)) >= 0)	//<sentence> ��Ģ�� ����
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

//<sentence> -> <if> | <assign>
int sentence(Tokit begin, Tokit end)
{
	cout << "ENTER <sentence>" << endl;
	int i = 0;	//����� ��ū ��
	int t;
	cout << begin[i].str << endl;

	if (begin[i].type == tokenType::_if)
	{
		// begin ������ ��ū�� <if> ��Ģ�� �´��� Ȯ��
		if ((t = _if(begin, end)) >= 0)
		{
			i = t;
		}
		else
		{
			return -1;
		}
	}
	else if (begin[i].type == tokenType::identifier)
	{
		if ((t = assign(begin, end)) >= 0)
		{
			i = t;
		}
		else
		{
			return -1;
		}
	}

	cout << "EXIT <sentence>" << endl;
	return i;
}

int assign(Tokit begin, Tokit end)
{
	cout << "ENTER <assign>" << endl;
	int i = 0;
	string var;
	int value;
	//�ĺ��ڿ� ��ġ �õ�
	if ((begin[i].type == tokenType::identifier))
	{
		var = begin[i].str;
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "�ĺ��ڰ� ��ġ�ؾ� �մϴ�"));
		return -1;
	}

	// �Ҵ� �����ڿ� ��ġ �õ�
	if ((begin[i].type == tokenType::assign))
	{
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "�Ҵ� ��ȣ�� ������ �ʾҽ��ϴ�"));
	}

	//������ ��ġ �õ�
	if ((begin[i].type == tokenType::integer))
	{
		value = atoi(begin[i].str.c_str());	//���ڿ��� ���ڷ� �����ؼ� ����
		i++;
	}
	else
	{
		Error.push_back(Err(begin[i], "������ ��ġ�ؾ� �մϴ�"));
		return -1;
	}

	//��� ���������� ���� ��쿣 �Ҵ�� ������ �� ���� �ʿ� ����
	variable[var] = value;

	cout << "EXIT <assign>" << endl;
	return i;
}

/*
<if> -> if "(" <cmp_expr> ")" <block>
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
		cout << "if ������ ����" << endl;
		exit(0);
	}


	// ���� ��ȣ�� �������� Ȯ��
	if (begin[i].type == tokenType::lparen)
	{
		i++;
	}
	else
	{
		cout << "���� �Ұ�ȣ�� �����ϴ�" << endl;
		exit(0);
	}

	// <cmp_expr> �� �������� Ȯ��
	Message m = cmp_expr(begin + i, end);
	i += m.useToken;

	// ������ ��ȣ�� �������� Ȯ��
	if (begin[i].type == tokenType::rparen)
	{
		i++;
	}
	else
	{
		cout << "�ݴ� �Ұ�ȣ�� �����ϴ�" << endl;
		exit(0);
	}

	if (m.message == "true")	//if�� ������ ���� ��쿡�� ���� block ����
	{
		// <block> �� �������� Ȯ��
		if ((t = block(begin + i, end)) >= 0)
		{
			i += t;
		}
		else
		{
			cout << "����� ������ �ʾҽ��ϴ�" << endl;
			exit(0);
		}
	}
	else		//if�� ������ ���̶�� block�� �˻����� �ʰ� ��ū�� block �������� �ѱ�
	{
		while (true)
		{
			if (begin[i].type == tokenType::rbracket)	//�ݴ� �߰�ȣ�� �����Ÿ� block�� ���� ��
			{
				break;
			}
			i++;
		}
		i++;
	}
	
	if (m.message == "false")	//else�� ��� if�� cmp_expr�� false�� ���� ������
	{
		/*
		�ϴ� ������� ������ ����, �ڿ� else if�� else�� ������ ���û�����
		���� else�� �߰ߵ��� �ʾҴ� �ϴ��� ���ж� �������� ����(return -1 ����)
		*/
		if (begin[i].type == tokenType::_else)
		{
			i++;

			// <block>�� ��ġ Ȯ�� (else <block>)	

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

	//sentence�� �������� Ȯ��
	if (t = sentence(begin + i, end))
	{
		i += t;
	}
	else
	{
		Error.push_back(Err(begin[i], "������ ������ ����"));
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

Message expr(Tokit begin, Tokit end)
{
	cout << "ENTER <expr>" << endl;
	int i = 0;	//����� ��ū ��
	int t;

	Message m;
	// <term>�� ��ġ �õ�
	m = term(begin + i, end);
	t = m.useToken;
	i += t;
	
	//��������� �͵� OK, �Ʒ��� ���� ���û���
	// + �Ǵ� -�� ��ġ �õ�
	if (begin[i].type == tokenType::plus || begin[i].type == tokenType::minus)
	{
		Message temp;
		string oper;
		if (begin[i].type == tokenType::plus)
		{
			oper = "plus";
		}
		else
		{
			oper = "minus";
		}
		i++;

		// <term> �� ��ġ �õ�
		temp = term(begin + i, end);
		t = temp.useToken;
		i += t;

		//���� ���� �߰�//
		if (oper=="plus")	//������ ���
		{
			if ((m.type == tokenType::integer) && (temp.type == tokenType::integer))	//���� + ����
			{
				m.message = to_string(atoi(m.message.c_str()) + atoi(temp.message.c_str()));
				m.type = tokenType::integer;
			}
			else if ((m.type == tokenType::identifier) && (temp.type == tokenType::integer))	//���� + ����
			{
				if (variable.find(m.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << m.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}

				m.message = to_string(variable[m.message] + atoi(temp.message.c_str()));
				m.type = tokenType::integer;
			}
			else if ((m.type == tokenType::integer) && (temp.type == tokenType::identifier))	//���� + ����
			{
				if (variable.find(temp.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << m.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}
				m.message = to_string(atoi(m.message.c_str()) + variable[temp.message]);
				m.type = tokenType::integer;
			}
			else if ((m.type == tokenType::identifier) && (temp.type == tokenType::identifier))	//���� + ����
			{
				if (variable.find(m.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << m.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}
				if (variable.find(temp.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << m.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}
				m.message = to_string(variable[m.message] + variable[temp.message]);
				m.type = tokenType::integer;
			}
			else
			{
				cout << "term()" << endl;
				exit(0);
			}
		}
		else //������ ���
		{
			if ((m.type == tokenType::integer) && (temp.type == tokenType::integer))	//���� - ����
			{
				m.message = to_string(atoi(m.message.c_str()) - atoi(temp.message.c_str()));
				m.type = tokenType::integer;
			}
			else if ((m.type == tokenType::identifier) && (temp.type == tokenType::integer))	//���� - ����
			{
				if (variable.find(m.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << m.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}

				m.message = to_string(variable[m.message] - atoi(temp.message.c_str()));
				m.type = tokenType::integer;
			}
			else if ((m.type == tokenType::integer) && (temp.type == tokenType::identifier))	//���� - ����
			{
				if (variable.find(temp.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << temp.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}

				m.message = to_string(atoi(m.message.c_str()) - variable[temp.message]);
				m.type = tokenType::integer;
			}
			else if ((m.type == tokenType::identifier) && (temp.type == tokenType::identifier))	//���� - ����
			{
				if (variable.find(m.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << m.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}
				if (variable.find(temp.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << m.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);

				}
				m.message = to_string(variable[m.message] - variable[temp.message]);
				m.type = tokenType::integer;
			}
			else
			{
				cout << "term()" << endl;
				exit(0);
			}
		}
		m.useToken = i;
	}
	cout << "EXIT <expr>" << endl;
	return m;

}

Message term(Tokit begin, Tokit end)
{
	cout << "ENTER <term>" << endl;
	int i = 0;	//����� ��ū ��
	int t;
	Message m;
	// <factor>�� ��ġ �õ�
	
	m = factor(begin +i, end);
	i += m.useToken;

	//��������� �͵� OK, �Ʒ��� ���� ���û���
	// * �Ǵ� /�� ��ġ �õ�
	if (begin[i].type == tokenType::asterisk || begin[i].type == tokenType::divide)
	{
		i++;
		// <factor>�� ��ġ �õ�
		Message t = factor(begin + i, end);
		i += t.useToken;
		if (begin[i-1-t.useToken].type == tokenType::asterisk)	//������ ���
		{
			if ((m.type == tokenType::integer) && (t.type == tokenType::integer))	//���� * ����
			{
				m.message = to_string(atoi(m.message.c_str()) * atoi(t.message.c_str()));
				m.type = tokenType::integer;
			}
			else if ((m.type == tokenType::identifier) && (t.type == tokenType::integer))	//���� * ����
			{
				//������ ��� ���� �ݵ�� ����Ǿ�� ��
				if (variable.find(m.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << m.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}
				m.message = to_string(variable[m.message] * atoi(t.message.c_str()));
				m.type = tokenType::integer;
			}
			else if ((m.type == tokenType::integer ) && (t.type == tokenType::identifier))	//���� * ����
			{
				//������ ��� ���� �ݵ�� ����Ǿ�� ��
				if (variable.find(t.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << t.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}
				m.message = to_string(atoi(m.message.c_str()) * variable[t.message]);
				m.type = tokenType::integer;
			}
			else if ((m.type == tokenType::identifier) && (t.type == tokenType::identifier))	//���� * ����
			{
				//������ ��� ���� �ݵ�� ����Ǿ�� ��
				if (variable.find(m.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << m.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}
				//������ ��� ���� �ݵ�� ����Ǿ�� ��
				if (variable.find(t.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << t.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}

				m.message = to_string(variable[m.message] * variable[t.message]);
				m.type = tokenType::integer;
			}
			else
			{
				cout << "term()" << endl;
				exit(0);
			}
		}
		else //�������� ���
		{
			if ((m.type == tokenType::integer) && (t.type == tokenType::integer))	//���� * ����
			{
				m.message = to_string(atoi(m.message.c_str()) / atoi(t.message.c_str()));
				m.type = tokenType::integer;
			}
			else if ((m.type == tokenType::identifier) && (t.type == tokenType::integer))	//���� * ����
			{
				//������ ��� ���� �ݵ�� ����Ǿ�� ��
				if (variable.find(m.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << m.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}
				m.message = to_string(variable[m.message] / atoi(t.message.c_str()));
				m.type = tokenType::integer;
			}
			else if ((m.type == tokenType::integer) && (t.type == tokenType::identifier))	//���� * ����
			{
				//������ ��� ���� �ݵ�� ����Ǿ�� ��
				if (variable.find(t.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << t.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}
				m.message = to_string(atoi(m.message.c_str()) / variable[t.message]);
				m.type = tokenType::integer;
			}
			else if ((m.type == tokenType::identifier) && (t.type == tokenType::identifier))	//���� * ����
			{
				//������ ��� ���� �ݵ�� ����Ǿ�� ��
				if (variable.find(m.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << m.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}
				if (variable.find(t.message) == variable.end())	//������ ������� �ʾ����� ���� �Ұ���
				{
					cout << "������� ���� ���� " << t.message << " �� ���Ǿ����ϴ�" << endl;
					exit(0);
				}
				m.message = to_string(variable[m.message] / variable[t.message]);
				m.type = tokenType::integer;
			}
			else
			{
				cout << "term()" << endl;
				exit(0);
			}
		}
		
	}
	
	m.useToken = i;
	cout << "EXIT <term>" << endl;
	return m;
}

Message factor(Tokit begin, Tokit end)
{
	cout << "ENTER <factor>" << endl;
	int i = 0;

	Message m;
	//���� �Ǵ� �ĺ��ڿ� ��ġ �õ�
	if ((begin[i].type == tokenType::integer) || (begin[i].type == tokenType::identifier))
	{
		
		m.message = begin[i].str;
		if (begin[i].type == tokenType::integer)
		{
			m.type = tokenType::integer;
		}
		else
		{
			m.type = tokenType::identifier;
		}
		i++;
	}
	else
	{
		cout << "factor) ���� �Ǵ� �ĺ��ڰ� ��ġ�ؾ� �մϴ�" << endl;
		exit(0);
		//return -1;
	}
	m.useToken = i;
	cout << "EXIT <factor>" << endl;
	return m;
}
Message cmp_op(Tokit begin, Tokit end)
{
	cout << "ENTER <cmp_op>" << endl;
	int i = 0;

	Message m;
	// ==, !=, >, >=, <, <= �� ��ġ �õ�
	if ((begin[i].type == tokenType::equal) || (begin[i].type == tokenType::notequal)
		|| (begin[i].type == tokenType::greater) || (begin[i].type == tokenType::greaterequal)
		|| (begin[i].type == tokenType::less) || (begin[i].type == tokenType::lessequal))
	{	
		m.type = begin[i].type;
		i++;
		m.useToken = i;
	}
	else
	{
		exit(0);
	}
	cout << "EXIT <cmp_op>" << endl;
	return m;
}

Message cmp_expr(Tokit begin, Tokit end)
{
	cout << "ENTER: <cmp_expr>" << endl;
	int i = 0;
	bool result;
	Message m, t, op, res;
	// <expr>�� ��ġ �õ�
	m = expr(begin, end);
	//m�� �������ٸ� �� ���� �ٷ� mValue��, m�� �������ٸ� ���� ���̺��� ã�Ƽ� mValue�� ����
	//��, ������ ������� �ʰ� ���Ǿ����� �Ұ���
	if ((m.type==tokenType::identifier)&&(variable.find(m.message) == variable.end()))	//������ ������� �ʾ����� ���� �Ұ���
	{
		cout << "������� ���� ���� " << m.message << " �� ���Ǿ����ϴ�" << endl;
		exit(0);
	}
	int mValue = (m.type == tokenType::integer) ? atoi(m.message.c_str()) : variable[m.message];

	i += m.useToken;
	
	// <cpm_op>�� ��ġ �õ�
	op = cmp_op(begin + i, end);
	i += op.useToken;

	// <expr>�� ��ġ �õ�
	t = expr(begin + i, end);
	int tValue = (t.type == tokenType::integer) ? atoi(t.message.c_str()) : variable[t.message];

	i += t.useToken;

	switch (op.type)
	{
	case tokenType::equal:		// ==�������� ���
		result = (mValue == tValue);
		break;
	case tokenType::notequal:	// !=�������� ���
		result = (mValue != tValue);
		break;
	case tokenType::greater:	// >�������� ���
		result = (mValue > tValue);
		break;
	case tokenType::greaterequal:	// >= �������� ���
		result = (mValue >= tValue);
		break;
	case tokenType::less:		// <�������� ���
		result = (mValue < tValue);
		break;
	case tokenType::lessequal:	// <=�������� ���
		result = (mValue <= tValue);
		break;
	}

	res.useToken = i;
	res.message = (result == true) ? "true" : "false";

	cout << "CMP_EXPR�� " << result << " �� ����" << endl;
	cout << "EXIT: <cmp_expr>" << endl;
	return res;
}