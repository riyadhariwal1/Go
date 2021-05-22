// a5.cpp

/////////////////////////////////////////////////////////////////////////
//
// Student Info
// ------------
//
// Name : Danielle Nix and Riya Dhariwal
// St.# : 301365731 and 301363373
// Email: dnix@sfu.ca and riya_dhariwal@sfu.ca
//
//
// Statement of Originality
// ------------------------
//
// All the code and comments below are my own original work. For any non-
// original work, I have provided citations in the comments with enough
// detail so that someone can see the exact source and extent of the
// borrowed work.
//
// In addition, I have not shared this work with anuserInputne else, and I have
// not seen solutions from other students, tutors, websites, books,
// etc.
//
/////////////////////////////////////////////////////////////////////////

#include "cmpt_error.h"           
#include <iostream>        
#include <string>
#include <vector>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <cstdlib>
#include <stdlib.h>

using namespace std;

//constant variables of rgb values
const string background = "\033[48;2;0;128;0m";
const string option = "\033[38;2;255;255;255m+\033[0m";
const string comp_piece = "\033[38;2;0;0;0m■\033[0m";
const string player_piece = "\033[38;2;255;255;255m■\033[0m";
const string reset_color = "\033[0m";
const string hyphen = "\033[38;2;255;255;255m-\033[0m";
const string purple = "\033[38;2;147;112;219m";

// represents the current state of the game board
class Board{

public:

    //constructor
    Board(vector<vector<string>>& newBoard){
        newBoard[3][3] = player_piece;
        newBoard[4][4] = player_piece;
        newBoard[3][4] = comp_piece;
        newBoard[4][3] = comp_piece;

        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                if (newBoard[i][j] == ""){
                    newBoard[i][j] = hyphen;
                }
            }
        }
    }

    // prints current board 
    void print_board(vector<vector<string>>& newBoard){

        cout << "  a b c d e f g h" << endl;

        for(int i = 0; i < 8; i++){
            cout << i + 1;
            for(int j = 0; j < 8; j++){
                string line = background + " " + newBoard[i][j];
                cout << line << reset_color; 
            }
            cout << endl;
        }
    }

    //updates 2, 2d vectors. Copies one contents to another.
    void update_board(vector<vector<string>>& newBoard, vector<vector<string>>& compBoard){
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                if (newBoard[i][j] == player_piece){
                    compBoard[i][j] = newBoard[i][j];
                }
                if (newBoard[i][j] == comp_piece){
                    compBoard[i][j] = newBoard[i][j];
                }
            }
        }
    }

    //'resets' a 2d vector to all hyphens 
    void reset_board(vector<vector<string>>& compBoard){
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                compBoard[i][j] = hyphen;
            }
        }
    }

    //counts number of pieces an returns it
    int token_count(vector<vector<string>>& newBoard, const string player){
        int counter = 0;

        for(int row = 0; row < 8; row++){
            for(int col = 0; col < 8; col++){
                if(newBoard[row][col] == player){
                    counter++;
                }
            }
        }

        return counter;
    }

    //returns true if there's a plus sign and false if there isnt one to check for any possible moves
    bool option_count(vector<vector<string>>& newBoard){
        for(int row = 0; row < 8; row++){
            for(int col = 0; col < 8; col++){
                if(newBoard[row][col] == option){
                    return true;
                }
            }
        }
        return false;
    }

    // destructor
    ~Board(){ }

};//Board class

// abstract base class called Player that represents a generic player, either a human or a computer
class Player{
public:

    virtual void valid(vector<vector<string>>& newBoard) = 0;
    virtual bool check(vector<vector<string>>& newBoard, int r, int direction_r, int c, int direction_c) = 0;
    virtual void option_location(int row, int col, vector<vector<string>>& newBoard) = 0;
    virtual void flip_token(vector<vector<string>>& newBoard, string userInput) = 0;
    virtual bool backtrack(vector<vector<string>>& newBoard, int r, int direction_r, int c, int direction_c) = 0;
    virtual void flip_location(int row, int col, vector<vector<string>>& newBoard) = 0;
    virtual string transformer(string user_input) = 0;
    virtual ~Player(){ }
    
};                   

