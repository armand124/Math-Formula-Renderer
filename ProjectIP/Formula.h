#include "SFML/Graphics.hpp"
#include "StringHandle.h"
#include <SFML/Window.hpp>
#include <stack>
#include <string>
#include <iostream>
class Formula {
private:
	float characterSize = 40.f;
	sf::Font mathFont, parFont;
	sf::RenderWindow* window;
	Parser parser;
	//float characterSize = 40.f;
	const char multiply = char(183);
	float spacing = characterSize * 0.25f;
	struct formulaTree {
		char operation;
		bool paranthases;
		std::string formula;
		std::string function;
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

	//Misc
	bool isOnlyTerm(formulaTree* node) { return (node->leftTree == NULL && node->rightTree == NULL); }
	std::string deleteParantheses(std::string str)
	{
		str.pop_back();
		return str.substr(1);
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

	void drawSquareRoot(float xDown, float yDown, float xUp, float yUp) {
		sf::VertexArray squareRoot(sf::LinesStrip, 4);

		squareRoot[0].position = sf::Vector2f(xDown, yDown - (yDown - yUp) * 0.4);
		squareRoot[1].position = sf::Vector2f(xDown + characterSize * 0.66, yDown);
		squareRoot[2].position = sf::Vector2f(xDown + characterSize * 1.33, yUp);
		squareRoot[3].position = sf::Vector2f(xUp, yUp);

		// Set the color for the square root symbol
		for (size_t i = 0; i < squareRoot.getVertexCount(); ++i) {
			squareRoot[i].color = sf::Color::White;
		}

		// Draw the square root symbol on the window
		window->draw(squareRoot);
	}


	//--------------------------------------------------------------------------------------
	formulaTree* buildTree(std::string formula)
	{
		//Create new node of tree
		formulaTree* node = new formulaTree(formula);

		//If it has paranthases then we eliminate them and go to the next node
		int numberOfOperators = 0;
		int numberOfMulDivOperators = 0;
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
		//Check if the input is a function

		node->function = parser.checkForFunction(formula);

		if (node->function != "none")
		{
			if (node->function == "sqrt")
				node->leftTree = buildTree(deleteParantheses(formula.substr(node->function.size())));
			else
				node->leftTree = buildTree(formula.substr(node->function.size()));
			return node;
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

			if ((formula[i] == multiply || formula[i] == '/') && currentPriority < mulDivPriority)
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
			buildFormulaCoordinates(node->rightTree, (heightPos + 0.7f * characterSize) * 2 - node->rightTree->height.up + characterSize * 0.28f, widthPos);

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

			//Get base coordinates
			buildFormulaCoordinates(node->leftTree, heightPos, widthPos);
			float savedWidthPos = widthPos;

			//Calculate the proportion of the exponent
			buildFormulaCoordinates(node->rightTree, heightPos - 0.4f * characterSize, widthPos);
			widthPos = savedWidthPos;

			//Recalculate the position depending on the size of the exponent
			buildFormulaCoordinates(node->rightTree, (heightPos - 0.4f * characterSize) * 2 - node->rightTree->height.down - node->leftTree->size.height, widthPos);

			//Save the information of the current formula
			node->height.up = node->rightTree->height.up;
			node->height.down = node->leftTree->height.down;
			node->size.width = node->leftTree->size.width + node->rightTree->size.width;
			node->size.height = std::max(0.f, node->height.down - node->height.up);
			return;

		}
		else if (node->paranthases)
		{
			node->position.xOperator = widthPos;
			//Calculate the proportion of the formula within the parantheses
			buildFormulaCoordinates(node->leftTree, heightPos, widthPos);

			widthPos = node->position.xOperator + getWidthOfParantheses('(', node->leftTree->size.height) + spacing;

			//Recalculate the positions
			buildFormulaCoordinates(node->leftTree, heightPos, widthPos);

			//Adjust height of parantheses depending on the proportions of the formula
			node->position.yOperator = (node->leftTree->height.down + node->leftTree->height.up
				- std::abs(node->leftTree->height.down - node->leftTree->height.up)) * 0.5f
				- (node->leftTree->size.height > 0 ? (getHeightOfParantheses('(', node->leftTree->size.height) * 0.25f) : 0)
				+ (node->leftTree->operation == '^' ? characterSize * 0.7f : 0);
			widthPos = widthPos + spacing + getWidthOfParantheses(')', node->leftTree->size.height);

			//Save the information of the current formula
			node->size.width = widthPos - node->position.xOperator;
			node->height.up = node->leftTree->height.up;
			node->height.down = node->leftTree->height.down;
			node->size.height = std::abs(node->height.up-node->height.down);
			return;
		}
		else if (node->function != "none")
		{

			node->position.xOperator = widthPos;
			node->position.yOperator = heightPos;
			widthPos += getWidthOfString(node->function);

			if (node->function == "sqrt")
			{
				buildFormulaCoordinates(node->leftTree, heightPos + characterSize * 0.20f, widthPos);
			}
			else
				buildFormulaCoordinates(node->leftTree, heightPos, widthPos);

			node->size.width = widthPos - node->position.xOperator;
			node->size.height = node->leftTree->size.height;
			node->height.up = node->leftTree->height.up - characterSize * 0.20f;
			node->height.down = node->leftTree->height.down;
			return;
		}
		else if (!node->paranthases)
		{
			//Build the first term
			buildFormulaCoordinates(node->leftTree, heightPos, widthPos);

			//Build the operator
			widthPos += spacing;
			node->position.xOperator = widthPos;
			node->position.yOperator = heightPos;
			widthPos += (spacing + getWidthOfChar(node->operation));

			//Build the second term
			buildFormulaCoordinates(node->rightTree, heightPos, widthPos);

			//Save the information of the current formula
			node->height.down = std::max(node->leftTree->height.down, node->rightTree->height.down);
			node->height.up = std::min(node->leftTree->height.up, node->rightTree->height.up);
			node->size.width = node->leftTree->size.width + node->rightTree->size.width +
				2 * spacing + getWidthOfChar(node->operation);
			node->size.height = std::abs(node->height.down - node->height.up);
			return;
		}
	}
	void drawFormula(formulaTree* node)
	{
		//std::cout << characterSize << '\n';
		if (node == NULL) return;

		if (isOnlyTerm(node))
		{
			sf::Text text;
			text.setCharacterSize(characterSize);
			text.setFillColor(sf::Color::White);
			text.setPosition(node->position.xFormula, node->position.yFormula);
			text.setFont(mathFont);
			text.setString(node->formula);
			window->draw(text);
			return;
		}
		else if (node->paranthases)
		{
			sf::Text text;
			text.setCharacterSize(characterSize + node->leftTree->size.height);
			text.setFillColor(sf::Color::White);
			text.setPosition(node->position.xOperator, node->position.yOperator);
			text.setFont(parFont);
			text.setString("(");
			window->draw(text);
			drawFormula(node->leftTree);
			text.setString(")");
			text.setPosition(node->position.xOperator + node->leftTree->size.width
				+ getWidthOfParantheses(')', node->leftTree->size.height) * 0.609 + int(characterSize * 3 / getWidthOfParantheses(')', node->leftTree->size.height)), node->position.yOperator);
			window->draw(text);
			return;
		}
		else if (node->function != "none")
		{
			if (node->function == "sqrt")
			{
				drawSquareRoot(node->position.xOperator, node->leftTree->height.down + characterSize, node->leftTree->size.width + node->position.xOperator + getWidthOfString("sqrt"),
					node->leftTree->height.up);
			}
			else {
				sf::Text text;
				text.setCharacterSize(characterSize);
				text.setFillColor(sf::Color::White);
				text.setPosition(node->position.xOperator, node->position.yOperator);
				text.setFont(mathFont);
				text.setString(node->function);
				window->draw(text);
			}
			drawFormula(node->leftTree);
			return;
		}
		else if (node->operation == '^')
		{
			drawFormula(node->leftTree);
			drawFormula(node->rightTree);
		}
		else if (node->operation == '/')
		{
			drawFormula(node->leftTree);
			sf::RectangleShape line(sf::Vector2f(node->size.width, characterSize * 0.04f));
			line.setPosition(node->position.xOperator, node->position.yOperator);
			line.setFillColor(sf::Color::White);
			window->draw(line);
			drawFormula(node->rightTree);
		}
		else if (node->operation != '^' && node->operation != '/')
		{
			sf::Text text;
			text.setCharacterSize(characterSize);
			text.setFillColor(sf::Color::White);
			text.setPosition(node->position.xOperator, node->position.yOperator);
			text.setFont(mathFont);
			std::string s; s.push_back(node->operation);
			text.setString(s);
			drawFormula(node->leftTree);
			window->draw(text);
			drawFormula(node->rightTree);
		}

	}

public:
	float sizedif = 40.f;
	inline Formula()
	{
		//Load the fonts
		if (!mathFont.loadFromFile("assets/math_font.otf")) exit(1);
		if (!parFont.loadFromFile("assets/fontParanteze.ttf")) exit(1);
	}

	void printFormula(std::string str, sf::RenderWindow& screen)
	{
		formulaTree* root = buildTree(str);

		float x = 0;
		buildFormulaCoordinates(root, 0, x);
		float xOffset = (screen.getSize().x - root->size.width) * 0.5f;
		buildFormulaCoordinates(root, std::max(0.f, screen.getSize().y * 0.1f - root->height.up), xOffset);
		window = &screen;
		drawFormula(root);
	}

	void setCharacterSize(float size)
	{
		characterSize = size;
		spacing = characterSize * 0.25f;
	}
	
	float getCharacterSize()
	{
		return characterSize;
	}

};