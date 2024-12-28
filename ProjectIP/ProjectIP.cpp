#include <iostream>
#include <string>
#include "SFML/Graphics.hpp"
#include <stack>
#include "StringHandle.h"

sf::Font mathFont, parFont;
sf::RenderWindow window(sf::VideoMode(1200, 1200), "Vizualizator formule");
float characterSize = 26;
float spacing = characterSize * 0.25f;
struct formulaTree {
	char operation;
	bool paranthases;
	std::string formula;
	formulaTree* leftTree, * rightTree;
	
	formulaTree(std::string treeForm) {
		leftTree = NULL;
		rightTree = NULL;
		paranthases = false;
		operation = 0;
		formula = treeForm;
	}
	struct position {
		float xFormula = 0, yFormula = 0;
		float xOperator = 0, yOperator = 0;
	} position;

	struct size {
		float width = 0;
		float height = 0;
	} size;

	struct height {
		float down = 0;
		float up = 0;
	} height;
};

bool isOnlyTerm(formulaTree* node) { return (node->leftTree == NULL && node->rightTree == NULL); }
std::string deleteParantheses(std::string str)
{
	str.pop_back();
	return str.substr(1);
}

formulaTree* buildTree(std::string formula)
{
	//Create new node of tree
	formulaTree* node = new formulaTree(formula);

	//If it has paranthases then we eliminate them and go to the next node
	if (formula[0] == '(' && formula[int(formula.size() - 1)] == ')')
	{
		std::stack<int>stk;
		for (int i = 0; i < formula.size() - 1; i++)
		{
			if (formula[i] == '(')
				stk.push(i);
			if (formula[i] == ')')
				stk.pop();
		}
		if (stk.top() == 0)
		{
			node->paranthases = true;
			node->leftTree = buildTree(deleteParantheses(formula));
			return node;
		}
	}

	//Check whats the lowest priority operation in the current string
	int addSubIndex = -1, addSubPriority = 1e9;
	int mulDivIndex = -1, mulDivPriority = 1e9;
	int powIndex = -1, powPriority = 1e9;

	int minimumPriority = 1e9, currentPriority = 0;

	for (int i = 0; i < formula.size(); i++)
	{
		if (formula[i] == '(') currentPriority++;
		if (formula[i] == ')') currentPriority--;

		if ((formula[i] == '+' || formula[i] == '-') && currentPriority < addSubPriority)
		{
			addSubPriority = currentPriority;
			addSubIndex = i;
		}

		if ((formula[i] == '*' || formula[i] == '/') && currentPriority < mulDivPriority)
		{
			mulDivPriority = currentPriority;
			mulDivIndex = i;
		}

		if ((formula[i] == '^') && currentPriority < powPriority)
		{
			powPriority = currentPriority;
			powIndex = i;
		}
	}

	//Check if there is at least an operator
	if (addSubIndex == -1 && mulDivIndex == -1 && powIndex == -1)
		return node;

	//Find which operator has the lowest priority and it's location
	minimumPriority = std::min(std::min(addSubPriority, mulDivPriority), powPriority);

	int operatorIndex = 0;

	if (minimumPriority == addSubPriority) operatorIndex = addSubIndex;
	else if (minimumPriority == mulDivPriority) operatorIndex = mulDivIndex;
	else operatorIndex = powIndex;

	//Break the current formula in two parts -> the left part of the operation and the right part
	node->operation = formula[operatorIndex];
	node->leftTree = buildTree(formula.substr(0, operatorIndex));
	node->rightTree = buildTree(formula.substr(operatorIndex + 1));
	return node;
}