class Human_player : public Player{
private:
    bool n, ne, nw, s, se, sw, e, w;
    string input;
public:
	//constructor
    Human_player ()
    : input("")
    {}

    // destructor 
    ~Human_player(){ }

    // validates input 
    string input_validator(vector<string>& newChoices){
        bool bool_input = 1;

        while(bool_input == 1){

            string input;
            cout << "Enter your move here  ";
            cin >> input;
            //calls input checker which checks to see if the user input is in the list of possible valid moves
            bool_input = input_checker(input, newChoices);

            if(bool_input){
                return input;
            }
            else{
                cout << "Illegal move played. PLease try again using the plus signs as indications of valid moves" << endl;
                bool_input = 1;
            }
        }
        return "";
    }

    // compares input to vector containing possible moves
    bool input_checker(string input, vector<string>& newChoices){
        string new_input;
        new_input = transformer(input);
        for(int i = 0; i < newChoices.size(); i++){
            if(newChoices[i] == new_input){
                return true;
            }
        }
        return false;
    }
    
    // makes a vector of possible choices
    void possible_choices(vector<vector<string>>& newBoard, vector<string>& newChoices){
        newChoices.clear();
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                if(newBoard[i][j] == option){
                    string moves;
                    moves += to_string(i);
                    moves += to_string(j);
                    newChoices.push_back(moves);
                }
            }
        }
    }

    //this function goes through all possible 8 directions of every piece a piece may move and calls check function 
    void valid(vector<vector<string>>& newBoard){
        
        for(int row = 0; row < 8; row++){
            for(int col = 0; col < 8; col++){
                if(newBoard[row][col] == player_piece){
                  
                    n = check(newBoard, row, -1, col, 0);
                    ne = check(newBoard, row, -1, col, 1);
                    nw = check(newBoard, row, -1, col, -1);
                    s = check(newBoard, row, 1, col, 0);
                    se = check(newBoard, row, 1, col, 1);
                    sw = check(newBoard, row, 1, col, -1);
                    e = check(newBoard, row, 0, col, 1);
                    w = check(newBoard, row, 0, col, -1);
                }
            }
        }
    }

    //returns false if it cant move...
    bool check(vector<vector<string>>& newBoard, int r, int direction_r, int c, int direction_c){

        if((r + direction_r < 0) || (r + direction_r > 7) || (c + direction_c < 0) || (c + direction_c > 7)){
            return false;
        }else{

            r += direction_r;
            c += direction_c;

            if(newBoard[r][c] == hyphen){
                return false;

            }else if(newBoard[r][c] == player_piece){
                return false;

            }else if(newBoard[r][c] == comp_piece){

                if((r + direction_r < 0) || (r + direction_r > 7) || (c + direction_c < 0) || (c + direction_c > 7)){
                    return false;
                //...else it will call option location which puts a plus sign 
                }else if(newBoard[r + direction_r][c + direction_c] == hyphen){
                    option_location(r + direction_r, c + direction_c, newBoard);
                    return true;

                }else{
                	//recusively calls the check function again to check the next piece if it's not a hyphen
                    return check(newBoard, r, direction_r, c, direction_c);
                }
            }else{
                return false;
            }
        }
    }
    //places a plus sign at a valid move
    void option_location(int row, int col, vector<vector<string>>& newBoard){
        newBoard[row][col] = option;
    }
    //flip works similarly to valid except it's for flipping tokens
    void flip_token(vector<vector<string>>& newBoard, string userInput){
       
        string new_input; 
        new_input = transformer(userInput);
        int row = 0;
        row = new_input[0] - '0';
        int col = 0;
        col = new_input[1] - '0';
        
        n = backtrack(newBoard, row, -1, col, 0);
        ne = backtrack(newBoard, row, -1, col, 1);
        nw = backtrack(newBoard, row, -1, col, -1);
        s = backtrack(newBoard, row, 1, col, 0);
        se = backtrack(newBoard, row, 1, col, 1);
        sw = backtrack(newBoard, row, 1, col, -1);
        e = backtrack(newBoard, row, 0, col, 1);
        w = backtrack(newBoard, row, 0, col, -1);
     
    }
    //backtrack works similarly to check except it starts from the plus sign and keeps going until it reaches a comp piece
    //else it will return false 
    bool backtrack(vector<vector<string>>& newBoard, int r, int direction_r, int c, int direction_c){

        if((r + direction_r < 0) || (r + direction_r > 7) || (c + direction_c < 0) || (c + direction_c > 7)){
            flip_location(r, c, newBoard);
            return false;
            
        }else{
            
            r += direction_r;
            c += direction_c;

            if(newBoard[r][c] == hyphen){
                return false;

            }else if(newBoard[r][c] == player_piece){
                return false;

            }else if(newBoard[r][c] == comp_piece){

                if((r + direction_r < 0) || (r + direction_r > 7) || (c + direction_c < 0) || (c + direction_c > 7)){
                    return false;

                }else if(newBoard[r + direction_r][c + direction_c] == player_piece){

                    while(newBoard[r][c] == comp_piece || newBoard[r][c] == option){
                    	//once it reaches the end, it will start flipping the pieces and reversing the way it iterated through
                        flip_location(r, c, newBoard);
                        r -= direction_r;
                        c -= direction_c;

                        if((r - direction_r < 0) || (r - direction_r > 7) || (c - direction_c < 0) || (c - direction_c > 7)){
                            return false;
                        }
                    }
                    return true;

                }else{
                    return backtrack(newBoard, r, direction_r, c, direction_c);
                }

            }else{
                return false;
            }
        } 
    }

    void flip_location(int row, int col, vector<vector<string>>& newBoard){
        newBoard[row][col] = player_piece;
    }

    // transforms user input to place on board
    string transformer(string user_input){
       
        int row = 0;
        row = user_input[0] - '0';

        string input;
        input += to_string(row - 1);

        if(user_input[1] == 'a'){
            input += to_string(0);
        }
        if(user_input[1] == 'b'){
            input += to_string(1);
        }
        if(user_input[1] == 'c'){
            input += to_string(2);
        }
        if(user_input[1] == 'd'){
            input += to_string(3);
        }
        if(user_input[1] == 'e'){
            input += to_string(4);
        }
        if(user_input[1] == 'f'){
            input += to_string(5);
        }
        if(user_input[1] == 'g'){
            input += to_string(6);
        }
        if(user_input[1] == 'h'){
            input += to_string(7);
        }

        return input;
    }

}; //Human_player class

