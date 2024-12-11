#include <string>
#include <stack>
namespace str {
	class Parser {
	private:
		const int numberOfFunctions = 10;
		std::string functions[20] = { "ln","sqrt","cos","sin","tg",
			"ctg","arcsin","arccos","arctg","arcctg" };
	public:
		

		bool isOperator(char x)
		{
			return (x == '+') || (x == '-') || (x == '*') || (x == '/') || (x == '^');
		}
		bool isLit(char x)
		{
			return (x >= 'a' && x <= 'z') || (x >= 'A' && x <= 'Z');
		}
		bool overAllValid(char x)
		{
			return isLit(x) || isOperator(x) || x == '(' || x == ')';
		}
		std::string formulaParsedString(std::string s)
		{
			std::string formula;
			for (int i = 0;i < s.size();i++)
			{
				if (s[i] != ' ')
				{
					if (s[i] == '*') {
						formula.push_back(' ');
						formula.push_back(char(183));
						formula.push_back(' ');
					}
					else if (isOperator(s[i]))
					{
						formula.push_back(' ');
						formula.push_back(s[i]);
						formula.push_back(' ');
					}
					else
						formula.push_back(s[i]);
				}
			}
			return formula;
		}
		
		bool isStringValid(std::string s)
		{
			std::string newS;
			std::stack<char>st;
			for (int i = 0;i < s.size();i++)
				if (s[i] != ' ') newS.push_back(s[i]);
			int i = 0;
			if (isOperator(newS[0]) || newS[0]==')' || newS[0]==',' || newS[0] == ';' || newS[0]=='_')
				return false;
			if (newS[0] == '(')
				st.push('(');
			if (isOperator(newS[newS.size() - 1]))
				return false;
			if (isLit(newS[0]))
			{
				std::string form;
				while (isLit(newS[i]))
				{
					form.push_back(newS[i]);
					i++;
				}
				if (form.size() != 1)
				{
					bool ok = 0;
					for (int j = 0;j < numberOfFunctions;j++)
					{
						if (form == functions[j])
						{
							ok = 1;
							break;
						}
					}
					if (!ok)
						return false;
					if (i < newS.size() - 1)
					{
						if (newS[i] != '(')
							return false;
					}
					else
						return false;
				}
			}
			if (i < 1)
				i = 1;
			while (i < newS.size())
			{
				if (newS[i] == '_' || newS[i] == ';')return false;
				if (isOperator(newS[i]) && isOperator(newS[i - 1]))return false;
				if (isOperator(newS[i]) && newS[i - 1] == '(')return false;
				if (newS[i] == ',' && !isdigit(newS[i - 1]))return false;
				if (isLit(newS[i]))
				{
					if (!isOperator(newS[i - 1]) && newS[i-1]!='(')
						return false;
					std::string form;
					while (isLit(newS[i]))
					{
						form.push_back(newS[i]);
						i++;
					}
					if (form.size() != 1)
					{
						bool ok = 0;
						for (int j = 0;j < numberOfFunctions;j++)
						{
							if (form == functions[j])
							{
								ok = 1;
								break;
							}
						}
						if (!ok)
							return false;
						if (i < newS.size() - 1)
						{
							if (newS[i] != '(')
								return false;
						}
						else
							return false;
					}
				}
				if (newS[i] == '(') {
					if (!isOperator(newS[i - 1]) && !isLit(newS[i-1]))
					{
						return false;
					}
					st.push('(');
				}
				if (newS[i] == ')')
				{
					if (st.empty())
						return false;
					else
						st.pop();
				}
				i++;
			}
			if (!st.empty())
				return false;
		}
	};
}
