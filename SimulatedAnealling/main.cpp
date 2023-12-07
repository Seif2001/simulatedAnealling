#include <SFML/Graphics.hpp>
#include "simulatedAnealling.hpp"
#include<windows.h>
using namespace std;

int main()
{
    // Create a window

    Placer p = makePlacer("C:\\Users\\elsha\\Desktop\\IC\\SimulatedAnealling\\d1.txt");
    makeCore(p);
    placeRandomly(p);
    initializeTemp(p);
    int** grid = getGrid(p);
    int gridSize = 15;
    int desiredWidth = p.ny * gridSize;
    int desiredHeight = p.nx * gridSize;
    cout << desiredHeight;
    if (desiredWidth < 800 || desiredHeight < 800) {
        gridSize *= std::max(800.0 / desiredWidth, 800.0 / desiredHeight);

    }
    const int tempBarH = gridSize / 3;

    desiredWidth = p.ny * gridSize;
    desiredHeight = p.nx * gridSize + tempBarH;

    sf::RenderWindow window(sf::VideoMode(desiredWidth, desiredHeight), "SFML Grid");


    // Main loop
    double temp = p.initialTemp;
    cout << "Initial temp: " << temp << endl;
    int hpw;

    sf::Font font;
    if (!font.loadFromFile("OpenSans-Bold.ttf")) {
        std::cerr << "Error loading font" << std::endl;
        return 1; // Return an error code
    }
    while (window.isOpen())
    {

        // Handle events
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // Clear the window

        if (temp > p.finalTemp) {
            window.clear();
            sf::RectangleShape tempBar(sf::Vector2f(p.ny * gridSize, tempBarH));
            tempBar.setPosition(0, 0);
            tempBar.setOutlineColor(sf::Color::Black);
            tempBar.setFillColor(sf::Color::White);
            tempBar.setOutlineThickness(3.0f);
            window.draw(tempBar);

            sf::RectangleShape tempNow(sf::Vector2f(((double)(temp / p.initialTemp) * (p.ny * gridSize)), tempBarH));

            tempNow.setPosition(0, 0);
            tempNow.setOutlineColor(sf::Color::Black);
            tempNow.setFillColor(sf::Color::Red);
            tempNow.setOutlineThickness(3.0f);
            window.draw(tempNow);


            sf::Text textTemp;
            textTemp.setFont(font);
            textTemp.setString(std::to_string(temp) + " °C");
            textTemp.setFillColor(sf::Color::Black);
            textTemp.setCharacterSize(tempBarH / 2);
            textTemp.setOrigin(0, 0);

            // Center the text within the cell
            sf::FloatRect textRect = textTemp.getLocalBounds();
            textTemp.setPosition((p.ny / 2.0f) * gridSize - gridSize / 2, tempBarH / 4);
            window.draw(textTemp);




            grid = getGrid(p);
            for (int i = 0; i < p.nx; i++) {
                for (int j = 0; j < p.ny; j++) {
                    sf::RectangleShape cell(sf::Vector2f(gridSize, gridSize));
                    cell.setPosition(j * gridSize, (i * gridSize) + tempBarH);
                    cell.setFillColor(grid[i][j] == -1 ? sf::Color::Black : sf::Color::Blue);
                    cell.setOutlineThickness(1.f);
                    cell.setOutlineColor(sf::Color::Black);
                    window.draw(cell);

                    sf::Text text;
                    text.setFont(font);
                    text.setString(grid[i][j] == -1 ? "" : std::to_string(grid[i][j]));  // Convert the number to a string
                    text.setCharacterSize(gridSize / 2);

                    // Center the text within the cell
                    sf::FloatRect textRect = text.getLocalBounds();
                    text.setOrigin(textRect.left + textRect.width / 2.0f, (textRect.top + textRect.height / 2.0f));
                    text.setPosition(j * gridSize + gridSize / 2.0f, (i * gridSize + gridSize / 2.0f) + tempBarH);

                    // Draw the text
                    window.draw(text);
                }
            }


            hpw = hpwl(p);
            std::cout << hpwl(p) << endl;
            float hpW = (gridSize / 1.5f) < 50 ? 150 : (gridSize / 1.5f);
            float hpH = (gridSize / 4) < 50 ? 80 : (gridSize / 4);
            cout << "HPE" << hpW;

            sf::RectangleShape hpwlRect(sf::Vector2f(hpW, hpH));

            hpwlRect.setPosition(desiredWidth - hpW, desiredHeight - hpH);
            hpwlRect.setFillColor(sf::Color(255, 255, 255, 128));
            window.draw(hpwlRect);

            sf::Text textHpwl;
            textHpwl.setFont(font);
            textHpwl.setString(std::to_string(hpw));
            textHpwl.setFillColor(sf::Color::Black);
            textHpwl.setCharacterSize(hpW / 3);
            textHpwl.setOrigin(0, 0);

            // Center the text within the cell
            textHpwl.setPosition(desiredWidth - hpW, desiredHeight - hpH);
            window.draw(textHpwl);

            window.display();

            sf::sleep(sf::milliseconds((1000 / temp) < 50 ? (1000 / temp) : 50));
            simulatedAnealing(p, temp);
            temp = temp * COOLING_RATE;
        }

        sf::RectangleShape tempBar(sf::Vector2f(p.ny * gridSize, tempBarH));
        tempBar.setPosition(0, 0);
        tempBar.setOutlineColor(sf::Color::Black);
        tempBar.setFillColor(sf::Color::White);
        tempBar.setOutlineThickness(3.0f);
        window.draw(tempBar);

        sf::RectangleShape tempNow(sf::Vector2f(((double)(temp / p.initialTemp) * (p.ny * gridSize)) * 10, tempBarH));

        tempNow.setPosition(0, 0);
        tempNow.setOutlineColor(sf::Color::Black);
        tempNow.setFillColor(sf::Color::Red);
        tempNow.setOutlineThickness(3.0f);
        window.draw(tempNow);


        sf::Text textTemp;
        textTemp.setFont(font);
        textTemp.setString(std::to_string(temp) + " °C");
        textTemp.setFillColor(sf::Color::Black);
        textTemp.setCharacterSize(tempBarH / 2);
        textTemp.setOrigin(0, 0);

        // Center the text within the cell
        sf::FloatRect textRect = textTemp.getLocalBounds();
        textTemp.setPosition((p.ny / 2.0f) * gridSize - gridSize / 2, tempBarH / 4);
        window.draw(textTemp);


        for (int i = 0; i < p.nx; i++) {
            for (int j = 0; j < p.ny; j++) {
                sf::RectangleShape cell(sf::Vector2f(gridSize, gridSize));
                cell.setPosition(j * gridSize, (i * gridSize) + tempBarH);
                cell.setFillColor(grid[i][j] == -1 ? sf::Color::Black : sf::Color::Blue);
                cell.setOutlineThickness(1.f);
                cell.setOutlineColor(sf::Color::Black);
                window.draw(cell);

                sf::Text text;
                text.setFont(font);
                text.setString(grid[i][j] == -1 ? "" : std::to_string(grid[i][j]));  // Convert the number to a string
                text.setCharacterSize(gridSize / 2);

                // Center the text within the cell
                sf::FloatRect textRect = text.getLocalBounds();
                text.setOrigin(textRect.left + textRect.width / 2.0f, (textRect.top + textRect.height / 2.0f));
                text.setPosition(j * gridSize + gridSize / 2.0f, (i * gridSize + gridSize / 2.0f) + tempBarH);

                // Draw the text
                window.draw(text);
            }


        }

        hpw = hpwl(p);
        std::cout << hpwl(p) << endl;
        float hpW = (gridSize / 1.5f) < 20 ? 30 : (gridSize / 1.5f);
        float hpH = (gridSize / 4) < 20 ? 30 : (gridSize / 4);
        sf::RectangleShape hpwlRect(sf::Vector2f(hpW, hpH));

        hpwlRect.setPosition(desiredWidth - hpW / 1.5f, desiredHeight - hpW / 4);
        hpwlRect.setOutlineColor(sf::Color::Black);
        hpwlRect.setFillColor(sf::Color(255, 255, 255, 128));
        hpwlRect.setOutlineThickness(3.0f);
        window.draw(hpwlRect);

        sf::Text textHpwl;
        textHpwl.setFont(font);
        textHpwl.setString("HPWL: " + std::to_string(hpw));
        textHpwl.setFillColor(sf::Color::Black);
        textHpwl.setCharacterSize(gridSize / 10);
        textHpwl.setOrigin(0, 0);
        window.draw(textHpwl);

        window.display();



        // Display the content
    }

    return 0;
}