// derived from the Player class
class Computer_player : public Player{
private:
        bool n, ne, nw, s, se, sw, e, w;
public: 
	//this function goes through all possible 8 directions of every piece a piece may move and calls check function 
    void valid(vector<vector<string>>& newBoard){
        
        for(int row = 0; row < 8; row++){
            for(int col = 0; col < 8; col++){
                if(newBoard[row][col] == comp_piece){
                  
                    n = check(newBoard, row, -1, col, 0);
                    ne = check(newBoard, row, -1, col, 1);
                    nw = check(newBoard, row, -1, col, -1);
                    s = check(newBoard, row, 1, col, 0);
                    se = check(newBoard, row, 1, col, 1);
                    sw = check(newBoard, row, 1, col, -1);
                    e = check(newBoard, row, 0, col, 1);
                    w = check(newBoard, row, 0, col, -1);

                }
            }
        }
    }

    bool check(vector<vector<string>>& newBoard, int r, int direction_r, int c, int direction_c){

        if((r + direction_r < 0) || (r + direction_r > 7) || (c + direction_c < 0) || (c + direction_c > 7)){
            return false;
        }else{

            r += direction_r;
            c += direction_c;

            if(newBoard[r][c] == hyphen){
                return false;

            }else if(newBoard[r][c] == comp_piece){
                return false;

            }else if(newBoard[r][c] == player_piece){

                if((r + direction_r < 0) || (r + direction_r > 7) || (c + direction_c < 0) || (c + direction_c > 7)){
                    return false;

                }else if(newBoard[r + direction_r][c + direction_c] == hyphen){
                    option_location(r + direction_r, c + direction_c, newBoard);
                    return true;

                }else{
                    return check(newBoard, r, direction_r, c, direction_c);
                }

            }else{
                return false;
            }
        }
    }

