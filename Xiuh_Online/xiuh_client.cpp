
//
// Disclaimer:
// ----------
//
// This code will work only if you selected window, graphics and audio.
//
// Note that the "Run Script" build phase will copy the required frameworks
// or dylibs to your application bundle so you can execute it on any OS X
// computer.
//
// Your resource files (images, sounds, fonts, ...) are also copied to your
// application bundle. To get the path to these resources, use the helper
// function `resourcePath()` from ResourcePath.hpp
//

#include "xiuh_client.hpp"
// Here is a small helper for you! Have a look.

int main(int, char const**)
{
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
    
    Menu menu(window.getSize().x, window.getSize().y);
   
    // Set the Icon
    sf::Image icon;
    if (!icon.loadFromFile("resources/icon.png")) {
        return EXIT_FAILURE;
    }

    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());

    // Load a sprite to display
    sf::Texture texture;
    if (!texture.loadFromFile("resources/cute_image.jpg")) {
        return EXIT_FAILURE;
    }
    sf::Sprite sprite(texture);

    // Create a graphical text to display
    sf::Font font;
    if (!font.loadFromFile("resources/sansation.ttf")) {
        return EXIT_FAILURE;
    }
    sf::Text text("Hello SFML", font, 50);
    text.setFillColor(sf::Color::Black);
    
    window.setKeyRepeatEnabled(true);
    
    //Rectangle
    sf::RectangleShape rectangle(sf::Vector2f(50, 50));
    
    // change the size to 100x100
    //rectangle.setSize(sf::Vector2f(100, 100));
    
    rectangle.setFillColor(sf::Color::Black);

    float posX, posY;
    posX = 0;
    posY = 550;
    sf::Vector2f posR;
    posR = rectangle.getPosition();    
    rectangle.setPosition(posX, posY);
    
    
    std::cout << posR.x << std::endl;
    
    // Load a music to play
    sf::Music music;
    if (!music.openFromFile("resources/nice_music.ogg")) {
        return EXIT_FAILURE;
    }
    
    // Play the music
    //music.play();
    
    // Start the game loop
    while (window.isOpen())
    {
        // Process events
        sf::Event event;
        while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::KeyReleased:
				switch (event.key.code)
				{
				case sf::Keyboard::Up:
					menu.MoveUp();
					break;

				case sf::Keyboard::Down:
					menu.MoveDown();
					break;

				case sf::Keyboard::Return:
					switch (menu.GetPressedItem())
					{
					case 0:
						std::cout << "Play button has been pressed" << std::endl;
                        
						break;
					case 1:
						std::cout << "Option button has been pressed" << std::endl;
						break;
					case 2:
						window.close();
						break;
					}

					break;
				}

				break;
			case sf::Event::Closed:
				window.close();

				break;

			}
		}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && posR.x < 750)
        {
            posR = rectangle.getPosition();
            //rectangle.setPosition(posX, posY);
            rectangle.move(0.5, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && posR.x > 0 )
        {
            posR = rectangle.getPosition();
            //rectangle.setPosition(posX, posY);
            rectangle.move(-0.5, 0);
        }
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
            }
            
            
        }        
        // Clear screen
        window.clear();
        /*
        // Draw the sprite
        window.draw(sprite);

        // Draw the string
        window.draw(text);
        
        window.draw(rectangle);
        */
        menu.draw(window);
        // Update the window
        window.display();
        
    }

    return EXIT_SUCCESS;
}