float getWidthOfString(std::string str)
{
	sf::Text txt;
	txt.setFont(mathFont);
	txt.setCharacterSize(characterSize);
	txt.setFillColor(sf::Color::White);
	txt.setString(str);
	txt.setPosition(0, 0);
	return txt.getLocalBounds().width;
}
float getHeightOfString(std::string str)
{
	sf::Text txt;
	txt.setFont(mathFont);
	txt.setCharacterSize(characterSize);
	txt.setFillColor(sf::Color::White);
	txt.setString(str);
	txt.setPosition(0, 0);
	return txt.getLocalBounds().height;
}
float getWidthOfChar(char c)
{
	sf::Text txt;
	std::string str;
	str.push_back(c);
	txt.setFont(mathFont);
	txt.setCharacterSize(characterSize);
	txt.setFillColor(sf::Color::White);
	txt.setString(str);
	txt.setPosition(0, 0);
	return txt.getLocalBounds().width;
}
float getWidthOfParantheses(char p, float height)
{
	sf::Text txt;
	std::string str;
	str.push_back(p);
	txt.setFont(mathFont);
	txt.setCharacterSize(characterSize + height);
	txt.setFillColor(sf::Color::White);
	txt.setString(str);
	txt.setPosition(0, 0);
	return txt.getLocalBounds().width;
}
float getHeightOfParantheses(char p, float height)
{
	sf::Text txt;
	std::string str;
	str.push_back(p);
	txt.setFont(mathFont);
	txt.setCharacterSize(characterSize + height);
	txt.setFillColor(sf::Color::White);
	txt.setString(str);
	txt.setPosition(0, 0);
	return txt.getLocalBounds().height;
}