    void option_location(int row, int col, vector<vector<string>>& compBoard){
        compBoard[row][col] = option;
    }

    // makes a vector of possible choices
    void possible_choices(vector<vector<string>>& newBoard, vector<string>& newChoices){

        newChoices.clear();
        for(int i = 0; i < 8; i++){
            for(int j = 0; j < 8; j++){
                if(newBoard[i][j] == option){
                    string moves;
                    moves += to_string(i);
                    moves += to_string(j);
                    newChoices.push_back(moves);
                }
            }
        }
    }
    //finding best move finds the best possible move the computer can play by finding which option will result in the most flipping of pieces
    string finding_best_move(vector<vector<string>>& compBoard, vector<string>& comp_choices){
        
        string comp_move = "";
        string best_move = "";
        
        int row, col = 0;
        int index = 0;
        int max_counter = 0;
        int vec_max_counter = 0;

        vector<int> vec_max;
        vector<int> counter;


        vec_max.clear();

        for(int i = 0; i < comp_choices.size(); i++){

            counter.clear();
            max_counter = 0;

            comp_move = comp_choices[i];

            row = comp_move[0] - '0';
            col = comp_move[1] - '0';
            //the counter vector will contain the number of pieces that can be flipped in each possible direction
            counter.push_back(counting_longest(compBoard, row, -1, col, 0, 0)); //north
            counter.push_back(counting_longest(compBoard, row, -1, col, 1, 0)); //north-east
            counter.push_back(counting_longest(compBoard, row, -1, col, -1, 0));//north-west
            counter.push_back(counting_longest(compBoard, row, 1, col, 0, 0));  //south
            counter.push_back(counting_longest(compBoard, row, 1, col, 1, 0));  //south-east
            counter.push_back(counting_longest(compBoard, row, 1, col, -1, 0)); //south-west
            counter.push_back(counting_longest(compBoard, row, 0, col, 1, 0));  //east
            counter.push_back(counting_longest(compBoard, row, 0, col, -1, 0)); //west
            //then for every piece, it will look through the counter vector and find the greatest piece and store it in max_counter variable
            for(int i = 0; i < counter.size(); i++){
                if(counter[i] >= max_counter){
                    max_counter = counter[i];
                }
            }
            //for every piece, the greatest direction counter value will be stored in the vec_max
            vec_max.push_back(max_counter);
        }
        //finally, to find the greatest value out of all the pieces, it will go through vec max and for the one with the greatest piece, it will store that index in the index variable to access it
        for(int i = 0; i < vec_max.size(); i++){
            if(vec_max[i] >= vec_max_counter){
                vec_max_counter = vec_max[i];
                index = i;
            }
        }
        //best move is the element in comp_choices where index is. comp choices is passed into the funtion and contains the posible moves found in possible_moves function
        best_move = comp_choices[index];
        return best_move;
    }
    //counting longest works similarly to backtrack but instead of flipping pieces, it counts them
    int counting_longest(vector<vector<string>>& compBoard, int r, int direction_r, int c, int direction_c, int counter){

        if((r + direction_r < 0) || (r + direction_r > 7) || (c + direction_c < 0) || (c + direction_c > 7)){
            return counter;
        }else{
            r += direction_r;
            c += direction_c;

            if(compBoard[r][c] == player_piece){

                if((r + direction_r < 0) || (r + direction_r > 7) || (c + direction_c < 0) || (c + direction_c > 7)){
                    return counter;
                }

                else if(compBoard[r + direction_r][c + direction_c] == comp_piece){

                    while(compBoard[r][c] == player_piece || compBoard[r][c] == option){
                        counter++;
                        r -= direction_r;
                        c -=direction_c;

                        if((r - direction_r < 0) || (r - direction_r > 7) || (c - direction_c < 0) || (c - direction_c > 7)){
                            return counter;
                        }
                    }

                    return counter;

                }else{
                    return counting_longest(compBoard, r, direction_r, c, direction_c, counter);
                }
            }
            else{
                return counter;
            }
        }
    }

