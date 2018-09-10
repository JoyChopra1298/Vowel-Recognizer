#include "stdafx.h"
#include "train.h"

void train(string training_dir_name){

	// Create output directories for R_i , LPC coefficients and cepstral coefficients
	string R_i_output = training_dir_name + string("\\output_R_i");
	CreateDirectory(R_i_output.c_str(), NULL);
	string a_i_output = training_dir_name + string("\\output_a_i");
	CreateDirectory(a_i_output.c_str(), NULL);
	string c_i_output = training_dir_name + string("\\output_c_i");
	CreateDirectory(c_i_output.c_str(), NULL);

	// create output directory for storing trimmed steady state sound
	string steady_state_output = training_dir_name + string("\\output_steady_state");
	CreateDirectory(steady_state_output.c_str(), NULL);

	// Traverse over all files in the training directory
	std::string pattern(training_dir_name);
	pattern.append("\\*.txt");
	WIN32_FIND_DATA data;
	HANDLE hFind;
	if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
		do {
			// Open the file
			string fileName = data.cFileName;
			string filePath = training_dir_name + string("\\") + fileName;

			// Get audio data from file
			vector<double> audio = get_audio_from_file(filePath);
			int num_samples = audio.size();
			int i;

			// Apply DC shift correction, normalise amplitude and trim silence.
			vector<int> markers = preprocess_audio(audio);
			int start_marker = markers[0];
			int end_marker = markers[1];

			// Create output files for R_i, LPC coefficients and cepstral coefficients
			string R_i_fileName = R_i_output + string("\\") + fileName;
			fstream R_i_file(R_i_fileName, ios::out);

			string a_i_fileName = a_i_output + string("\\") + fileName;
			fstream a_i_file(a_i_fileName, ios::out);

			string c_i_fileName = c_i_output + string("\\") + fileName;
			fstream c_i_file(c_i_fileName, ios::out);

			R_i_file << left << setprecision(output_float_precision) << fixed;
			a_i_file << left << setprecision(output_float_precision) << fixed;
			c_i_file << left << setprecision(output_float_precision) << fixed;

			R_i_file << setw(ouput_width) << "R_" + to_string(0) << " ";

			// write headers in each file
			for (i = 1; i <= number_of_coefficients; i++){
				R_i_file << setw(ouput_width) << "R_" + to_string(i) << " ";
				c_i_file << setw(ouput_width) << "c_" + to_string(i) << " ";
				a_i_file << setw(ouput_width) << "a_" + to_string(i) << " ";
			}
			R_i_file << endl;
			a_i_file << endl;
			c_i_file << endl;

			// take 10 steady state frames from the signal (middle portion of the trimmed signal)
			int steady_frame_start = start_marker + ((end_marker - start_marker) - (frame_size + (num_frames_to_analyse - 1) * frame_skip))/2;
			
			string steady_state_fileName = steady_state_output + string("\\") + fileName;
			fstream steady_state_file(steady_state_fileName, ios::out);

			steady_state_file << "SAMPLES: " << frame_size + frame_skip * (num_frames_to_analyse - 1) << endl;
			steady_state_file << "BITSPERSAMPLE:	16\nCHANNELS : 1\nSAMPLERATE : 16000\nNORMALIZED : FALSE" << endl;
			for (i = 0; i < frame_size + (num_frames_to_analyse - 1) * frame_skip; i++){
				steady_state_file << audio[steady_frame_start + i] << endl;
			}
			steady_state_file.close();

			for (int l = 0; l < num_frames_to_analyse; l++){

				//apply Hamming's window to the frame
				vector<long double> filtered_audio(frame_size, 0.0);
				long double sum = 0.0;
				for (int j = 0; j < frame_size; j++){
					double w_j = 0.54 - 0.46 * (cos(2 * (3.14159)*j / (frame_size - 1)));
					filtered_audio[j] = w_j * audio[j + steady_frame_start + (l *frame_skip)];
				}

				// calculate R_i
				vector<long double> R = calculate_Ri(filtered_audio);
				for (int i = 0; i <= number_of_coefficients; i++){
					R_i_file << setw(ouput_width) << R[i] << " ";
				}
				R_i_file << endl;

				// calculate a_i
				vector<long double> a = calculate_ai(R);
				for (int i = 1; i <= number_of_coefficients; i++){
					a_i_file << setw(ouput_width) << a[i] << " ";
				}
				a_i_file << endl;

				//calculate c_i
				vector<long double> c = calculate_ci(a,R[0]);
				for (int i = 1; i <= number_of_coefficients; i++){
					c_i_file << setw(ouput_width) << c[i] << " ";
				}
				c_i_file << endl;
			}
			R_i_file.close();
			a_i_file.close();
			c_i_file.close();

			cout << "Finished for file - " << fileName << endl;
		} while (FindNextFile(hFind, &data) != 0);
		FindClose(hFind);
	}
}
