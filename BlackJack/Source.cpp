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

	bool * changePlayerHand = new bool;
	bool * isHouseTurn = new bool;
	bool * isPlayerTurn = new bool;
	bool * playerHasSplit = new bool;
	bool * selectDouble = new bool;

	vector<Card> ** currentHand = new vector<Card> *; // Pointer containing the current hand in play
	int ** currentScore = new int *; // Pointer containing the score of the current hand in play

	std::mt19937 * rng = new mt19937;
	(*rng).seed(std::random_device()());

	*numHands = 1;
	*turn = 1;
	*waitTime = 300;
	*isHouseTurn = false;
	*isPlayerTurn = true;
	*selectDouble = false;
	*playerHasSplit = false;
	*houseScore = *houseInitScore = *playerScore = 0;
	*playerFirstHandScore = *playerSecondHandScore = 0;

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

	// Player's first turn
	while(*turn == 1 && isPlayerTurn) {
		if(*playerScore < 21) {
			std::cout << "Select action\n" << "([H]it, S[t]and, [D]ouble";
			if((*playerHand).at(0).strValue == (*playerHand).at(1).strValue || (*playerHand).at(0).value >= 10 && (*playerHand).at(1).value >= 10) {
				std::cout << ", S[p]lit";
			}
			std::cout << "): ";
			std::cin >> *option;
			std::cout << '\n';

			switch(*option) {
				case 'H':
				case 'h':
					std::cout << "Player hits.\n\n";
					dealCard(vDeck, chosenCard, rng, playerHand);
					playerScore = calculateHandValue(playerHand, playerScore);
					printPlayerHand(playerHand, playerScore);
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
					dealCard(vDeck, chosenCard, rng, playerHand);
					playerScore = calculateHandValue(playerHand, playerScore);
					printPlayerHand(playerHand, playerScore);
					*isPlayerTurn = false;
					*isHouseTurn = true;
					(*turn)++;
					break;
				case 'P':
				case 'p':
					if((*playerHand).at(0).strValue == (*playerHand).at(1).strValue || (*playerHand).at(0).value == (*playerHand).at(1).value) {
						std::cout << "Player splits.\n\n";
						*numHands = 2;
						*playerHasSplit = true;
						(*playerFirstHand).push_back((*playerHand).at(0));
						(*playerSecondHand).push_back((*playerHand).at(1));
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
						playerFirstHandScore = calculateHandValue(playerFirstHand, playerFirstHandScore);
						playerSecondHandScore = calculateHandValue(playerSecondHand, playerSecondHandScore);
						printPlayerHand(playerFirstHand, playerFirstHandScore, 1);
						printPlayerHand(playerSecondHand, playerSecondHandScore, 2);
						(*turn)++;
						break;
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

	for(unsigned i = 0; i < *numHands; i++) {
		*changePlayerHand = false;
		if(*numHands == 2) {
			if(i == 0) {
				currentHand = &playerFirstHand;
				currentScore = &playerFirstHandScore;
			}
			else {
				currentHand = &playerSecondHand;
				currentScore = &playerSecondHandScore;
			}
		}
		else {
			currentHand = &playerHand;
			currentScore = &playerScore;
		}
		while(*turn > 1 && **currentScore < 21 && *isPlayerTurn && !(*changePlayerHand)) {
			if(*numHands == 2) {
				std::cout << "Select action for hand " << i + 1 << ":\n([H]it, S[t]and)\n";
			}
			else {
				std::cout << "Select action:\n([H]it, S[t]and)\n";
			}
			std::cin >> *option;
			std::cout << '\n';
			switch(*option) {
			case 'H':
			case 'h':
				std::cout << "Player hits.\n\n";
				dealCard(vDeck, chosenCard, rng, *currentHand);
				*currentScore = calculateHandValue(*currentHand, *currentScore);
				printPlayerHand(*currentHand, *currentScore);
				(*turn)++;
				break;
			case 'T':
			case 't':
				std::cout << "Player stands.\n\n";
				if((i + 1) == *numHands) {
					// If the player is playing their last hand
					*isPlayerTurn = false;
					*isHouseTurn = true;
				}
				else {
					*changePlayerHand = true;
				}
				(*turn)++;
				break;
			default:
				std::cout << "Invalid option, choose again\n";
				break;
			}
			std::cout << '\n';
		}
	}

	// House turn
	int * targetScore = new int;
	int * upperScore = new int;
	*upperScore = 0;

	// Check if a non-split hand is bust
	if(*numHands == 1) {
		if(*playerScore > 21) {
			std::cout << "PLAYER BUST!!\n";
			(*numHands)--;
		}
		else {
			*targetScore = *playerScore;
		}
	}

	// Check if split hands are bust
	if(*numHands == 2) {
		if(*playerFirstHandScore > 21) {
			std::cout << "PLAYER HAND 1 BUST!!\n";
			(*numHands)--;
		}
		if(*playerSecondHandScore > 21) {
			std::cout << "PLAYER HAND 2 BUST!!\n";
			(*numHands)--;
		}
	}

	if(*numHands > 0) { // Player has at least one playable hand
		// Set the target scores
		if(*numHands == 2) {
			if(*playerFirstHandScore < *playerSecondHandScore) {
				*targetScore = *playerFirstHandScore;
				*upperScore = *playerSecondHandScore;
			}
			else if(*playerFirstHandScore > *playerSecondHandScore) {
				*targetScore = *playerSecondHandScore;
				*upperScore = *playerFirstHandScore;
			}
			else {
				*targetScore = *playerFirstHandScore;
			}
		}
		else if(*numHands == 1 && *playerHasSplit) {
			if(*playerFirstHandScore <= 21) {
				*targetScore = *playerFirstHandScore;
			}
			else {
				*targetScore = *playerSecondHandScore;
			}
		}

		houseScore = calculateHandValue(houseHand, houseScore);
		printHouseHand(houseHand, houseScore, turn);
		std::cout << "\n";
		while(*isHouseTurn && *targetScore <= 21) { // House's turn, and House's hand is playable
			if(*houseScore > *targetScore) { // House's total is greater than the target score
				if(*houseScore <= 21) { // House hasn't bust
					if(*numHands == 2) { // There are split hands, the House must check if it beat the second hand
						if(*houseScore > *upperScore) {
							std::cout << "HOUSE WINS AGAINST BOTH HANDS!!\n\n";
						}
						else if(*houseScore < *upperScore) {
							std::cout << "HOUSE WINS AGAINST ONE HAND!!\n\n";
							std::cout << "HOUSE LOSES AGAINST ONE HAND!!\n\n";
						}
						else {
							std::cout << "HOUSE WINS AGAINST ONE HAND!!\n\n";
							std::cout << "HOUSE PUSHES ONE HAND!!\n\n";
						}
					}
					else { // There is only one hand, the House won
						std::cout << "HOUSE WINS!!\n\n";
					}
				}
				*isHouseTurn = false;
			}
			else if(*houseScore == *targetScore) {
				std::cout << "PUSH!!\n\n";
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
		if(*houseScore > 21) {
			std::cout << "HOUSE BUST!!\nPLAYER WINS!!\n\n";
		}
	}
	else { // Player has no playable hands
		std::cout << "HOUSE WINS!!\n\n";
	}
	
	currentHand = NULL;
	currentScore = NULL;

	// Debug listing remaining cards in deck
	/*for(unsigned i = 0; i < (*vDeck).size(); i++) {
	std::cout << (*vDeck).at(i).printName() << '\n';
	}*/

	delete changePlayerHand;
	delete chosenCard;
	delete currentHand;
	delete currentScore;
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
	delete targetScore;
	delete turn;
	delete upperScore;
	delete vDeck;
	delete waitTime;

	// Debugs to check if and what objects are not deleting correctly
	/*std::cout << "Deleting changePlayerHand" << '\n';
	delete changePlayerHand;
	std::cout << "Deleting chosenCard" << '\n';
	delete chosenCard;
	std::cout << "Deleting currentHand" << '\n';
	delete currentHand;
	std::cout << "Deleting currentScore" << '\n';
	delete currentScore;
	std::cout << "Deleting houseHand" << '\n';
	delete houseHand;
	std::cout << "Deleting houseInitScore" << '\n';
	delete houseInitScore;
	std::cout << "Deleting houseScore" << '\n';
	delete houseScore;
	std::cout << "Deleting isHouseTurn" << '\n';
	delete isHouseTurn;
	std::cout << "Deleting isPlayerTurn" << '\n';
	delete isPlayerTurn;
	std::cout << "Deleting numHands" << '\n';
	delete numHands;
	std::cout << "Deleting option" << '\n';
	delete option;
	std::cout << "Deleting playerHand" << '\n';
	delete playerHand;
	std::cout << "Deleting playerFirstHand" << '\n';
	delete playerFirstHand;
	std::cout << "Deleting playerSecondHand" << '\n';
	delete playerSecondHand;
	std::cout << "Deleting playerHasSplit" << '\n';
	delete playerHasSplit;
	std::cout << "Deleting playerScore" << '\n';
	delete playerScore;
	std::cout << "Deleting playerFirstHandScore" << '\n';
	delete playerFirstHandScore;
	std::cout << "Deleting playerSecondHandScore" << '\n';
	delete playerSecondHandScore;
	std::cout << "Deleting rng" << '\n';
	delete rng;
	std::cout << "Deleting selectDouble" << '\n';
	delete selectDouble;
	std::cout << "Deleting turn" << '\n';
	delete turn;
	std::cout << "Deleting vDeck" << '\n';
	delete vDeck;
	std::cout << "Deleting waitTime" << '\n';
	delete waitTime;*/
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