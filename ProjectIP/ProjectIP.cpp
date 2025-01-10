#include "SFML/Graphics.hpp"
#include "StringHandle.h"
#include <string>
#include <cmath>
#include <iostream>
#include <algorithm>
#include <stack>
#include <iomanip>
#include <queue>
#include <vector>
#include "Formula.h"
#include "Button.h"

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
	Formula formulaDraw;
	Formula size;
	std::string buttonsstring[29] = { "1", "2", "3", "+", "(", "sin","arcsin", "4",
	"5", "6", "-", ")", "cos", "arccos", "7", "8", "9", "=", "ln", "tg", "arctg",
	"*", "0", "/", "^", "sqrt", "ctg", "arcctg"};
	float charSize = 40.f;
	bool startScreen = true;
	struct ButtonPair {
		sf::RectangleShape bk;
		sf::Text txt;
	};

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
			text.setPosition(gui.getWindowWidth() * 0.21, gui.getWindowHeight() * 0.5);
			sf::RectangleShape rectangle(sf::Vector2f(width, height));
			rectangle.setSize(sf::Vector2f(width, height * 0.80));
			rectangle.setFillColor(sf::Color(36, 36, 36, 140));
			rectangle.setPosition(gui.getWindowWidth() * 0.2, gui.getWindowHeight() * 0.5);
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
		warningMessage.setPosition(warningMessage.getGlobalBounds().width, 30);
		warningMessage.setString("");
		Parser pars;
		bool startedTyping = false;
		bool warning = false;
		inputBoxText.setString("Scrie formula");
		sf::RectangleShape minusButton(sf::Vector2f(40, 20));
		minusButton.setPosition(60, 10);
		minusButton.setFillColor(sf::Color::White);

		sf::Text minusText;
		minusText.setString("-");
		minusText.setFont(generalFont);
		minusText.setCharacterSize(25);
		minusText.setFillColor(sf::Color::Black);
		minusText.setPosition(
			minusButton.getPosition().x + (minusButton.getSize().x - minusText.getLocalBounds().width) / 2,
			minusButton.getPosition().y + (minusButton.getSize().y - minusText.getLocalBounds().height) / 2 - 15
		);
		sf::RectangleShape plusButton(sf::Vector2f(40, 20));
		plusButton.setPosition(10, 10);
		plusButton.setFillColor(sf::Color::White);

		sf::Text plusText;
		plusText.setString("+");
		plusText.setFont(generalFont);
		plusText.setCharacterSize(20);
		plusText.setFillColor(sf::Color::Black);
		plusText.setPosition(
			plusButton.getPosition().x + (plusButton.getSize().x - plusText.getLocalBounds().width) / 2,
			plusButton.getPosition().y + (plusButton.getSize().y - plusText.getLocalBounds().height) / 2 - 5
		);
		
		ButtonPair buttons[29];
		int pozx=WIDTH*0.29, pozy=HEIGHT*0.509;
		for (int i = 0;i < 28;i++)
		{
			sf::RectangleShape Button(sf::Vector2f(100, 60));
			if (i % 7 == 0)
			{
				pozx=WIDTH*0.29;
				pozy += 70;
			}

			Button.setPosition(pozx, pozy);
			Button.setFillColor(sf::Color::White);

			sf::Text Text;
			Text.setString(buttonsstring[i]);
			Text.setFont(generalFont);
			Text.setCharacterSize(25);
			Text.setFillColor(sf::Color::Black);
			Text.setPosition(
				Button.getPosition().x + (Button.getSize().x - Text.getLocalBounds().width) / 2,
				Button.getPosition().y + (Button.getSize().y - Text.getLocalBounds().height) / 2 - 5
			);
			ButtonPair pair;
			pair.bk = Button;
			pair.txt = Text;
			buttons[i] = pair;
			pozx += 110;
		}


		int delay = 0;
		while (window.isOpen())
		{
			sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				for (int i = 0;i < 28;i++)
				{
					if (buttons[i].bk.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition)))
					{
						buttons[i].bk.setFillColor(sf::Color::Green);
						if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left)
						{
							buttons[i].bk.setFillColor(sf::Color::Red);
							inputString.append(buttonsstring[i]);
							inputBoxText.setString(inputString);
							formula.setString(inputString);

							if (inputString != "") {
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
					}
					else
					{
						buttons[i].bk.setFillColor(sf::Color::White);
					}
				}
				if (minusButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					minusButton.setFillColor(sf::Color::Green); 
					if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
						minusButton.setFillColor(sf::Color::Red); 
						charSize = std::max(charSize - 4, 22.f);
					}
				}
				else {
					minusButton.setFillColor(sf::Color::White);
				}

				if (plusButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					plusButton.setFillColor(sf::Color::Green); 
					if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
						plusButton.setFillColor(sf::Color::Red);
						charSize = std::min(charSize + 4, 60.f);
					}
				}
				else {
					plusButton.setFillColor(sf::Color::White); 
				}
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
					if (inputString != "") {
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
			}

			//-----------------------------------------------
			//Draw
			window.clear();
			if (inputString == "") formula.setString("");
			if (inputString != "" && !pars.isStringValid(inputString)) warningMessage.setString("Formula este gresita!");
			else warningMessage.setString("");
			formulaDraw.printFormula(formula.getString(), window, charSize);
			window.draw(minusButton);
			window.draw(minusText);
			window.draw(plusButton);
			window.draw(plusText);
			for (int i = 0;i < 28;i++) { window.draw(buttons[i].bk); window.draw(buttons[i].txt); }
			window.draw(warningMessage);
			window.draw(inputBoxFrame);
			window.draw(inputBoxText);
			//----------------------------------------
			window.display();
		}
	}
};

int main()
{
	Gui app("Vizualizator Formule", 1920, 1080);
	app.run();
}
