#include <iostream>
#include <string>
#include "SFML/Graphics.hpp"
#include <stack>
#include "StringHandle.h"
#include <locale>
sf::Font mathFont;
sf::RenderWindow window(sf::VideoMode(1200, 1200), "Vizualizator formule");

struct formulaTree {
	formulaTree* leftTree;
	formulaTree* rightTree;
	bool hasParentheses;
	std::string treeFormula;
	std::string function;
	char operation;
	float xPosOfFormula, yPosOfFormula;
	float xPosOfOperator, yPosOfOperator;
	float lengthOfDivision, xPosOfFormulaEnd;
	float minHeightParantheses, maxHeightParantheses;
	float xPosOfParentheses;
	formulaTree()
	{
		leftTree = NULL, rightTree = NULL;
		hasParentheses = 0;
		operation = ' ';
		xPosOfFormula = 0;
		yPosOfFormula = 0;
		xPosOfOperator = 0;
		yPosOfOperator = 0;
		lengthOfDivision = 0;
		xPosOfFormulaEnd = xPosOfParentheses= 0;
		minHeightParantheses = 0, maxHeightParantheses = 0;
	}
};

str::Parser stringParser;

formulaTree* buildFormulaTree(std::string formula)
{
	int plus_minus_index = -1;
	int plus_minus_priority = 1e9;

	int multiply_divide_index = -1;
	int multiply_divide_priority = 1e9;

	int power_index = -1;
	int power_priority = 1e9;

	int currentPriority = 0;
	int minimumGeneralPriority = 1e9;


	for (int i = 0; i <= static_cast<int>(formula.size()); i++)
	{
		if (formula[i] == '(') currentPriority++;
		if (formula[i] == ')') currentPriority--;
		if ((formula[i] == '+' || formula[i] == '-') && (plus_minus_priority > currentPriority))
		{
			plus_minus_priority = currentPriority;
			plus_minus_index = i;
		}
		if ((formula[i] == '*' || formula[i] == '/') && (multiply_divide_priority > currentPriority))
		{
			multiply_divide_priority = currentPriority;
			multiply_divide_index = i;
		}
		if ((formula[i] == '^') && (power_priority > currentPriority))
		{
			power_priority = currentPriority;
			power_index = i;
		}
		if (stringParser.isOperator(formula[i])) minimumGeneralPriority = std::min(minimumGeneralPriority, currentPriority);
	}

	int operatorIndex = -1;

	if (minimumGeneralPriority == plus_minus_priority) operatorIndex = plus_minus_index;
	else if (minimumGeneralPriority == multiply_divide_priority) operatorIndex = multiply_divide_index;
	else if (minimumGeneralPriority == power_priority) operatorIndex = power_index;

	

	formulaTree* node = new formulaTree();

	if (operatorIndex == -1) {
		node->treeFormula = formula;
		node->leftTree = NULL;
		node->rightTree = NULL;
		return node;
	}
	
	while (formula[0] == ' ')
	{
		formula = formula.substr(1);
		operatorIndex--;
	}
	while (formula.back() == ' ')
	{
		formula.pop_back();
	}

	if (formula[0]=='(' && formula[formula.size()-1]==')')
	{
		std::stack<int>par;
		int cnt = 0, ordin = 0;
		for (int i = 0;i < static_cast<int>(formula.size());i++)
		{
			if (formula[i] == '(')
				par.push(++ordin);
			if (formula[i] == ')')
			{
				if (par.top() == 1)
					cnt++;
				ordin--;
				par.pop();
			}
		}
		if (cnt == 1) {
			node->hasParentheses = 1;
			formula.pop_back();
			formula = formula.substr(1);
			operatorIndex--;
		}
	}
	
	node->treeFormula = formula;
	node->operation = formula[operatorIndex];
	
	node->leftTree = buildFormulaTree(formula.substr(0, operatorIndex));
	node->rightTree = buildFormulaTree(formula.substr(operatorIndex + 1));

	return node;
}

bool isLeaf(formulaTree* node) { return (node->leftTree == NULL && node->rightTree == NULL); }

