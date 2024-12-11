#include "SFML/Graphics.hpp"
#include "StringHandle.h"
#include <string>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <stack>
#include <iomanip>
#include <queue>
int max(int x, int y)
{
	if (x > y)
		return x;
	return y;
}

class Gui {
private:
	std::size_t WIDTH, HEIGHT;
	std::string applicationTitle;
	sf::RenderWindow window;
	sf::Event event;
	bool startScreen = true;
	class Components {
	public:
		sf::Font mathFont;
		struct InputBoxCluster {
			sf::RectangleShape Frame;
			sf::Text inText;
			std::string userInput = "";
		};
		void loadFont()
		{
			if (!mathFont.loadFromFile("assets/math_font.otf")) { exit(1); }
		}
		InputBoxCluster InputBox(Gui& gui, std::size_t width, std::size_t height)
		{
			sf::Text text;
			text.setFont(mathFont);
			text.setCharacterSize((height * 0.40));
			text.setFillColor(sf::Color::White);
			text.setPosition(gui.getWindowWidth() * 0.21, gui.getWindowHeight() * 0.815);
			sf::RectangleShape rectangle(sf::Vector2f(width, height));
			rectangle.setSize(sf::Vector2f(width, height * 0.80));
			rectangle.setFillColor(sf::Color(36, 36, 36, 140));
			rectangle.setPosition(gui.getWindowWidth() * 0.2, gui.getWindowHeight() * 0.8);
			InputBoxCluster returnedCluster;
			returnedCluster.Frame = rectangle;
			returnedCluster.inText = text;
			return returnedCluster;
		}


	};
	void initWindow() {
		window.create(sf::VideoMode(static_cast<int>(WIDTH),
			static_cast<int>(HEIGHT)), applicationTitle);
	}
public:
	Gui(const std::string& titleOfApp, std::size_t appWidth, std::size_t appHeight) :
		applicationTitle(titleOfApp), WIDTH(appWidth), HEIGHT(appHeight) {
		initWindow();
	}

	void setWindowWidth(std::size_t newWidth) { WIDTH = newWidth; }
	void setWindowHeight(std::size_t newHeight) { HEIGHT = newHeight; }
	const std::size_t& getWindowWidth() const { return WIDTH; }
	const std::size_t& getWindowHeight() const { return HEIGHT; }