    void flip_token(vector<vector<string>>& newBoard, string new_input){

        int row = 0;
        row = new_input[0] - '0';
        int col = 0;
        col = new_input[1] - '0';
        
        n = backtrack(newBoard, row, -1, col, 0);
        ne = backtrack(newBoard, row, -1, col, 1);
        nw = backtrack(newBoard, row, -1, col, -1);
        s = backtrack(newBoard, row, 1, col, 0);
        se = backtrack(newBoard, row, 1, col, 1);
        sw = backtrack(newBoard, row, 1, col, -1);
        e = backtrack(newBoard, row, 0, col, 1);
        w = backtrack(newBoard, row, 0, col, -1);
     
    }

    bool backtrack(vector<vector<string>>& newBoard, int r, int direction_r, int c, int direction_c){

        if((r + direction_r < 0) || (r + direction_r > 7) || (c + direction_c < 0) || (c + direction_c > 7)){
            flip_location(r, c, newBoard);
            return false;
            
        }else{

            r += direction_r;
            c += direction_c;

            if(newBoard[r][c] == hyphen){
                return false;

            }else if(newBoard[r][c] == comp_piece){
                return false;

            }else if(newBoard[r][c] == player_piece){

                if((r + direction_r < 0) || (r + direction_r > 7) || (c + direction_c < 0) || (c + direction_c > 7)){
                    return false;
                }

                else if(newBoard[r + direction_r][c + direction_c] == comp_piece){

                    while(newBoard[r][c] == player_piece || newBoard[r][c] == option){
                        flip_location(r, c, newBoard);
                        r -= direction_r;
                        c -= direction_c;
                        
                        if((r - direction_r < 0) || (r - direction_r > 7) || (c - direction_c < 0) || (c - direction_c > 7)){
                            return false;
                        }
                    }
                    return true;

                }else{
                    return backtrack(newBoard, r, direction_r, c, direction_c);
                }

            }else{
                return false;
            }
        }
    }
    
    void flip_location(int row, int col, vector<vector<string>>& newBoard){
        newBoard[row][col] = comp_piece;
    }

    string transformer(string user_input){
       
        int row = 0;
        row = user_input[0] - '0';

        string input;
        input += to_string(row + 1);

        if(user_input[1] == '0'){
            input += "a";
        }
        if(user_input[1] == '1'){
            input += "b";
        }
        if(user_input[1] == '2'){
            input += "c";
        }
        if(user_input[1] == '3'){
            input += "d";
        }
        if(user_input[1] == '4'){
            input += "e";
        }
        if(user_input[1] == '5'){
            input += "f";
        }
        if(user_input[1] == '6'){
            input += "g";
        }
        if(user_input[1] == '7'){
            input += "h";
        }

        return input;
    } 


};


