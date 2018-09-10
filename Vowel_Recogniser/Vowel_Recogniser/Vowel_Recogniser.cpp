// Vowel_Recogniser.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "train.h"
#include "test.h"

int main(int argc, char* argv[])
{
	printf("Do you want to train before testing? Enter 0 or 1: ");
	int is_train = 0;
	scanf("%d", &is_train);

	/*	Training phase
		Input - Path to folder containing 5 txt files for each vowel 
		Output - R_i, LPC coeffiicients  and cepstral coefficients for 10 frames of each audio file
	*/
	string trainingDirName;
	if (is_train){
		printf("\nEnter complete path to the training directory\n");
		//All txt files in the directory are assumed to be training data.
		cin.clear();
		cin.sync();
		getline(cin, trainingDirName);
		printf("\nStarted training\n");
		train(trainingDirName);
		printf("Training finished. Please check the output folders generated in the provided training directory\n");
	}
	/*	Test phase
		Input - directory containing cepstral coefficient data. test file
		Output - average distance from each vowel. Predicted vowel
	*/
	//The folder containing cepstral coefficient
	string trained_files_dir;
	if (is_train){
		trained_files_dir = trainingDirName + string("\\output_c_i");
	}
	else{
		printf("\nEnter complete path of the directory containing cepstral coefficient files\n");
		cin.clear();
		cin.sync();
		getline(cin, trained_files_dir);
	}
	//The file to test against
	string test_file_path;
	printf("\nEnter complete path of test file\n");
	cin.clear();
	cin.sync();
	getline(cin, test_file_path);

	char predicted_vowel = test(trained_files_dir, test_file_path);
	printf("\nPrediction for provided file - %c\n", predicted_vowel);
	
	int t;
	scanf_s("%d", &t);
	return 0;
}