sf::Text getDrawableFormula(std::string formula, float x, float y)
{
	sf::Text textFormula;
	textFormula.setCharacterSize(32);
	textFormula.setFont(mathFont);
	textFormula.setFillColor(sf::Color::White);
	textFormula.setString(formula);
	textFormula.setPosition(x,y);
	return textFormula;
}
sf::Text getDrawableParantheses(std::string formula, float x, float y, float fontSize)
{
	sf::Text textFormula;
	textFormula.setCharacterSize(fontSize);
	textFormula.setFont(mathFont);
	textFormula.setFillColor(sf::Color::White);
	textFormula.setString(formula);
	textFormula.setPosition(x, y);
	return textFormula;
}
sf::Text operatorToText(char op, float x, float y)
{
	std::string s;
	s.push_back(op);
	sf::Text txt;
	txt.setCharacterSize(28);
	txt.setFont(mathFont);
	txt.setFillColor(sf::Color::White);
	txt.setString(s);
	txt.setPosition(x, y);
	return txt;
}
float optimalFontSize(formulaTree* node)
{
	return float(std::max(float(28), float(28) + std::abs(node->minHeightParantheses - node->maxHeightParantheses)));
}
float calculateHeightPosition(formulaTree* node)
{
	return ((node->maxHeightParantheses + node->minHeightParantheses) - 
		std::abs(node->minHeightParantheses - node->maxHeightParantheses) -
		getDrawableParantheses("(",0,0,optimalFontSize(node)).getLocalBounds().height)* 0.5f;
}

void buildFormulaCoords(formulaTree* node, float& xPos, float yPos, float& maximumHeight, float& minimumHeight)
{					
	if (node == NULL) return;

	if (isLeaf(node))
	{
		node->xPosOfFormula = xPos;
		node->yPosOfFormula = yPos;
		xPos = xPos + getDrawableFormula(node->treeFormula, xPos, yPos).getLocalBounds().width;
		maximumHeight = std::max(maximumHeight, yPos + getDrawableFormula(node->treeFormula, xPos, yPos).getLocalBounds().height);
		minimumHeight = std::min(minimumHeight,yPos + getDrawableFormula(node->treeFormula, xPos, yPos).getLocalBounds().height);
		return;
	}
	
	if (node->operation == '/') {
		float xAxisDifferenceUp, xAxisDifferenceDown;
		xAxisDifferenceUp = xAxisDifferenceDown = xPos;

		node->yPosOfFormula = yPos;
		node->xPosOfFormula = xPos;
		
		xPos += 10;
		float savedX = xPos;
		float savedMinimumHeight = yPos;
		minimumHeight = 10000;
		maximumHeight = -1;
		buildFormulaCoords(node->leftTree, xPos, yPos - 22, maximumHeight, minimumHeight);
		if (node->hasParentheses) node->minHeightParantheses = minimumHeight;
		xAxisDifferenceUp = std::abs(xPos - savedX);

		xPos = savedX;
		float maxHeight = maximumHeight;

		minimumHeight = 10000;
		maximumHeight = -1;


		buildFormulaCoords(node->rightTree, xPos, yPos + 22, maximumHeight, minimumHeight);
		if (node->hasParentheses) node->maxHeightParantheses = maximumHeight;
		xAxisDifferenceDown = std::abs(xPos - savedX);
		float minHeight = minimumHeight;
		float diffInAxis = std::max(xAxisDifferenceDown, xAxisDifferenceUp);
		node->lengthOfDivision = diffInAxis;
		node->xPosOfOperator = savedX;
		node->xPosOfParentheses = savedX;
		node->yPosOfOperator = yPos + 20;
		node->xPosOfFormulaEnd = savedX + getDrawableParantheses(")", node->xPosOfFormula, calculateHeightPosition(node),
			optimalFontSize(node)).getLocalBounds().width + diffInAxis + 10;
		node->xPosOfOperator = savedX + getDrawableParantheses("(", node->xPosOfFormula, calculateHeightPosition(node),
			optimalFontSize(node)).getLocalBounds().width;

		xPos = (savedX + getDrawableParantheses("(", node->xPosOfFormula, calculateHeightPosition(node),
			optimalFontSize(node)).getLocalBounds().width + static_cast<float>(std::abs(xAxisDifferenceUp - diffInAxis)) * 0.5f);
		minimumHeight = 10000;
		maximumHeight = -1;
	
		buildFormulaCoords(node->leftTree, xPos, yPos - 22 - std::abs(yPos - maxHeight), maximumHeight, minimumHeight);

		xPos = (node->xPosOfOperator + static_cast<float>(std::abs(xAxisDifferenceDown - diffInAxis)) * 0.5f);

		buildFormulaCoords(node->rightTree, xPos, yPos + 22 + std::max(0.0f,(yPos + 42 - minHeight)), maximumHeight, minimumHeight);
		if (node->hasParentheses) xPos += getDrawableParantheses(")", node->xPosOfFormula, calculateHeightPosition(node),
			optimalFontSize(node)).getLocalBounds().width;
		return;
	}
	if (node->operation == '^')
	{
		
		buildFormulaCoords(node->leftTree, xPos, yPos, maximumHeight, minimumHeight);
		
		maximumHeight = -1;
		xPos+=10;
		float savedXPos = xPos;
		buildFormulaCoords(node->rightTree, xPos, yPos - 5, maximumHeight, minimumHeight);
		
		buildFormulaCoords(node->rightTree, savedXPos, yPos - 5 - std::max(float(0),maximumHeight-(yPos-5)), maximumHeight, minimumHeight);
		if (node->rightTree->hasParentheses) {
			node->maxHeightParantheses = maximumHeight;
			node->minHeightParantheses = minimumHeight;
		}
		return;
	}

	node->xPosOfFormula = xPos;
	float maxH = -1, minH = 100000;

	xPos = node->xPosOfFormula;
	buildFormulaCoords(node->leftTree, xPos, yPos, maximumHeight, minimumHeight);
	maxH = maximumHeight; minH = minimumHeight;
	xPos += 10;
	node->xPosOfOperator = xPos;
	node->yPosOfOperator = yPos;
	xPos = xPos + operatorToText(node->operation, xPos, yPos).getLocalBounds().width;
	xPos += 10;
	minimumHeight = 10000;
	maximumHeight = -1;
	
	buildFormulaCoords(node->rightTree, xPos, yPos, maximumHeight, minimumHeight);
	maxH = std::max(maxH, maximumHeight);
	minH = std::min(minH, minimumHeight);
	xPos = node->xPosOfFormula;
	node->maxHeightParantheses = maxH; node->minHeightParantheses = minH;
	node->xPosOfParentheses = xPos;
	if (node->hasParentheses) xPos = xPos + getDrawableParantheses("(", node->xPosOfParentheses, calculateHeightPosition(node),
		optimalFontSize(node)).getLocalBounds().width;
	node->xPosOfFormula = xPos;
	minimumHeight = 10000;
	maximumHeight = -1;
	xPos += 5;
	
	buildFormulaCoords(node->leftTree, xPos, yPos, maximumHeight, minimumHeight);
	xPos += 10;
	node->xPosOfOperator = xPos;
	node->yPosOfOperator = yPos;
	xPos = xPos + operatorToText(node->operation, xPos, yPos).getLocalBounds().width;
	xPos += 10;

	
	buildFormulaCoords(node->rightTree, xPos, yPos, maximumHeight, minimumHeight);
	


	node->xPosOfFormulaEnd = xPos;

	node->yPosOfFormula = yPos;

	if (node->hasParentheses) xPos += getDrawableParantheses(")", node->xPosOfFormula, calculateHeightPosition(node),
		optimalFontSize(node)).getLocalBounds().width;

	return;
}

