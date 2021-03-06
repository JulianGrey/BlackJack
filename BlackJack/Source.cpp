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

int calculateInsuranceBet(int * bet) {
	return (*bet) / 2;
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

void printHouseHand(vector<Card> * hand, int * score, int * turn, bool insured = false) {
	// Hiding the second House card at the start of a game
	if(*turn == 1 && !insured) {
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

void displayPlayerStats(double * chipTotal, double betOne = 0, double betTwo = 0, int numHands = 1) {
	if(betOne != 0) { // If a bet is in play
		if(numHands == 1) {
			std::cout.precision(std::to_string(static_cast<int>(betOne)).size() + 2); // Size of integer value + two decimal points
			std::cout << "Current bet: " << betOne << '\n';
		}
		else if(numHands > 1) { // Preparing a statement for implementing multiple splits
			for(unsigned i = 0; i < numHands; i++) {
				std::cout << "Current bet (hand " << i << "): ";
				if(i == 0) {
					std::cout.precision(std::to_string(static_cast<int>(betOne)).size() + 2);
					std::cout << betOne;
				}
				else if(i == 1) {
					std::cout.precision(std::to_string(static_cast<int>(betTwo)).size() + 2);
					std::cout << betTwo;
				}
				std::cout << '\n';
			}
		}
		std::cout << '\n';
	}
	std::cout.precision(std::to_string(static_cast<int>(*chipTotal)).size() + 2);
	std::cout << "Chip total: " << *chipTotal << '\n';
	std::cout.precision(0);
}

void playBlackJack(Card deck[], double * chipTotal) {
	Card * chosenCard = new Card;
	vector<Card> * houseHand = new vector<Card>;
	vector<Card> * playerHand = new vector<Card>;
	vector<Card> * playerHandOne = new vector<Card>;
	vector<Card> * playerHandTwo = new vector<Card>;
	vector<Card> * vDeck = resetDeck(deck);
	vector<Card> ** currentHand = new vector<Card> *; // Pointer containing the current hand in play

	char * option = new char;
	int * houseInitScore = new int; // Score of the first card in House's hand
	int * houseScore = new int;
	int * numHands = new int;
	int * playerScore = new int;
	int * playerHandOneScore = new int;
	int * playerHandTwoScore = new int;
	int * turn = new int;
	int * waitTime = new int;
	int ** currentScore = new int *; // Pointer containing the score of the current hand in play

	double * bet = new double;
	double * chipsWon = new double;
	double * handOneBet = new double;
	double * handTwoBet = new double;
	double * initChipTotal = new double;
	double * insuranceBet = new double;

	bool * changePlayerHand = new bool;
	bool * isHouseTurn = new bool;
	bool * isPlayerTurn = new bool;
	bool * isInsured = new bool;
	bool * insuranceCheck = new bool;
	bool * houseBlackjack = new bool;
	bool * playerHasSplit = new bool;
	bool * selectDouble = new bool;
	bool * surrender = new bool;

	std::mt19937 * rng = new mt19937;
	(*rng).seed(std::random_device()());

	*isPlayerTurn = true;
	*houseBlackjack = *isHouseTurn = *isInsured = false;
	*insuranceCheck = *playerHasSplit = *selectDouble = false;
	*surrender = false;

	*waitTime = 300;
	*numHands = *turn = 1;
	*insuranceBet = 0;
	*chipsWon = *handOneBet = *handTwoBet = 0;
	*houseScore = *houseInitScore = *playerScore = 0;
	*playerHandOneScore = *playerHandTwoScore = 0;
	*initChipTotal = *chipTotal;

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
	displayPlayerStats(chipTotal);
	std::cout << "Place your bet: ";
	while(*handOneBet == 0) {
		std::cin >> *bet;

		if(*bet > *chipTotal) {
			std::cout << "Insufficient funds. Enter a valid bet: ";
		}
		else {
			*handOneBet = *bet;
			*chipTotal -= *bet;
			displayPlayerStats(chipTotal, *handOneBet);
		}
	}
	houseScore = calculateHandValue(houseHand, houseScore);
	playerScore = calculateHandValue(playerHand, playerScore);
	printHouseHand(houseHand, houseInitScore, turn);
	printPlayerHand(playerHand, playerScore);
	std::cout << '\n';

	if(*houseInitScore == 11) {
		*insuranceCheck = true;
	}
	if(*houseScore == 21) {
		*houseBlackjack = true;
	}

	// Player's first turn
	while(*turn == 1 && isPlayerTurn) {
		if(*insuranceCheck && *playerScore < 21) {
			std::cout << "Do you want insurance?\n([Y]es, [N]o): ";
			while(*insuranceCheck) {
				cin >> *option;

				switch(*option) {
					case 'Y':
					case 'y':
						*chipTotal -= *bet / 2;
						*insuranceBet = *bet / 2;
						*isInsured = true;
						*insuranceCheck = false;
						printHouseHand(houseHand, houseScore, turn, *isInsured);
						break;
					case 'N':
					case 'n':
						*isInsured = false;
						*insuranceCheck = false;
						break;
					default:
						std::cout << "Invalid option, choose again\n";
						break;
				}
			}
		}

		if(*isInsured && *houseScore == 21) { // Insurance bet won
			*isPlayerTurn = false;
			*isHouseTurn = true;
			(*turn)++;
			break;
		}
		else {
			*insuranceBet = 0;
		}

		if(*playerScore < 21) {
			std::cout << "Select action\n" << "([H]it, S[t]and";
			if(*playerScore >= 9 && *playerScore <= 11) {
				std::cout << ", [D]ouble";
			}
			if((*playerHand).at(0).strValue == (*playerHand).at(1).strValue || (*playerHand).at(0).value >= 10 && (*playerHand).at(1).value >= 10) {
				std::cout << ", S[p]lit";
			}
			std::cout << ", S[u]rrender): ";
			std::cin >> *option;
			std::cout << '\n';

			switch(*option) {
				case 'H':
				case 'h':
					std::cout << "Player hits.\n\n";
					dealCard(vDeck, chosenCard, rng, playerHand);
					playerScore = calculateHandValue(playerHand, playerScore);
					printPlayerHand(playerHand, playerScore);
					if(*playerScore == 21) {
						std::cout << "Player stands on 21\n\n";
					}
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
					if(*playerScore >= 9 && *playerScore <= 11) {
						if((*bet) * 2 <= *chipTotal) {
							std::cout << "Player doubles up.\n\n";
							*selectDouble = true;
							*chipTotal -= *bet;
							*handOneBet += *bet;
							dealCard(vDeck, chosenCard, rng, playerHand);
							playerScore = calculateHandValue(playerHand, playerScore);
							printPlayerHand(playerHand, playerScore);
							displayPlayerStats(chipTotal, *handOneBet);
							*isPlayerTurn = false;
							*isHouseTurn = true;
							(*turn)++;
						}
						else {
							std::cout << "Insufficient funds. Can't double.\n\n";
						}
					}
					else {
						std::cout << "Invalid option, choose again\n";
					}
					break;
				case 'P':
				case 'p':
					if((*playerHand).at(0).strValue == (*playerHand).at(1).strValue || (*playerHand).at(0).value == (*playerHand).at(1).value) {
						if((*bet) * 2 <= *chipTotal) {
							std::cout << "Player splits.\n\n";
							*numHands = 2;
							*playerHasSplit = true;
							*handTwoBet = *handOneBet;
							*chipTotal -= *handTwoBet;
							(*playerHandOne).push_back((*playerHand).at(0));
							(*playerHandTwo).push_back((*playerHand).at(1));
							// Revert any changes made to the value of Aces in the event of being dealt two Aces
							// (see line 101 for information)
							if((*playerHandOne).at(0).value == 1) {
								(*playerHandOne).at(0).setValues(11);
							}
							if((*playerHandTwo).at(0).value == 1) {
								(*playerHandTwo).at(0).setValues(11);
							}
							dealCard(vDeck, chosenCard, rng, playerHandOne);
							dealCard(vDeck, chosenCard, rng, playerHandTwo);
							displayPlayerStats(chipTotal, *handOneBet, *handTwoBet, *numHands);
							playerHandOneScore = calculateHandValue(playerHandOne, playerHandOneScore);
							playerHandTwoScore = calculateHandValue(playerHandTwo, playerHandTwoScore);
							printPlayerHand(playerHandOne, playerHandOneScore, 1);
							printPlayerHand(playerHandTwo, playerHandTwoScore, 2);
							(*turn)++;
						}
						else {
							std::cout << "Insufficient funds, can't split.\n\n";
						}
						break;
					}
					else {
						std::cout << "Invalid option, choose again\n";
						break;
					}
				case 'U':
				case 'u':
					*surrender = true;
					*isPlayerTurn = false;
					*isHouseTurn = true;
					(*turn)++;
					break;
				default:
					std::cout << "Invalid option, choose again\n";
					break;
			}
		}
		else { // Player's score is at least 21
			if(*playerScore == 21) {
				*isHouseTurn = true;
			}
			*isPlayerTurn = false;
			(*turn)++;
		}
		std::cout << '\n';
	}

	for(unsigned i = 0; i < *numHands; i++) {
		*changePlayerHand = false;
		if(*numHands == 2) {
			if(i == 0) {
				currentHand = &playerHandOne;
				currentScore = &playerHandOneScore;
			}
			else {
				currentHand = &playerHandTwo;
				currentScore = &playerHandTwoScore;
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
				std::cout << "Select action:\n([H]it, S[t]and, S[u]rrender)\n";
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
				if(**currentScore > 21) {
					if(*numHands == 2) {
						std::cout << "PLAYER HAND " << i + 1 << " BUST!!\n\n";
					}
					else {
						std::cout << "PLAYER BUST!!\n\n";
					}
				}
				else if(**currentScore == 21) {
					std::cout << "Player stands on 21\n\n";
				}
				if(**currentScore >= 21 && i != (*numHands) - 1) { // If there is a next hand
					*changePlayerHand = true;
				}
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
			case 'U':
			case 'u':
				if(*numHands == 1) {
					*surrender = true;
					*isPlayerTurn = false;
					*isHouseTurn = true;
					(*turn)++;
				}
				else {
					std::cout << "Invalid option, choose again\n";
				}
				break;
			default:
				std::cout << "Invalid option, choose again\n";
				break;
			}
			if(*changePlayerHand) {
				std::cout << "Changing hands...\n\n";
				printPlayerHand(playerHandTwo, playerHandTwoScore);
			}
			std::cout << '\n';
		}
	}
	*isPlayerTurn = false;
	*isHouseTurn = true;

	// Check if a non-split hand is bust
	if(*numHands == 1) {
		if(*playerScore > 21) {
			(*numHands)--;
		}
	}

	// Check if split hands are bust
	if(*numHands == 2) {
		if(*playerHandOneScore > 21) {
			(*numHands)--;
		}
		if(*playerHandTwoScore > 21) {
			(*numHands)--;
		}
	}

	if(*numHands > 0) { // Player has at least one playable hand
		houseScore = calculateHandValue(houseHand, houseScore);
		if(!(*surrender)) {
			printHouseHand(houseHand, houseScore, turn);
		}
		std::cout << "\n";
		std::this_thread::sleep_for(std::chrono::milliseconds(*waitTime));

		while(*isHouseTurn && !(*surrender) && *houseScore < 17) {
			dealCard(vDeck, chosenCard, rng, houseHand);
			houseScore = calculateHandValue(houseHand, houseScore);
			printHouseHand(houseHand, houseScore, turn);
			std::cout << '\n';
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(*waitTime));

		if(*houseScore <= 21) {
			if(!(*surrender)) {
				std::cout << "House stands.\n\n";
			}

			if(*surrender) {
				if(*houseInitScore >= 10) {
					printHouseHand(houseHand, houseScore, turn);
					std::cout << '\n';
				}
				if(*houseBlackjack) {
					std::cout << "HOUSE HAS A BLACKJACK!! SURRENDER DENIED!!\n\n";
				}
				else {
					if(*numHands == 1) {
						*chipsWon += *handOneBet / 2;
					}
				}
			}
			else if(*numHands == 2) {
				if(*houseScore == *playerHandOneScore) {
					std::cout << "HOUSE PUSHES PLAYER HAND 1!!\n\n";
					*chipsWon += *handOneBet;
				}
				else if(*houseScore > *playerHandOneScore) {
					std::cout << "HOUSE WINS AGAINST PLAYER HAND 1!!\n\n";
				}
				else {
					std::cout << "PLAYER WINS WITH HAND 1!!\n\n";
					*chipsWon += *handOneBet * 2;
				}
				if(*houseScore == *playerHandTwoScore) {
					std::cout << "HOUSE PUSHES PLAYER HAND 2!!\n\n";
					*chipsWon += *handTwoBet;
				}
				else if(*houseScore > *playerHandTwoScore) {
					std::cout << "HOUSE WINS AGAINST PLAYER HAND 2!!\n\n";
				}
				else {
					std::cout << "PLAYER WINS WITH HAND 2!!\n\n";
					*chipsWon += *handTwoBet * 2;
				}
			}
			else if(*numHands == 1) {
				if(*houseBlackjack && *isInsured) { // House dealt Blackjack with Ace as showing card, and player insured
					std::cout << "INSURANCE PAYS OUT!!\n\n";
					*chipsWon += *handOneBet;
				}
				else if(*playerHasSplit) {
					if(*playerHandOneScore <= 21) {
						if(*houseScore == *playerHandOneScore) {
							std::cout << "HOUSE PUSHES PLAYER HAND 1!!\n\n";
							*chipsWon += *handOneBet;
						}
						else if(*houseScore > *playerHandOneScore) {
							std::cout << "HOUSE WINS AGAINST PLAYER HAND 1!!\n\n";
						}
						else {
							std::cout << "PLAYER WINS WITH HAND 1!!\n\n";
							*chipsWon += *handOneBet * 2;
						}
					}
					else {
						if(*houseScore == *playerHandTwoScore) {
							std::cout << "HOUSE PUSHES PLAYER HAND 2!!\n\n";
							*chipsWon += *handTwoBet;
						}
						else if(*houseScore > *playerHandTwoScore) {
							std::cout << "HOUSE WINS AGAINST PLAYER HAND 2!!\n\n";
						}
						else {
							std::cout << "PLAYER WINS WITH HAND 2!!\n\n";
							*chipsWon += *handTwoBet * 2;
						}
					}
				}
				else {
					if(*playerScore == 21 && (*playerHand).size() == 2) { // If a BlackJack was dealt
						std::cout << "BLACKJACK\n\n";
						if(*houseScore == *playerScore) {
							std::cout << "HOUSE PUSHES PLAYER!!\n\n";
							*chipsWon += *handOneBet;
						}
						else {
							std::cout << "PLAYER WINS WITH A BLACKJACK!!\n\n";
							*chipsWon += *handOneBet * 2.5;
						}
					}
					else if(*houseScore == *playerScore) {
						std::cout << "HOUSE PUSHES PLAYER!!\n\n";
						*chipsWon += *handOneBet;
					}
					else if(*houseScore > *playerScore) {
						std::cout << "HOUSE WINS!!\n\n";
					}
					else {
						std::cout << "PLAYER WINS!!\n\n";
						*chipsWon += *handOneBet * 2;
					}
				}
			}
		}
		else {
			std::cout << "HOUSE BUST!!\nPLAYER WINS!!\n\n";
			*chipsWon += *handOneBet * 2;
		}
	}
	else { // Player has no playable hands
		std::cout << "HOUSE WINS!!\n\n";
	}
	
	*chipTotal += *chipsWon + *insuranceBet;

	// Reset the bet values
	// This is so that we only get the chip total from displayPlayerStats
	*handOneBet = 0;
	*handTwoBet = 0;

	std::cout << "Balance change: ";
	if(*chipTotal - *initChipTotal > 0) {
		std::cout << "+";
	}
	std::cout << *chipTotal - *initChipTotal << '\n';
	displayPlayerStats(chipTotal);
	std::cout << '\n';

	currentHand = NULL;
	currentScore = NULL;

	// Debug listing remaining cards in deck
	/*for(unsigned i = 0; i < (*vDeck).size(); i++) {
	std::cout << (*vDeck).at(i).printName() << '\n';
	}*/

	delete bet;
	delete changePlayerHand;
	delete chipsWon;
	delete chosenCard;
	delete currentHand;
	delete currentScore;
	delete houseBlackjack;
	delete houseHand;
	delete houseInitScore;
	delete houseScore;
	delete initChipTotal;
	delete insuranceBet;
	delete insuranceCheck;
	delete isHouseTurn;
	delete isInsured;
	delete isPlayerTurn;
	delete numHands;
	delete option;
	delete playerHand;
	delete playerHandOne;
	delete playerHandOneScore;
	delete playerHandTwo;
	delete playerHandTwoScore;
	delete playerHasSplit;
	delete playerScore;
	delete rng;
	delete selectDouble;
	delete surrender;
	delete turn;
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
	std::cout << "Deleting playerHandOne" << '\n';
	delete playerHandOne;
	std::cout << "Deleting playerHandTwo" << '\n';
	delete playerHandTwo;
	std::cout << "Deleting playerHasSplit" << '\n';
	delete playerHasSplit;
	std::cout << "Deleting playerScore" << '\n';
	delete playerScore;
	std::cout << "Deleting playerHandOneScore" << '\n';
	delete playerHandOneScore;
	std::cout << "Deleting playerHandTwoScore" << '\n';
	delete playerHandTwoScore;
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
	double * chipTotal = new double;
	bool * playGame = new bool;

	*chipTotal = 300;
	*playGame = true;
	while(*playGame) {
		playBlackJack(pDeck, chipTotal);
		*playGame = playAgain();
	}
	delete chipTotal;
	delete playGame;
	return 0;
}