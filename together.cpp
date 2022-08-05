#include <iostream>
#include <fstream>
#include <limits>
#include <string>
#include <array>            // std::array
#include <cstddef>
#include <algorithm>        // std::transform
#include <type_traits>      // std::is_same_v usw.
#include <vector>
#include <filesystem>
#include <set>
#include <chrono>
#include <thread>
#include <memory>

#include "encryption.h"
#include <unistd.h>

auto account = std::make_shared<std::string>("standin");

bool login(std::shared_ptr<std::string>& login){

	std::string username, password, original_un, original_pw;

	std::cout << "Username: ";
	std::cin >> username;
	std::cout << "Password: ";
	std::cin >> password;
	
	login = std::make_shared<std::string>(username);

	std::ifstream read("accounts/" + username + ".txt");
	std::getline(read, original_un);
	std::getline(read, original_pw);

	if (original_un == username && original_pw == password ){
		return true;
	}
	else {
		return false;
	}
}

void registration(){
	std::string username, password;

	std::cout << "select a username: ";
	std::cin >> username;
	
	if( std::filesystem::exists("accounts/" + username + ".txt")) {
		std::cout << "This Account already exists. Login or choose another name." << '\n';
		registration();
	}
	else{
		std::cout << "select a password: ";
		std::cin >> password;
		
		std::ofstream file;
		file.open("accounts/" + username + ".txt");

		file << username << '\n' << password;
		file.close();
		
			
		if (!std::filesystem::is_directory("entries/" + username + "_entries") || !std::filesystem::exists("entries/" + username + "_entries")) { // Check if src folder exists
			std::filesystem::create_directory("entries/" + username + "_entries"); // create src folder
		}
	}
	
}

void list(std::shared_ptr<std::string> login, int& number) {
	std::string title;
	
	std::filesystem::path path{std::filesystem::current_path()};
    std::string path_string{path.u8string()};
	
	std::string filepath = path_string + "/entries/" + *login + "_entries/";
	auto dirIter = std::filesystem::directory_iterator(filepath);
	int fileCount = 0;
	
	std::set<std::filesystem::path> sorted_by_name;

	for (auto& entry : dirIter)
	{
		if (entry.is_regular_file()){
			++fileCount;
			number = fileCount;
			sorted_by_name.insert(entry.path());
			
		}
	}
	int i = 0;
	std::string standin = *login;
	for (auto &filename : sorted_by_name){
		std::string path_string = filename.c_str();
		path_string.erase (path_string.begin(),path_string.begin() + filepath.size());	
		path_string.erase (path_string.begin() + 2, path_string.begin() + standin.size() + 14);
		
		//std::string id_str = std::to_string(number);
		i++;
		std::string j = std::to_string(i);
		
		std::ifstream read("entries/" + *login + "_entries/" + j + "_" + *login + "_entries.txt");
		getline(read, title);
		path_string.append(title);
		std::cout << path_string << '\n';
	}
			
    
}

//in Datei schreiben
void entry_input(std::shared_ptr<std::string> login, const int id){
	std::string title, username_entry, password_entry, comment_entry;
	
	std::string id_str = std::to_string(id);
	
	
	std::cout << "Enter the Title here: ";
	std::cin >> title;
	std::cout << "\n\nEnter your Username here: ";
	std::cin >> username_entry;
	std::cout << "\n\nEnter your Password here: ";
	std::cin >> password_entry;
	std::cout << "\n\nEnter your Comment here: ";
	std::cin >> comment_entry;
	
	
	//Benutzername verschlüsseln
	std::string input_un = username_entry;
    std::vector<unsigned char> outbase64_un {};
    encode(outbase64_un, input_un);
    
    //Passwort verschlüsseln
	std::string input_pw = password_entry;
    std::vector<unsigned char> outbase64_pw {};
    encode(outbase64_pw, input_pw);
    
    //Kommentar verschlüsseln
	std::string input_cmt = comment_entry;
    std::vector<unsigned char> outbase64_cmt {};
    encode(outbase64_cmt, input_cmt);
	
	//Datei öffnen
	std::ofstream file;
	file.open("entries/" + *login + "_entries/" + id_str + "_" + *login + "_entries.txt");

	file << title << "\n" << flatten(outbase64_un) << "\n" << flatten(outbase64_pw) << "\n" << flatten(outbase64_cmt);
	file.close();
}

//aus Datei lesen
void entry_output(std::shared_ptr<std::string> login, const int id){
	std::string title, username_entry, password_entry, comment_entry;
	
	char choice;
	
	std::string id_str = std::to_string(id);
	
	//Datei öffnen
	std::ifstream read("entries/" + *login + "_entries/" + id_str + "_" + *login + "_entries.txt");
	getline(read, title);
	getline(read, username_entry);
	getline(read, password_entry);
	getline(read, comment_entry);
	
	
	std::string decoded_un {};
    decode(decoded_un, username_entry);
    
    std::string decoded_pw {};
    decode(decoded_pw, password_entry);
    
    std::string decoded_cmt {};
    decode(decoded_cmt, comment_entry);
	
	std::cout << "\n\nTitle: " << title << '\n';
	std::cout << "\n\nUsername: " << flatten(decoded_un) << '\n';
	std::cout << "\n\nPassword: " << flatten(decoded_pw) << '\n';
	std::cout << "\n\nComment: " << flatten(decoded_cmt) << '\n';
	
	std::cout <<"\n\nEnter any single key to return to the menu:";
	std::cin >> choice;
	
	
	
	
}

void selection(std::shared_ptr<std::string> login, int& number){
	int choice;
	int view_choice;
	
	std::cout << "\nYour choice: ";
	std::cin >> choice;
	
	if (choice == 1){
		//View entry
		std::cout << "The number of the entry you want to view: ";
		std::cin >> view_choice;
		
		entry_output(login, view_choice);
		system("clear");
		std::cout << "\n1. View Entry\n2. Add Entry\n3. Close Application" << "\n\n";
		list(login, number);
		
	}
	else if (choice == 2){
		entry_input(login, number + 1);
		system("clear");
		std::cout << "\n1. View Entry\n2. Add Entry\n3. Close Application" << "\n\n";
		list(login, number);
	}
	else if (choice == 3){
		//close Application
		return;
	}
	else {
		std::cout << "choice is not possible." << '\n';

		selection(login, number);
	}
	
	selection(login, number);
	
}


int main() {
	
	int choice;
	int number = 0;
	//auto account = std::make_shared<std::string>("standin"); //nullptr cannot be converted to string --> logic error

	std::cout << "\n1. Register\n2. Login\n3. Close Application\n\nYour choice: ";
	std::cin >> choice;
	
	if (choice == 1){
	registration();

	main();
	}
	else if (choice == 2) {
		bool status = login(account);

		if(!status){
			std::cout << "False Login!" << '\n';
			
			return 0;
		}
		else {
			std::cout << "Login successful" << '\n';
			std::this_thread::sleep_for(std::chrono::seconds(1));
			system("clear");
	
			std::cout << "\n1. View Entry\n2. Add Entry\n3. Close Application" << "\n\n";
			list(account, number);
			selection(account, number);
		}
	}
	else if (choice == 3) {
		return 0;
	}
	else if (choice > 3){
		system("clear");
		std::cout << "\n1Your choice is not possible.\nEnter one of the allowed numbers.\n";
		main();
	}
	else if (!(std::cin >> choice)){
		std::cin.clear(); //clear bad input flag
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); //discard input
		system("clear");
		std::cout << "\n2Your choice is not possible.\nEnter one of the allowed numbers.\n";
		main();
	}

	
	
	return EXIT_SUCCESS;
}
