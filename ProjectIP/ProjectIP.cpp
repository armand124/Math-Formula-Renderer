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
	"5", "6", "-", ")", "cos", "arccos", "7", "8", "9", ",", "ln", "tg", "arctg",
	"*", "0", "/", "^", "sqrt", "ctg", "arcctg"};
	float charSize = 40.f;
	bool light = 0;
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
		bool changed = false;
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
		
		sf::RectangleShape darkmodeButton(sf::Vector2f(40, 20));
		darkmodeButton.setPosition(WIDTH*0.97, 10);
		darkmodeButton.setFillColor(sf::Color::White);

		sf::Text darkmodeText;
		darkmodeText.setString("DARK");
		darkmodeText.setFont(generalFont);
		darkmodeText.setCharacterSize(10);
		darkmodeText.setFillColor(sf::Color::Black);
		darkmodeText.setPosition(
			darkmodeButton.getPosition().x + (darkmodeButton.getSize().x - darkmodeText.getLocalBounds().width) / 2,
			darkmodeButton.getPosition().y + (darkmodeButton.getSize().y - darkmodeText.getLocalBounds().height) / 2 - 5
		);
		sf::RectangleShape lightmodeButton(sf::Vector2f(40, 20));
		lightmodeButton.setPosition(WIDTH*0.97-50, 10);
		lightmodeButton.setFillColor(sf::Color::White);

		sf::Text lightmodeText;
		lightmodeText.setString("LIGHT");
		lightmodeText.setFont(generalFont);
		lightmodeText.setCharacterSize(10);
		lightmodeText.setFillColor(sf::Color::Black);
		lightmodeText.setPosition(
			lightmodeButton.getPosition().x + (lightmodeButton.getSize().x - lightmodeText.getLocalBounds().width) / 2,
			lightmodeButton.getPosition().y + (lightmodeButton.getSize().y - lightmodeText.getLocalBounds().height) / 2 - 5
		);

		sf::RectangleShape backspaceButton(sf::Vector2f(51, 51));
		backspaceButton.setPosition(WIDTH*0.81, HEIGHT*0.5);
		backspaceButton.setFillColor(sf::Color::White);

		sf::Text backspaceText;
		backspaceText.setString("<");
		backspaceText.setFont(generalFont);
		backspaceText.setCharacterSize(30);
		backspaceText.setFillColor(sf::Color::Black);
		backspaceText.setPosition(
			backspaceButton.getPosition().x + (backspaceButton.getSize().x - backspaceText.getLocalBounds().width) / 2,
			backspaceButton.getPosition().y + (backspaceButton.getSize().y - backspaceText.getLocalBounds().height) / 2 - 10
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


		
		while (window.isOpen())
		{
			sf::Vector2i mousePosition = sf::Mouse::getPosition(window);
			while (window.pollEvent(event))
			{
				changed = false;
				if (event.type == sf::Event::Closed)
					window.close();
				if (darkmodeButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					darkmodeButton.setFillColor(sf::Color::Green);
					if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
						darkmodeButton.setFillColor(sf::Color::Red);
						light = 0;
						backspaceButton.setFillColor(sf::Color::White);
						backspaceText.setFillColor(sf::Color::Black);
						formula.setFillColor(sf::Color::White);
						minusButton.setFillColor(sf::Color::White);
						minusText.setFillColor(sf::Color::Black);
						plusButton.setFillColor(sf::Color::White);
						plusText.setFillColor(sf::Color::Black);
						lightmodeButton.setFillColor(sf::Color::White);
						lightmodeText.setFillColor(sf::Color::Black);
						darkmodeButton.setFillColor(sf::Color::White);
						darkmodeText.setFillColor(sf::Color::Black);
						for (int i = 0;i < 28;i++) {
							buttons[i].bk.setFillColor(sf::Color::White);
							buttons[i].txt.setFillColor(sf::Color::Black);
						}
					}
				}
				else {
						if (light == 1)
							darkmodeButton.setFillColor(sf::Color::Black);
						else
							darkmodeButton.setFillColor(sf::Color::White);
					}	

				if (lightmodeButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					lightmodeButton.setFillColor(sf::Color::Green);
					if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
						lightmodeButton.setFillColor(sf::Color::Red);
						light = 1;
						backspaceButton.setFillColor(sf::Color::Black);
						backspaceText.setFillColor(sf::Color::White);
						formula.setFillColor(sf::Color::Black);
						minusButton.setFillColor(sf::Color::Black);
						minusText.setFillColor(sf::Color::White);
						plusButton.setFillColor(sf::Color::Black);
						plusText.setFillColor(sf::Color::White);
						lightmodeButton.setFillColor(sf::Color::Black);
						lightmodeText.setFillColor(sf::Color::White);
						darkmodeButton.setFillColor(sf::Color::Black);
						darkmodeText.setFillColor(sf::Color::White);
						for (int i = 0;i < 28;i++) { 
							buttons[i].bk.setFillColor(sf::Color::Black);
						buttons[i].txt.setFillColor(sf::Color::White);
						}
					}
				}
				else {
					if (light == 1)
						lightmodeButton.setFillColor(sf::Color::Black);
					else
						lightmodeButton.setFillColor(sf::Color::White);
				}
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
							changed = true;
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
						if (light == 0)
							buttons[i].bk.setFillColor(sf::Color::White);
						else
							buttons[i].bk.setFillColor(sf::Color::Black);
					}
				}
				if (minusButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					minusButton.setFillColor(sf::Color::Green); 
					if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
						minusButton.setFillColor(sf::Color::Red); 
						charSize = std::max(charSize - 4, 22.f);
						changed = true;
					}
				}
				else {
					changed = true;
					if (light == 0)
						minusButton.setFillColor(sf::Color::White);
					else
						minusButton.setFillColor(sf::Color::Black);
				}

				if (backspaceButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					backspaceButton.setFillColor(sf::Color::Green);
					if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
						backspaceButton.setFillColor(sf::Color::Red);
						if (!inputString.empty()) {
							inputString.pop_back();
							inputBoxText.setString(inputString);
							formula.setString(inputString);
							changed = true;
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
				}
				else {
					if (light == 0)
						backspaceButton.setFillColor(sf::Color::White);
					else
						backspaceButton.setFillColor(sf::Color::Black);
				}

				if (plusButton.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePosition))) {
					plusButton.setFillColor(sf::Color::Green);
					if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
						plusButton.setFillColor(sf::Color::Red);
						changed = true;
						charSize = std::min(charSize + 4, 60.f);
					}
				}
				else {
					changed = true;
					if (light == 0)
						plusButton.setFillColor(sf::Color::White);
					else
						plusButton.setFillColor(sf::Color::Black);
				}
				if (event.type == sf::Event::TextEntered)
				{
					if (event.text.unicode == 13 || event.text.unicode == 9 || event.text.unicode == 27 || event.text.unicode == 32)
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
					changed = true;
				}
			}

			//-----------------------------------------------
		
			if (inputString == "") formula.setString("");
			if (inputString != "" && !pars.isStringValid(inputString)) warningMessage.setString("Formula este gresita!");
			else warningMessage.setString("");
			if (changed || !startedTyping) {
				if (light == 1)
					window.clear(sf::Color::White);
				else
					window.clear(sf::Color::Black);
				formulaDraw.printFormula(formula.getString(), window, charSize, light);
				window.draw(minusButton);
				window.draw(minusText);
				window.draw(plusButton);
				window.draw(plusText);
				window.draw(lightmodeButton);
				window.draw(lightmodeText);
				window.draw(darkmodeButton);
				window.draw(darkmodeText);
				window.draw(backspaceButton);
				window.draw(backspaceText);
				for (int i = 0;i < 28;i++) { window.draw(buttons[i].bk); window.draw(buttons[i].txt); }
				window.draw(warningMessage);
				window.draw(inputBoxFrame);
				window.draw(inputBoxText);
			//----------------------------------------
			}
			window.display();
		}
	}
};

int main()
{
	Gui app("Vizualizator Formule", 1920, 1080);
	app.run();
}