void buildFormulaCoordinates(formulaTree* node, float heightPos, float& widthPos)
{
	if (node == NULL) return;

	//Check if node is a leaf and mark the coordinates
	if (isOnlyTerm(node))
	{
		node->position.xFormula = widthPos;
		node->position.yFormula = heightPos;
		node->height.up = heightPos;
		node->height.down = heightPos;
		node->size.width = getWidthOfString(node->formula);
		node->size.height = getHeightOfString(node->formula);
		widthPos += getWidthOfString(node->formula);
		return;
	}

	if (node->operation == '/')
	{

		//Get height and width coordinates
		float saveWidthPos = widthPos;
		buildFormulaCoordinates(node->leftTree, heightPos - 0.7f * characterSize, widthPos);
		widthPos = saveWidthPos;
		buildFormulaCoordinates(node->rightTree, heightPos + 0.7f * characterSize, widthPos);
		widthPos = saveWidthPos;

		//Rebuild the position with correct settling

		widthPos += (std::max(node->leftTree->size.width, node->rightTree->size.width) - node->leftTree->size.width) * 0.5f;
		buildFormulaCoordinates(node->leftTree, (heightPos - 0.7f * characterSize) * 2 - node->leftTree->height.down, widthPos);

		widthPos = saveWidthPos + (std::max(node->leftTree->size.width, node->rightTree->size.width) - node->rightTree->size.width) * 0.5f;
		buildFormulaCoordinates(node->rightTree, (heightPos + 0.7f * characterSize) * 2 - node->rightTree->height.up, widthPos);

		node->height.up = node->leftTree->height.up;
		node->height.down = node->rightTree->height.down;
		node->size.width = std::max(node->leftTree->size.width, node->rightTree->size.width);
		node->size.height = std::abs(node->height.down - node->height.up);

		node->position.xOperator = saveWidthPos;
		node->position.yOperator = heightPos + 0.7f * characterSize;

		//Reset position after fraction
		widthPos = saveWidthPos + std::max(node->leftTree->size.width, node->rightTree->size.width);
		return;
	}
	else if (node->operation == '^')
	{

		buildFormulaCoordinates(node->leftTree, heightPos, widthPos);
		float savedWidthPos = widthPos;
		buildFormulaCoordinates(node->rightTree, heightPos - 0.4f * characterSize, widthPos);
		widthPos = savedWidthPos;
		buildFormulaCoordinates(node->rightTree, (heightPos - 0.4f * characterSize) * 2
			- node->rightTree->height.down - node->leftTree->size.height, widthPos);

		node->height.up = node->rightTree->height.up;
		node->height.down = node->leftTree->height.down;
		node->size.width = node->leftTree->size.width + node->rightTree->size.width;
		node->size.height = std::max(0.f,node->height.down - node->height.up);
		return;

	}
	else if (!node->paranthases)
	{
		buildFormulaCoordinates(node->leftTree, heightPos, widthPos);

		widthPos += spacing;
		node->position.xOperator = widthPos;
		node->position.yOperator = heightPos;
		widthPos += (spacing + getWidthOfChar(node->operation));

		buildFormulaCoordinates(node->rightTree, heightPos, widthPos);

		node->height.down = std::max(node->leftTree->height.down, node->rightTree->height.down);
		node->height.up = std::min(node->leftTree->height.up, node->rightTree->height.up);

		node->size.width = node->leftTree->size.width + node->rightTree->size.width +
			2 * spacing + getWidthOfChar(node->operation);

		node->size.height = std::abs(node->height.down - node->height.up);
		return;
	}
	else if (node->paranthases)
	{
		node->position.xOperator = widthPos;


		buildFormulaCoordinates(node->leftTree, heightPos, widthPos);

		widthPos = node->position.xOperator + getWidthOfParantheses('(', node->leftTree->size.height) + spacing;
		std::cout << getWidthOfParantheses('(', node->leftTree->size.height) << '\n';
		buildFormulaCoordinates(node->leftTree, heightPos, widthPos);

		node->position.yOperator = (node->leftTree->height.down + node->leftTree->height.up
			- std::abs(node->leftTree->height.down - node->leftTree->height.up)) * 0.5f
			- (node->leftTree->size.height > 0 ? (getHeightOfParantheses('(', node->leftTree->size.height) * 0.25f) : 0);

		widthPos = widthPos + spacing + getWidthOfParantheses(')', node->leftTree->size.height);

		node->size.width = widthPos - node->position.xOperator;
		node->size.height = node->leftTree->size.height;

		node->height.up = node->leftTree->height.up;
		node->height.down = node->leftTree->height.down;
		return;
	}
}
void drawFormula(formulaTree* node)
{
	if (node == NULL) return;

	if (isOnlyTerm(node))
	{
		sf::Text text;
		text.setCharacterSize(characterSize);
		text.setFillColor(sf::Color::White);
		text.setPosition(node->position.xFormula, node->position.yFormula);
		text.setFont(mathFont);
		text.setString(node->formula);
		window.draw(text);
		return;
	}
	if (node->paranthases)
	{
		sf::Text text;
		text.setCharacterSize(characterSize + node->leftTree->size.height);
		text.setFillColor(sf::Color::White);
		text.setPosition(node->position.xOperator, node->position.yOperator);
		text.setFont(parFont);
		text.setString("(");
		window.draw(text);
		drawFormula(node->leftTree);
		text.setString(")");
		text.setPosition(node->position.xOperator + node->leftTree->size.width
			+ getWidthOfParantheses(')',node->leftTree->size.height)*0.609 + int(characterSize*3/getWidthOfParantheses(')', node->leftTree->size.height)), node->position.yOperator);
		window.draw(text);
		return;
	}
	if (node->operation == '^')
	{
		drawFormula(node->leftTree);
		drawFormula(node->rightTree);
	}
	if (node->operation == '/')
	{
		drawFormula(node->leftTree);
		sf::RectangleShape line(sf::Vector2f(node->size.width, characterSize * 0.04f));
		line.setPosition(node->position.xOperator, node->position.yOperator);
		line.setFillColor(sf::Color::White);
		window.draw(line);
		drawFormula(node->rightTree);
	}
	if (node->operation != '^' && node->operation != '/')
	{
		sf::Text text;
		text.setCharacterSize(characterSize);
		text.setFillColor(sf::Color::White);
		text.setPosition(node->position.xOperator, node->position.yOperator);
		text.setFont(mathFont);
		std::string s; s.push_back(node->operation);
		text.setString(s);
		drawFormula(node->leftTree);
		window.draw(text);
		drawFormula(node->rightTree);
	}


}
int main()
{
	if (!mathFont.loadFromFile("assets/math_font.otf")) exit(1);
	if (!parFont.loadFromFile("assets/fontParanteze.ttf")) exit(1);
	std::string str = "";
	formulaTree* root = buildTree(str);
	float w = 50, h = 600;
	buildFormulaCoordinates(root, h, w);
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