void drawFormula(formulaTree* node)
{
	if (node == NULL) return;
	if (isLeaf(node)) { window.draw(getDrawableFormula(node->treeFormula, node->xPosOfFormula, node->yPosOfFormula)); return; }
	if (node->hasParentheses) window.draw(getDrawableParantheses("(", node->xPosOfParentheses ,  calculateHeightPosition(node),
		optimalFontSize(node)));
	drawFormula(node->leftTree);
	if (node->operation != '/' && node->operation != '^') {
		window.draw(operatorToText(node->operation, node->xPosOfOperator, node->yPosOfOperator));
	}
	if (node->operation == '/') {
		sf::RectangleShape line(sf::Vector2f(node->lengthOfDivision, 1));
		line.setPosition(node->xPosOfOperator, node->yPosOfOperator);
		line.setFillColor(sf::Color::White);
		window.draw(line);
	}
	drawFormula(node->rightTree);
	if (node->hasParentheses)  window.draw(getDrawableParantheses(")", node->xPosOfFormulaEnd, calculateHeightPosition(node),
		std::max(float(28), float(28) + std::abs(node->minHeightParantheses - node->maxHeightParantheses))));
}

int main()
{
	if (!mathFont.loadFromFile("assets/math_font.otf")) { exit(1); }
	
	std::string input = "(x+y+1/2/3/4/5)/(1/2+y-x)";
	formulaTree* root = buildFormulaTree(input);

	float test = 100;
	float minH = 1000, maxH = 0;
	buildFormulaCoords(root,test,600,maxH,minH);
	
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{

			if (event.type == sf::Event::Closed)
				window.close();
		}
		window.clear(sf::Color::Black);
		drawFormula(root);
		
		window.display();
	}

} 