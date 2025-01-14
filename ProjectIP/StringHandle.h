#pragma once
#include <string>
#include <unordered_set>
#include <stack>
#include <regex>
class Parser {
	private:
		const int numberOfFunctions = 10;
		std::string functions[10] = { "ln","sqrt","cos","sin","tg",
			"ctg","arcsin","arccos","arctg","arcctg" };
	public:
		std::string checkForFunction(std::string str)
		{
			for (int i = 0;i < numberOfFunctions;i++)
			{
				if (functions[i].size() < str.size())
				{
					bool isValid = true;
					for (int j = 0;j < functions[i].size();j++)
					{
						if (functions[i][j] != str[j])
						{
							isValid = false;
							break;
						}
					}
					if (isValid) {
						std::stack<int>stk;
						for (int j = functions[i].size();j < str.size()-1;j++)
						{
							if (str[j] == '(')
								stk.push(j);
							if (str[j] == ')') {
								if (stk.empty()) isValid = false;
								else stk.pop();
							}
						}
						if (isValid&&!stk.empty() && stk.top() == functions[i].size())
							return functions[i];
					}
				}
			}
			return "none";
		}
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
			for (int i = 0;i <s.size();i++)
			{
				if (s[i] != ' ')
				{
					if (s[i] == '*')
						formula.push_back(char(183));
					else
						formula.push_back(s[i]);
				}
			}
			return formula;
		}
		
		bool isStringValid(std::string s)
		{
			
			for (auto x : s)
			{
				if (!(x == ')' || (x == '(') || isOperator(x) || isLit(x) || isdigit(x) || x == ',')) return false;
			}
			if (s.size() == 1 && !isLit(s[0]) && !isdigit(s[0])) return false;
			//Check if the paranthases are correct
			std::stack<int>stk;
			for (int i = 0;i < s.size();i++)
			{
				if (s[i] == '(')
					stk.push(i);
				if (s[i] == ')')
				{
					if (stk.empty()) return false;
					stk.pop();
				}
			}
			if (!stk.empty()) return false;
			//Check for correct ','
			int i = 0;
			int virgula = 0;
			while (i < s.size())
			{
				if (s[i] == ',')
					virgula++;
				else if (!isdigit(s[i])) {
					virgula = std::max(0, virgula - 1);
				}
				if (virgula > 1)
					return false;
				i++;
			}
			//Check wether the functions are correct
			i = 0;
			virgula = 0;
			while (i < s.size())
			{
				if (isLit(s[i]))
				{
					if (i - 1 >= 0)
					{
						if (isdigit(s[i - 1]) || s[i - 1] == ',' || s[i - 1] == ')') return false;
					}
					if (i + 1 < s.size())
					{
						if (isdigit(s[i + 1]) || s[i + 1] == ',') return false;
					}
				}
				if (isdigit(s[i]))
				{
					if (i - 1 >= 0)
					{
						if (isLit(s[i - 1])) return false;
						if (s[i - 1] == ')') return false;
					}
				}
				if (isLit(s[i]) && isLit(s[i + 1]))
				{
					bool ok = false;
					std::string function;
					int j = i;
					for (j = i;isLit(s[j]) && j < s.size();j++)
					{
						function.push_back(s[j]);
					}
					for (int k = 0; k < numberOfFunctions; k++)
					{
						if (functions[k] == function)
						{
							ok = true;
						}
					}
					if (!ok) return false;
					if (j - s.size() < 2) return false;
					i = j;
				}
				if (s[i] == '/' || s[i] == '*' || s[i] == '^')
				{
					if (i - 1 < 0) return false;
					if (i + 1 == s.size()) return false;
					if (!((isLit(s[i + 1]) || isdigit(s[i + 1]) || s[i + 1] == '(') && (isLit(s[i - 1]) || isdigit(s[i - 1]) || s[i - 1] == ')')))
						return false;
				}
				if (s[i] == '+' || s[i] == '-')
				{
					if (i-1>=0 && isOperator(s[i - 1]))return false;
					if (i + 1 == s.size()) return false;
					if (isOperator(s[i + 1])) return false;
				}
				if (s[i] == '0' && ((i>0 && !isdigit(s[i-1])||(i==0))) && i + 1 != s.size() && isdigit(s[i + 1]) && virgula == 0) return false;
				if (s[i] == ',' && i - 1 < 0) return false;
				if (s[i] == ',' && !isdigit(s[i - 1])) return false;
				if (s[i] == ')' && s[i - 1] == '(') return false;
				if (s[i] == ',') virgula++;
				else if(!isdigit(s[i])) virgula = std::max(0, virgula - 1);

			
				i++;
			}
			
			return true;
		}

};