	void run()
	{
		sf::Font generalFont;
		if (!generalFont.loadFromFile("assets/arial.ttf")) { exit(1); }
		Components component;
		component.loadFont();
		sf::Color backgroundColor(51, 51, 51, 255);
		sf::RectangleShape inputBoxFrame = component.InputBox(*this, 0.6 * WIDTH, 0.06 * HEIGHT).Frame;
		sf::Text inputBoxText = component.InputBox(*this, 0.6 * WIDTH, 0.06 * HEIGHT).inText;
		std::string inputString = component.InputBox(*this, 0.6 * WIDTH, 0.06 * HEIGHT).userInput;
		sf::Text warningMessage;
		sf::Text formula;
		formula.setFont(component.mathFont);
		formula.setCharacterSize((HEIGHT * 0.06 * 0.60));
		formula.setFillColor(sf::Color::White);
		formula.setString("");
		formula.setPosition(WIDTH * 0.2, HEIGHT * 0.6);
		warningMessage.setFont(generalFont);
		warningMessage.setCharacterSize((HEIGHT * 0.06 * 0.40));
		warningMessage.setFillColor(sf::Color::Red);
		warningMessage.setPosition(warningMessage.getGlobalBounds().width, 0);
		warningMessage.setString("");
		str::Parser pars;
		bool startedTyping = false;
		bool warning = false;
		inputBoxText.setString("Scrie formula");


		//Welcome screen
		sf::Text welcomeMessage;
		welcomeMessage.setCharacterSize((HEIGHT * 0.07 * 0.40));
		welcomeMessage.setFillColor(sf::Color::White);
		welcomeMessage.setPosition(WIDTH * 0.21, HEIGHT * 0.815);

		while (window.isOpen())
		{

			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				if (event.type == sf::Event::TextEntered)
				{
					if (event.text.unicode == 13 || event.text.unicode == 9 || event.text.unicode == 27)
						continue;
					if (!startedTyping)
					{
						startedTyping = 1;
						inputBoxText.setString("");
					}
					if (event.text.unicode == 8 && !inputString.empty())
					{
						inputString.pop_back();
						inputBoxText.setString(inputString);
						if (warning)
							warning = 0;
					}
					else if (event.text.unicode != 8 && !warning) {
						inputString += event.text.unicode;
						inputBoxText.setString(inputString);
					}
					if (inputBoxText.getGlobalBounds().width >= WIDTH * 0.59) {
						char lastChar = inputString.back();
						inputString.pop_back();
						inputString.push_back('\n');
						inputString.push_back(lastChar);
						inputBoxText.setString(inputString);
					}
					if (inputBoxText.getGlobalBounds().height >= HEIGHT * 0.185)
					{
						warningMessage.setString("Expresia este prea lunga");
						warningMessage.setPosition((WIDTH - warningMessage.getGlobalBounds().width) * 0.5, 0);
						inputString.pop_back();
						inputBoxText.setString(inputString);

						warning = true;

					}
					else if (!warning) {
						inputBoxFrame.setSize(sf::Vector2f(0.6 * WIDTH, max(0.07 * 0.8 * HEIGHT, 0.07 * 0.4 * HEIGHT + inputBoxText.getGlobalBounds().height)));
						warningMessage.setString("");

					}
					if (!pars.isStringValid(inputString)) {
						warningMessage.setString("Formula este gresita!");
						formula.setString("");
					}
					else
					{
						std::string goodString = pars.formulaParsedString(inputString);
						formula.setString(goodString);

					}
				}
			}



			std::queue<sf::Text>expressionOrder;
			std::queue<sf::RectangleShape>fractionLine;
			if (formula.getString() != "")
			{
				std::string formulaString = formula.getString();
				sf::Text normalForm;
				normalForm.setFont(component.mathFont);
				normalForm.setCharacterSize((HEIGHT * 0.06 * 0.6));
				normalForm.setFillColor(sf::Color::White);
				std::string currentString;
				normalForm.setPosition(WIDTH * 0.2, HEIGHT * 0.6);
				normalForm.setString("  ");
				int space = normalForm.getGlobalBounds().width;
				normalForm.setString("");
				int i = 0;
				int lastX = WIDTH * 0.2, lastY = HEIGHT * 0.6;
				while (i < formulaString.size())
				{
					if (formulaString[i] == '/')
					{
						std::string numit = "";
						std::string numar = "";
						currentString.pop_back();
						currentString.pop_back();
						int j = i - 2;
						if (formulaString[j] != ')')
						{
							numar.push_back(formulaString[j]);
						}
						else {
							j--;
							std::stack<int>ss;
							ss.push(1);
							while (j >= 0)
							{
								if (formulaString[j] == ')')
									ss.push(1);
								if (formulaString[j] == '(')
									ss.pop();
								if (ss.empty())
									break;
								numar.push_back(formulaString[j]);
								currentString.pop_back();
								j--;
							}
							std::reverse(numar.begin(), numar.end());
						}
						j = i + 2;
						if (formulaString[j] != '(')
						{
							numit.push_back(formulaString[j]);
							j++;
						}
						else {
							j++;
							std::stack<int>ss;
							ss.push(1);
							while (j < formulaString.size())
							{
								if (formulaString[j] == '(')
									ss.push(1);
								if (formulaString[j] == ')')
									ss.pop();
								if (ss.empty())
									break;
								numit.push_back(formulaString[j]);
								j++;
							}
						}
						i = j;
						sf::Text numarator, numitor;
						currentString.pop_back();
						normalForm.setPosition(lastX, lastY);
						normalForm.setString(currentString);

						numarator.setFont(component.mathFont);
						numarator.setCharacterSize((HEIGHT * 0.06 * 0.60));
						numarator.setFillColor(sf::Color::White);
						numarator.setString(numar);


						numitor.setFont(component.mathFont);
						numitor.setCharacterSize((HEIGHT * 0.06 * 0.60));
						numitor.setFillColor(sf::Color::White);
						numitor.setString(numit);

						double diff = std::abs(numitor.getLocalBounds().width - numarator.getLocalBounds().width);
					
						numarator.setPosition(lastX + normalForm.getLocalBounds().width + space + (numarator.getLocalBounds().width < numitor.getLocalBounds().width ? diff * 0.5 : 0)
							, HEIGHT * 0.6 - (normalForm.getLocalBounds().height));
						numitor.setPosition(lastX + normalForm.getLocalBounds().width + space + (numarator.getLocalBounds().width > numitor.getLocalBounds().width ? diff * 0.5 : 0)
							, HEIGHT * 0.6 + (normalForm.getLocalBounds().height));

						sf::RectangleShape line(sf::Vector2f(max(numarator.getGlobalBounds().width, numitor.getGlobalBounds().width), 2));

						line.setFillColor(sf::Color::White);
						line.setPosition(lastX + normalForm.getLocalBounds().width + space, HEIGHT * 0.6 + (normalForm.getLocalBounds().height));


						expressionOrder.push(numarator);
						expressionOrder.push(numitor);
						expressionOrder.push(normalForm);
						fractionLine.push(line);
						lastX = (line.getPosition().x + line.getGlobalBounds().width);
						normalForm.setString("");
						currentString = "";
					}
					else
					{
						currentString.push_back(formulaString[i]);
						normalForm.setPosition(lastX, HEIGHT * 0.6);

						normalForm.setString(currentString);

					}
					i++;
				}
				expressionOrder.push(normalForm);
			}



			window.clear();
			while (!expressionOrder.empty())
			{
				window.draw(expressionOrder.front());
				expressionOrder.pop();
			}
			while (!fractionLine.empty())
			{
				window.draw(fractionLine.front());
				fractionLine.pop();
			}
			window.draw(inputBoxFrame);
			window.draw(warningMessage);
			window.draw(inputBoxText);
			//----------------------------------------
			window.display();
		}

	}
};

int main()
{
	Gui app("Miawer", 1920, 1080);
	app.run();
}