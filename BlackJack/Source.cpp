#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <thread>
#include <vector>
using namespace std;

class Card {
  public:
	int value;
	string suit, strValue, colour;
	void setValues(int a, string c, string s, string v) {
		value = a;
		colour = c;
		suit = s;
		strValue = v;
	}
	void setValues(int a) {
		value = a;
	}
	string printName() { return strValue + " of " + suit; }
};

string wordify(int value) {
	int number[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
	string strValue[13] = {"Ace", "Two", "Three", "Four", "Five", "Six", "Seven", "Eight", "Nine", "Ten", "Jack", "Queen", "King"};

	return strValue[value - 1];
}

Card * buildDeck(Card deck[]) {
	Card * card;
	string * strValue = new string;
	string * strSuit = new string;
	string * colour = new string;
	int * counter = new int;
	(*counter) = 0;

	for(unsigned i = 1; i < 5; i++) {
		switch(i) {
		case 1:
			*strSuit = "Spades";
			*colour = "Black";
			break;
		case 2:
			*strSuit = "Hearts";
			*colour = "Red";
			break;
		case 3:
			*strSuit = "Clubs";
			*colour = "Black";
			break;
		case 4:
			*strSuit = "Diamonds";
			*colour = "Red";
			break;
		}
		for(unsigned j = 1; j < 14; j++) {
			card = new Card;
			int * value = new int;
			*strValue = wordify(j);
			if(*strValue == "Jack" || *strValue == "Queen" || *strValue == "King") {
				*value = 10;
			}
			else if(*strValue == "Ace") {
				*value = 11;
			}
			else {
				*value = j;
			}
			card->setValues(*value, *colour, *strSuit, *strValue);
			deck[*counter] = *card;
			(*counter)++;
			delete card;
		}
	}
	delete colour;
	delete counter;
	delete strSuit;
	delete strValue;

	return deck;
}

vector<Card> * resetDeck(Card deck[]) {
	vector<Card> * vDeck = new vector < Card >;
	for(unsigned i = 0; i < 52; i++) {
		(*vDeck).push_back(deck[i]);
	}
	return vDeck;
}

int * calculateHandValue(vector<Card> * hand, int * score) {
	*score = 0;
	for(unsigned i = 0; i < (*hand).size(); i++) {
		*score += (*hand).at(i).value;
	}
	if(*score > 21) {
		// When Aces are first dealt to hands, they have a value of 11, and this has the
		// unintended behaviour of making hands with Aces evaluate to >21. This statement
		// checks whether there are any cards with a value of 11 (the initial value of a
		// dealt Ace) in a hand with a total score of >21, and change its value to 1.
		bool * aceChanged = new bool;
		*aceChanged = false;
		for(unsigned i = 0; i < (*hand).size(); i++) {
			if(!(*aceChanged) && (*hand).at(i).value == 11) {
				(*hand).at(i).setValues(1);
				*aceChanged = true;
			}
		}
		if(*aceChanged) {
			// If an Ace's value has been changed, recalculate the hand's total score
			*score = 0;
			for(unsigned i = 0; i < (*hand).size(); i++) {
				*score += (*hand).at(i).value;
			}
		}
		delete aceChanged;
	}
	return score;
}

void dealCard(vector<Card> * deck, Card * card, std::mt19937 * rng, vector<Card> * hand, int * houseInitScore = 0, int cardDealNum = 1) {
	std::uniform_int_distribution<std::mt19937::result_type> distDeck(0, (*deck).size() - 1);
	*card = (*deck).at(distDeck(*rng));
	(*hand).push_back(*card);
	if(cardDealNum == 0) {
		*houseInitScore += (*card).value;
	}
	for(unsigned i = 0; i < (*deck).size(); i++) {
		if((*deck).at(i).printName() == (*card).printName()) {
			(*deck).erase((*deck).begin() + i);
		}
	}
}

void printPlayerHand(vector<Card> * hand, int * score, int handNumber = 0) {
	// This is to set up for the player splitting their hand
	if(handNumber != 0) {
		std::cout << "Player hand " << handNumber << ": ";
	}
	else {
		std::cout << "Player hand: ";
	}
	for(unsigned i = 0; i < (*hand).size(); i++) {
		if(i != 0) {
			std::cout << ", ";
		}
		std::cout << (*hand).at(i).printName();
	}
	std::cout << " (score: " << *score << ")\n";
}

void printHouseHand(vector<Card> * hand, int * score, int * turn) {
	// Hiding the second House card at the start of a game
	if(*turn == 1) {
		std::cout << "House hand: " << (*hand).at(0).printName();
		std::cout << ", ??? (score: " << *score << ")\n";
	}
	else {
		std::cout << "House hand: ";
		for(unsigned i = 0; i < (*hand).size(); i++) {
			if(i != 0) {
				std::cout << ", ";
			}
			std::cout << (*hand).at(i).printName();
		}
		std::cout << " (score: " << *score << ")\n";
	}
}

void playBlackJack(Card deck[]) {
	Card * chosenCard = new Card;
	vector<Card> * houseHand = new vector<Card>;
	vector<Card> * playerHand = new vector<Card>;
	vector<Card> * playerFirstHand = new vector<Card>;
	vector<Card> * playerSecondHand = new vector<Card>;
	vector<Card> * vDeck = resetDeck(deck);
	char * option = new char;
	int * houseInitScore = new int; // Score of the first card in House's hand
	int * houseScore = new int;
	int * numHands = new int;
	int * playerScore = new int;
	int * playerFirstHandScore = new int;
	int * playerSecondHandScore = new int;
	int * turn = new int;
	int * waitTime = new int;

	bool * isHouseTurn = new bool;
	bool * isPlayerTurn = new bool;
	bool * playerHasSplit = new bool;
	bool * selectDouble = new bool;

	vector<Card> ** currentHand; // Pointer containing the current hand in play
	int ** currentScore; // Pointer containing the score of the current hand in play

	std::mt19937 * rng = new mt19937;
	(*rng).seed(std::random_device()());

	*numHands = 1;
	*turn = 1;
	*waitTime = 1000;
	*isHouseTurn = false;
	*isPlayerTurn = true;
	*selectDouble = false;
	*playerHasSplit = false;
	*houseScore = *houseInitScore = *playerScore = 0;

	// Gameplay
	// Deal cards and calculate values
	std::cout << "--------------------------------------------------------------------------\n\n";
	for(unsigned i = 0; i < 4; i++) {
		if(i % 2 == 0) {
			dealCard(vDeck, chosenCard, rng, houseHand, houseInitScore, i);
		}
		else {
			dealCard(vDeck, chosenCard, rng, playerHand);
		}
	}
	houseScore = calculateHandValue(houseHand, houseScore);
	playerScore = calculateHandValue(playerHand, playerScore);
	printHouseHand(houseHand, houseInitScore, turn);
	printPlayerHand(playerHand, playerScore);
	std::cout << '\n';

	// Setting up pointers to pointer. This is a change to accommodate dealing to split hands
	currentHand = &playerHand;
	currentScore = &playerScore;

	// Player's first turn
	while(*turn == 1 && isPlayerTurn) {
		if(*playerScore < 21) {
			std::cout << "Select action\n" << "([H]it, S[t]and, [D]ouble";
			if((**currentHand).at(0).strValue == (**currentHand).at(1).strValue || (**currentHand).at(0).value >= 10 && (**currentHand).at(1).value >= 10) {
				std::cout << ", S[p]lit";
			}
			std::cout << "): ";
			std::cin >> *option;
			std::cout << '\n';

			switch(*option) {
			case 'H':
			case 'h':
				std::cout << "Player hits.\n\n";
				dealCard(vDeck, chosenCard, rng, *currentHand);
				playerScore = calculateHandValue(*currentHand, *currentScore);
				printPlayerHand(*currentHand, *currentScore);
				(*turn)++;
				break;
			case 'T':
			case 't':
				std::cout << "Player stands.\n\n";
				*isPlayerTurn = false;
				*isHouseTurn = true;
				(*turn)++;
				break;
			case 'D':
			case 'd':
				std::cout << "Player doubles up.\n\n";
				*selectDouble = true;
				dealCard(vDeck, chosenCard, rng, *currentHand);
				playerScore = calculateHandValue(*currentHand, *currentScore);
				printPlayerHand(*currentHand, *currentScore);
				*isPlayerTurn = false;
				*isHouseTurn = true;
				(*turn)++;
				break;
			case 'P':
			case 'p':
				if((**currentHand).at(0).strValue == (**currentHand).at(1).strValue || (**currentHand).at(0).value >= 10 && (**currentHand).at(1).value >= 10) {
					std::cout << "Player splits.\n\n";
					*numHands = 2;
					*playerHasSplit = true;
					(*playerFirstHand).push_back((**currentHand).at(0));
					(*playerSecondHand).push_back((**currentHand).at(1));
					// Revert any changes made to the value of Aces in the event of being dealt two Aces
					// (see line 101 for information)
					if((*playerFirstHand).at(0).value == 1) {
						(*playerFirstHand).at(0).setValues(11);
					}
					if((*playerSecondHand).at(0).value == 1) {
						(*playerSecondHand).at(0).setValues(11);
					}
					dealCard(vDeck, chosenCard, rng, playerFirstHand);
					dealCard(vDeck, chosenCard, rng, playerSecondHand);
					calculateHandValue(playerFirstHand, playerFirstHandScore);
					calculateHandValue(playerSecondHand, playerSecondHandScore);
					printPlayerHand(playerFirstHand, playerFirstHandScore, 1);
					printPlayerHand(playerSecondHand, playerSecondHandScore, 2);
					(*turn)++;
					break;
				}
				else {
					std::cout << "Cannot split\n";
				}
			default:
				std::cout << "Invalid option, choose again\n";
				break;
			}
		}
		else { // Automatically switch to house's turn if player has 21
			*isPlayerTurn = false;
			*isHouseTurn = true;
			(*turn)++;
		}
		std::cout << '\n';
	}

	while(*turn > 1 && **currentScore < 21 && *isPlayerTurn) {
		std::cout << "Select action:\n([H]it, S[t]and)\n";
		std::cin >> *option;
		std::cout << '\n';
		switch(*option) {
		case 'H':
		case 'h':
			std::cout << "Player hits.\n\n";
			dealCard(vDeck, chosenCard, rng, *currentHand);
			playerScore = calculateHandValue(*currentHand, *currentScore);
			printPlayerHand(*currentHand, *currentScore);
			(*turn)++;
			break;
		case 'T':
		case 't':
			std::cout << "Player stands.\n\n";
			*isPlayerTurn = false;
			*isHouseTurn = true;
			(*turn)++;
			break;
		default:
			std::cout << "Invalid option, choose again\n";
			break;
		}
		std::cout << '\n';
	}

	// House turn
	if(**currentScore > 21) {
		std::cout << "PLAYER BUST!!\nHouse wins!!\n\n";
	}
	else {
		*isPlayerTurn = false;
		*isHouseTurn = true;
		houseScore = calculateHandValue(houseHand, houseScore);
		printHouseHand(houseHand, houseScore, turn);
		std::cout << "\n";
		while(*isHouseTurn && **currentScore <= 21) {
			if(*houseScore > **currentScore) {
				if(*houseScore <= 21) {
					std::cout << "House wins!!\n\n";
					*isHouseTurn = false;
				}
				else {
					std::cout << "HOUSE BUST!!\nPlayer wins!!\n\n";
					*isHouseTurn = false;
				}
			}
			else if(*houseScore == **currentScore) {
				std::cout << "Push\n\n";
				*isHouseTurn = false;
			}
			else {
				std::this_thread::sleep_for(std::chrono::milliseconds(*waitTime));
				std::cout << "House hits.\n\n";
				dealCard(vDeck, chosenCard, rng, houseHand);
				houseScore = calculateHandValue(houseHand, houseScore);
				printHouseHand(houseHand, houseScore, turn);
				(*turn)++;
			}
			std::cout << '\n';
		}
	}

	// Debug listing remaining cards in deck
	/*for(unsigned i = 0; i < (*vDeck).size(); i++) {
	std::cout << (*vDeck).at(i).printName() << '\n';
	}*/

	delete chosenCard;
	delete houseHand;
	delete houseInitScore;
	delete houseScore;
	delete isHouseTurn;
	delete isPlayerTurn;
	delete numHands;
	delete option;
	delete playerHand;
	delete playerFirstHand;
	delete playerSecondHand;
	delete playerHasSplit;
	delete playerScore;
	delete playerFirstHandScore;
	delete playerSecondHandScore;
	delete rng;
	delete selectDouble;
	delete turn;
	delete vDeck;
	delete waitTime;
}

bool playAgain() {
	char * option = new char;
	while(0 != 1) {
		std::cout << "Play again? ([Y]es / [N]o): ";
		std::cin >> *option;

		switch(*option) {
		case 'Y':
		case 'y':
			std::cout << "\n\n";
			delete option;
			return true;
			break;
		case 'N':
		case 'n':
			delete option;
			return false;
			break;
		default:
			std::cout << "Invalid option\n\n";
			break;
		}
	}
}

int main() {
	Card deck[52]; // Default deck
	Card * pDeck = buildDeck(deck); // Dynamic deck. This will be used as cards are taken from the deck
	bool * playGame = new bool;
	*playGame = true;
	while(*playGame) {
		playBlackJack(pDeck);
		*playGame = playAgain();
	}
	delete playGame;
	return 0;
}