int main(){
	//initializing 2, 2d vectors 
    vector<vector<string>> board(8, vector<string>(8));
    vector<vector<string>> comp_choices_board(8, vector<string>(8));
    vector<string> player_choices;
    vector<string> comp_choices;
    //objects created
    Computer_player cpu;
    Human_player hp;
    Board game(board);

    game.reset_board(comp_choices_board);
    game.update_board(board, comp_choices_board);

    bool human_turn = true;
    bool stop = false;
    bool no_movesP;
    bool no_movesCPU;

    cout << endl << endl;

    cout << purple
         << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" <<endl
         << "                                               Welcome to Reversi ! " << endl
         << "    You're playing white and I'm playing black. The objective of the game is to ensure that you have more pieces than me." <<endl
         << "                 To pick a spot to move, enter in the row first and then the colomn. For example, 2e" <<endl
         << "                         Make sure it's a legal move as indicated by the white plus signs (+)" <<endl
         << "                               You're playing first. Good luck !  You'll need it !!" <<endl
         << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" <<endl
         <<reset_color;

    cout << endl << endl << endl << endl << endl << endl;

    int continue_playing = 1;
    //allows user to keep playing
    while (continue_playing == 1){
    	//when the game ends
    	while(!stop){
    		//whose turn it is: cpu or human
	        if(human_turn){

	            cout << endl << "Your turn !" << endl << endl;
	            game.reset_board(board);
	            game.update_board(comp_choices_board, board);

	            hp.valid(board);

	            game.print_board(board);
	            int comp_tokens = 0;
	            comp_tokens = game.token_count(board, comp_piece);
	            int player_tokens = 0;
	            player_tokens = game.token_count(board, player_piece);
	            cout << "White Pieces - " << player_tokens << endl << "Black Pieces - " << comp_tokens << endl;
	            cout << endl;
	            
	            hp.possible_choices(board, player_choices);

	            no_movesP = game.option_count(board);
	            if(no_movesP == false){
	            	no_movesP = true;
	            	cout << "Your turn skipped because you have no possible moves" << endl;

	            }else{
	            	no_movesP = false;
		            string player_move;
		            player_move = hp.input_validator(player_choices);

		            hp.flip_token(board, player_move);

		            game.reset_board(comp_choices_board);
		            game.update_board(board, comp_choices_board);

		            cout << endl << endl << "you played " << player_move << endl << endl;

		            game.print_board(comp_choices_board);
		            comp_tokens = 0;
		            comp_tokens = game.token_count(board, comp_piece);
		            player_tokens = 0;
		            player_tokens = game.token_count(board, player_piece);
		            cout << "White Pieces - " << player_tokens << endl << "Black Pieces - " << comp_tokens << endl;
		            cout << endl << endl;
	            }

	        }else{

	            game.reset_board(comp_choices_board);
	            game.update_board(board, comp_choices_board);
	            
	            cpu.valid(comp_choices_board);

	            cpu.possible_choices(comp_choices_board, comp_choices);

    			no_movesCPU = game.option_count(board);
	            if(no_movesCPU == false){
	            	cout << "Computer skips turn because I have no moves" <<endl;
	            	no_movesCPU = true;
	            }
	            else{
	            	no_movesCPU = false;

		            string computer_move;
		            computer_move = cpu.finding_best_move(comp_choices_board, comp_choices);
		            
		            cpu.flip_token(comp_choices_board, computer_move); 

		            game.reset_board(board);
		            game.update_board(comp_choices_board, board);

		            cout << endl << "computer plays " << cpu.transformer(computer_move) << endl << endl;
		            game.print_board(board);
		            int comp_tokens = 0;
		            comp_tokens = game.token_count(board, comp_piece);
		            int player_tokens = 0;
		            player_tokens = game.token_count(board, player_piece);
		            cout << "White Pieces - " << player_tokens << endl << "Black Pieces - " << comp_tokens << endl;
		            cout << endl << endl;
	            }
	        }

	        if(no_movesP == true && no_movesCPU == true){
	        	stop = !stop;
	        }
	        else{
	        	human_turn = !human_turn;
	        }
	    }

	    cout << "Game Over !!" <<endl;

	    int comp_tokens = 0;
        comp_tokens = game.token_count(board, comp_piece);
        int player_tokens = 0;
        player_tokens = game.token_count(board, player_piece);

        if(comp_tokens > player_tokens){
        	cout << "Computer wins !  Try better next time human !" << endl;
        }
       	if(player_tokens > comp_tokens){
       		cout << "Player wins !  Congratulations. You were a worthy opponent." << endl;
       	}
       	if(player_tokens == comp_tokens){
       		cout << "It's a tie ! Looks like we're both champions !" << endl;
       	}

       	stop = !stop;
       	cout << "Would you like to play again? If yes, enter 1." << endl;
       	cin >> continue_playing;
    }

    cout << endl << endl << "I hope you had fun playing ! Good-bye !!" << endl;
}