
#include "server.hpp"
#include <ctime>
#include <cmath>


std::string getCurrentTime() 
{
    std::time_t currentTime = std::time(NULL);
    return std::ctime(&currentTime);
}

std::string flipCoin()
{
    std::srand(static_cast<unsigned int>(std::time(0)));
    int result = std::rand() % 2;
    return (result == 0) ? "Heads" : "Tails";
}

std::string getTrivia()
{
    std::vector<std::string> triviaStatements;
    triviaStatements.push_back("The Eiffel Tower can be 15 cm taller during the summer.");
    triviaStatements.push_back("Honey never spoils.");
    triviaStatements.push_back("The shortest war in history was between Britain and Zanzibar on August 27, 1896. Zanzibar surrendered after 38 minutes.");
    triviaStatements.push_back("A group of flamingos is called a 'flamboyance.'");
    triviaStatements.push_back("The world's largest desert is Antarctica.");
    triviaStatements.push_back("Bananas are berries, but strawberries aren't.");
    triviaStatements.push_back("Octopuses have three hearts.");
    triviaStatements.push_back("The longest recorded flight of a chicken is 13 seconds.");
    triviaStatements.push_back("Cows have best friends.");
    triviaStatements.push_back("A group of owls is called a 'parliament'.");
    triviaStatements.push_back("The average person will spend six months of their life waiting for red lights to turn green.");
    triviaStatements.push_back("A 'jiffy' is an actual unit of time: 1/100th of a second.");
    triviaStatements.push_back("The Great Wall of China is not visible from the moon without aid.");
    triviaStatements.push_back("A newborn kangaroo is the size of a lima bean.");
    triviaStatements.push_back("The longest word in the English language without a vowel is 'rhythms.'");
    triviaStatements.push_back("There are more possible iterations of a game of chess than there are atoms in the known universe.");
    triviaStatements.push_back("Honeybees can recognize human faces.");
    triviaStatements.push_back("Penguins only have one mate their entire life.");
    triviaStatements.push_back("A 'butt' was a medieval unit of measure for wine.");
    triviaStatements.push_back("The shortest war in history was between Britain and Zanzibar on August 27, 1896. Zanzibar surrendered after 38 minutes.");

    std::srand(std::time(0));
    int randomIndex = std::rand() % triviaStatements.size();
    return triviaStatements[randomIndex];
}

void server::mybot(int fd, const std::string& command, std::string channelstr) 
{

    std::string message;
    std::vector<channel>::iterator currentchannel = selectChannel(channelstr);

    if (command == "!bot\r\n")
    {
        if (!currentchannel->getIsConnected(-1))//changer l'index de base de MrRobot?
            currentchannel->setUserConnect(MrRobot);//attention au channel privé + limite d'user
        currentchannel->sendToChannel(*MrRobot,"I am MrRobot, your friendly IRC bot! type \"!bot help\" to discover my commands!");
    }
    else if (!currentchannel->getIsConnected(-1))
    {
        currentchannel->sendToChannel(*MrRobot, "MrRobot is not present in this channel, use !bot to invite him."); //envoyer le message via notice?
        return;
    }
    else if (command == "!bot help\r\n")
    	currentchannel->sendToChannel(*MrRobot,"Available commands: help | time | trivia | coinflip | thanks");
    else if (command == "!bot time\r\n")
    	currentchannel->sendToChannel(*MrRobot,"Current time is " + getCurrentTime());
    else if (command == "!bot trivia\r\n")
    	currentchannel->sendToChannel(*MrRobot, getTrivia());
    else if (command == "!bot coinflip\r\n")
    	currentchannel->sendToChannel(*MrRobot, flipCoin());
    else if (command == "!bot thanks\r\n")
    	currentchannel->sendToChannel(*MrRobot, "Cyprien, Sandra, and Théo thank you for using their IRC server!");
    else
    	currentchannel->sendToChannel(*MrRobot, "Sorry i don't know this command. Type \"!bot help\" to know my commands");
}


/*
pas oublier de delete et new mr robot (comme admin)
*/

// dans  server.hpp


//**********************************/BOT//**********************************/

	// void mybot(int fd, const std::string& command);


// dans commande.cpp

	// //gestion du bot
	// if (commandText.length() > 7)
	// {
	// 	std::string checkbot = commandText.substr(2, 5);
	// 	if (checkbot == "!bot " || checkbot == "!bot\r")
	// 	{
	// 		mybot(fd, commandText.substr(2, commandText.length() - 2), name);
	// 	}
	// }
	// // fin de gestion du bot
