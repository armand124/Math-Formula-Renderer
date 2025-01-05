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
	float chasize = 40.f;
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
		sf::Font font1;
		if (!font1.loadFromFile("ariaal.ttf")) { 
			std::cerr << "Failed to load font!" << std::endl;
			exit(1);
		}
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
		minusText.setFont(font1);
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
		plusText.setFont(font1);
		plusText.setCharacterSize(20);
		plusText.setFillColor(sf::Color::Black);
		plusText.setPosition(
			plusButton.getPosition().x + (plusButton.getSize().x - plusText.getLocalBounds().width) / 2,
			plusButton.getPosition().y + (plusButton.getSize().y - plusText.getLocalBounds().height) / 2 - 5
		);

		sf::RectangleShape Button1(sf::Vector2f(40, 20));
		Button1.setPosition(400, 600);
		Button1.setFillColor(sf::Color::White);

		sf::Text Text1;
		Text1.setString("1");
		Text1.setFont(font1);
		Text1.setCharacterSize(25);
		Text1.setFillColor(sf::Color::Black);
		Text1.setPosition(
			Button1.getPosition().x + (Button1.getSize().x - Text1.getLocalBounds().width) / 2,
			Button1.getPosition().y + (Button1.getSize().y - Text1.getLocalBounds().height) / 2 - 5
		);
		sf::RectangleShape Button2(sf::Vector2f(40, 20));
		Button2.setPosition(450, 600);
		Button2.setFillColor(sf::Color::White);

		sf::Text Text2;
		Text2.setString("2");
		Text2.setFont(font1);
		Text2.setCharacterSize(20);
		Text2.setFillColor(sf::Color::Black);
		Text2.setPosition(
			Button2.getPosition().x + (Button2.getSize().x - Text2.getLocalBounds().width) / 2,
			Button2.getPosition().y + (Button2.getSize().y - Text2.getLocalBounds().height) / 2 - 5
		);

		//Welcome screen
		sf::Text welcomeMessage;
		welcomeMessage.setCharacterSize((HEIGHT * 0.07 * 0.40));
		welcomeMessage.setFillColor(sf::Color::White);
		welcomeMessage.setPosition(WIDTH * 0.21, HEIGHT * 0.815);

		while (window.isOpen())
		{
			sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
			while (window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					window.close();
				if (minusButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					minusButton.setFillColor(sf::Color::Green); 
					if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
						minusButton.setFillColor(sf::Color::Red); 
						std::cout << "-\n";
						size.setCharacterSize(size.getCharacterSize() - 3);
					}
				}
				else {
					minusButton.setFillColor(sf::Color::White);
				}

				if (plusButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					plusButton.setFillColor(sf::Color::Green); 
					if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
						plusButton.setFillColor(sf::Color::Red);
						std::cout << "+\n";
						size.setCharacterSize(size.getCharacterSize() + 3);
						
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

			window.clear();
			if (inputString == "") formula.setString("");
			formulaDraw.printFormula(formula.getString(), window);
			window.draw(minusButton);
			window.draw(minusText);
			window.draw(plusButton);
			window.draw(plusText);
			window.draw(Button1);
			window.draw(Text1);
			window.draw(Button2);
			window.draw(Text2);
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
