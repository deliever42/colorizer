#include <iostream>
#include <sys/stat.h>
#include <windows.h>
#include <string>
#include <fstream>
#include <filesystem>
#include <random>
#include <iterator>
#include <algorithm>

using namespace std;
namespace fs = std::filesystem;

const char* KERNEL_URL = "https://raw.githubusercontent.com/richzhang/colorization/caffe/resources/pts_in_hull.npy";
const char* PROTOTXT_URL = "https://raw.githubusercontent.com/richzhang/colorization/caffe/colorization/models/colorization_deploy_v2.prototxt";
const char* MODEL_URL = "https://people.eecs.berkeley.edu/~rich.zhang/projects/2016_colorization/files/demo_v2/colorization_release_v2.caffemodel";
const char* PYTHON_URL = "https://www.python.org/ftp/python/3.11.0/python-3.11.0-amd64.exe";
const char* FFMPEG_URL = "https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-master-latest-win64-gpl.zip";
const char* PYTHON_SCRIPT_URL = "https://raw.githubusercontent.com/deliever42/colorizer/scripts/colorize_image.py";

fs::path currentPath = fs::current_path();

void cls() {
	system("cls");
}

string randomString(int len)
{
	string result = "";
	char charset[] = { 
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I',
		'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r',
		'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R',
		's', 't', 'u', 'v', 'w', 'x', 'y', 'z', 
		'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };

	random_device r;
	
	seed_seq seed{ r(), r(), r(), r(), r(), r(), r(), r() };
	
	shuffle(begin(charset), end(charset), mt19937(seed));
	
	for (int i = 0; i < len; i++)
	{
		result += charset[rand() % (sizeof(charset) - 1)];
	}

	return result;
}

void exitApp(int closeTimeout) {
	cout << "Exiting..." << endl;

	Sleep(closeTimeout * 1000);
	exit(0);
}

void restartApp() {
	cout << "Restarting..." << endl;
	Sleep(3000);

	string filename = randomString(36) + ".bat";
	ofstream file(filename);

	file << "@echo off" << endl;
	file << "start \"\" \"" + currentPath.string() + "\\Colorizer.exe\"" << endl;
	file << "exit" << endl;

	file.close();

	system(filename.c_str());
	fs::remove(filename.c_str());
	exit(0);
}

bool checkPython() {
	if (system("py --version") != 0) {
		return false;
	}

	return true;
}

bool checkFFmpeg() {
	if (system("ffmpeg -version") != 0) {
		return false;
	}

	if (system("ffprobe -version") != 0) {
		return false;
	}

	return true;
}

bool checkDependencies() {
	if (!fs::exists(currentPath.string() + "\\bin")) {
		return false;
	}

	if (!fs::exists(currentPath.string() + "\\bin\\models")) {
		return false;
	}

	if (!fs::exists(currentPath.string() + "\\bin\\kernel")) {
		return false;
	}

	if (!fs::exists(currentPath.string() + "\\bin\\models\\colorization_deploy_v2.prototxt")) {
		return false;
	}

	if (!fs::exists(currentPath.string() + "\\bin\\models\\colorization_release_v2.caffemodel")) {
		return false;
	}

	if (!fs::exists(currentPath.string() + "\\bin\\kernel\\pts_in_hull.npy")) {
		return false;
	}

	if (!fs::exists(currentPath.string() + "\\bin\\colorize_image.py")) {
		return false;
	}

	if (!checkPython()) {
		return false;
	}

	if (!checkFFmpeg()) {
	    return false;
	}
	
	return true;
}

void downloadFile(const char* url, const char* path) {
	string command = "curl -L -o \"" + string(path) + "\" \"" + string(url) + "\"";
	system(command.c_str());
}

void installPython() {
	string destPath = fs::temp_directory_path().string() + "\\" + "py-" + randomString(6) + ".exe";

	downloadFile(PYTHON_URL, destPath.c_str());
	
	string command = destPath + string(" /quiet InstallAllUsers=1 PrependPath=1 Include_test=0");
	
	system(command.c_str());
	fs::remove(destPath.c_str());
	
}

void installFFmpeg() {
	string destPath = fs::temp_directory_path().string() + "\\" + "ffmpeg-" + randomString(6) + ".zip";

	downloadFile(FFMPEG_URL, destPath.c_str());

	string command = "powershell.exe -Command \"Expand-Archive -LiteralPath " + destPath + " -DestinationPath C:\\ -Force\"";

	system(command.c_str());
	fs::remove(destPath.c_str());

	command = "powershell.exe -Command \"[Environment]::SetEnvironmentVariable('Path', $env:Path + ';C:\\ffmpeg-master-latest-win64-gpl\\bin', [EnvironmentVariableTarget]::Machine)\"";
	system(command.c_str());
}

void downloadDependencies() {
	//step 1: create bin folder
	if (!fs::is_directory(currentPath.string() + "\\bin")) {
		fs::create_directory(currentPath.string() + "\\bin");
		cout << "[1/9] Created bin folder" << endl;
	}
	else {
		cout << "[1/9] Bin folder already exists" << endl;
	}

	//step 2: create bin\models folder
	if (!fs::is_directory(currentPath.string() + "\\bin\\models")) {
		fs::create_directory(currentPath.string() + "\\bin\\models");
		cout << "[2/9] Created models folder" << endl;
	}
	else {
		cout << "[2/9] Models folder already exists" << endl;
	}

	//step 3: create bin\kernel folder
	if (!fs::is_directory(currentPath.string() + "\\bin\\kernel")) {
		fs::create_directory(currentPath.string() + "\\bin\\kernel");
		cout << "[3/9] Created kernel folder" << endl;
	}
	else {
		cout << "[3/9] Kernel folder already exists" << endl;
	}

	//step 4: download pts_in_hull.npy
	if (!fs::exists(currentPath.string() + "\\bin\\kernel\\pts_in_hull.npy")) {
		downloadFile(KERNEL_URL, (currentPath.string() + "\\bin\\kernel\\pts_in_hull.npy").c_str());
		cout << "[4/9] Downloaded pts_in_hull.npy" << endl;
	}
	else {
		cout << "[4/9] pts_in_hull.npy already exists" << endl;
	}

	//step 5: download colorization_deploy_v2.prototxt
	if (!fs::exists(currentPath.string() + "\\bin\\models\\colorization_deploy_v2.prototxt")) {
		downloadFile(PROTOTXT_URL, (currentPath.string() + "\\bin\\models\\colorization_deploy_v2.prototxt").c_str());
		cout << "[5/9] Downloaded colorization_deploy_v2.prototxt" << endl;
	}
	else {
		cout << "[5/9] colorization_deploy_v2.prototxt already exists" << endl;
	}

	//step 6: download colorization_release_v2.caffemodel
	if (!fs::exists(currentPath.string() + "\\bin\\models\\colorization_release_v2.caffemodel")) {
		downloadFile(MODEL_URL, (currentPath.string() + "\\bin\\models\\colorization_release_v2.caffemodel").c_str());
		cout << "[6/9] Downloaded colorization_release_v2.caffemodel" << endl;
	}
	else {
		cout << "[6/9] colorization_release_v2.caffemodel already exists" << endl;
	}

	//step 7: install python
	if (!checkPython()) {
		installPython();
		cout << "[7/9] Downloaded python" << endl;
	}
	else {
		cout << "[7/9] Python already exists" << endl;
	}

	//step 8: install ffmpeg
	if (!checkFFmpeg()) {
		installFFmpeg();
		
		cout << "[8/9] FFmpeg installed" << endl;
	}
	else {
		cout << "[8/9] FFmpeg already exists" << endl;
	}

	//step 9: download colorize_image.py
	if (!fs::exists(currentPath.string() + "\\bin\\colorize_image.py")) {
		downloadFile(PYTHON_SCRIPT_URL, (currentPath.string() + "\\bin\\colorize_image.py").c_str());
		cout << "[9/9] Downloaded colorize_image.py" << endl;
	}
	else {
		cout << "[9/9] colorize_image.py already exists" << endl;
	}
	
	restartApp();
}

void colorizeImage(const char* imagePath, const char* destinationPath) {
	string command = "cd bin && py colorize_image.py --imagePath " + string(imagePath) + " --destinationPath " + string(destinationPath);
	system(command.c_str());
}

int main()
{
	cout << "[1] Colorize image\n[2] Colorize video\nCopyright (C) Deliever42 (https://deliever42.me)" << endl;
	cout << "\nPlease enter your choice: ";

	int choice;
	cin >> choice;

	if (choice == 1) {
		if (!checkDependencies()) {
			cls();
			cout << "Dependencies not found, downloading..." << endl;
			
			downloadDependencies();
		}
		else {
			cls();
			cout << "Please enter filename: ";

			string basename;
			cin >> basename;

			if (!fs::exists(currentPath.string() + "\\" + basename)) {
				cout << "File not found!" << endl;
				exitApp(3);
			}
			else {
				cls();
				cout << "Image colorizing..." << endl;
				
				string filename = basename.substr(0, basename.find_last_of("."));
				string extname = basename.substr(basename.find_last_of(".") + 1);
				
				string destinationFilename = filename + "_colorized-" + randomString(6) + "." + extname;

				colorizeImage(("../" + basename).c_str(), ("../" + destinationFilename).c_str());

				cls();
				cout << "Colorized image saved as " << destinationFilename << endl;
				exitApp(3);
			}
		}

	}
	else if (choice == 2) {
		if (!checkDependencies()) {
			cls();
			cout << "Dependencies not found, downloading..." << endl;

			downloadDependencies();
		}
		else {
			cls();
			cout << "Please enter filename: ";

			string basename;
			cin >> basename;

			if (!fs::exists(currentPath.string() + "\\" + basename)) {
				cout << "File not found!" << endl;
				exitApp(3);
			}
			else {
				cls();
				
				//step 1: create directories
				cout << "[1/9] Creating directories..." << endl;
				string destPath = fs::temp_directory_path().string() + "\\" + randomString(12) + "\\";
				fs::create_directory(destPath);
				fs::create_directory(destPath + "\\frames");
				fs::create_directory(destPath + "\\colorized_frames");

				//step 2: extract audio
				cout << "[2/9] Extracting audio..." << endl;
				system(("ffmpeg -i " + currentPath.string() + "\\" + basename + " " + destPath + "\\audio.mp3 -hide_banner -loglevel error").c_str());

				//step 3: extract fps
				cout << "[3/9] Extracting fps..." << endl;
				system(("ffprobe -v error -select_streams v:0 -of default=noprint_wrappers=1:nokey=1 -show_entries stream=r_frame_rate " + currentPath.string() + "\\" + basename + " > " + destPath + "\\fps.txt").c_str());
				
				ifstream fpsFile(destPath + "\\fps.txt");
				string fps;
				getline(fpsFile, fps);
				fpsFile.close();

				//step 4: extract pixel format
				cout << "[4/9] Extracting pixel format..." << endl;
				system(("ffprobe -v error -select_streams v:0 -of default=noprint_wrappers=1:nokey=1 -show_entries stream=pix_fmt " + currentPath.string() + "\\" + basename + " > " + destPath + "\\fps.txt").c_str());

				ifstream pixelFormatFile(destPath + "\\pixel_format.txt");
				string pixelFormat;
				getline(pixelFormatFile, pixelFormat);
				pixelFormatFile.close();

				//step 5: extract frames
				cout << "[5/9] Extracting frames..." << endl;
				system(("ffmpeg -i " + currentPath.string() + "\\" + basename + " -framerate " + fps + " " + destPath + "\\frames\\frame-%d.png -hide_banner -loglevel error").c_str());

				//step 6: colorize frames
				cout << "[6/9] Colorizing frames..." << endl;
				for (const auto& entry : fs::directory_iterator(destPath + "\\frames")) {
					string filename = entry.path().filename().string();
					colorizeImage((destPath + "\\frames\\" + filename).c_str(), (destPath + "\\colorized_frames\\" + filename).c_str());
				}
				
				//step 7: create video
				cout << "[7/9] Creating video..." << endl;
				string extname = basename.substr(basename.find_last_of(".") + 1);
				system(("ffmpeg -framerate " + fps + " -i " + destPath + "\\colorized_frames\\frame-%d.png -i " + destPath + "\\audio.mp3 -c:v libx264 -crf 23 -preset slow -pix_fmt " + pixelFormat + " -c:a copy " + destPath + "\\colorized_video." + extname + " -hide_banner -loglevel error").c_str());

				//step 8: move video to current directory
				cout << "[8/9] Moving video..." << endl;
				string destinationFilename = basename.substr(0, basename.find_last_of(".")) + "_colorized-" + randomString(6) + "." + extname;
				fs::copy(destPath + "\\colorized_video." + extname, currentPath.string() + "\\" + destinationFilename);
				
				//step 9: delete directories
				cout << "[9/9] Deleting directories..." << endl;
				fs::remove_all(destPath);
				
				cls();
				cout << "Colorized video saved as " << destinationFilename << endl;
				exitApp(3);
			}
		}
	}
	else {
		cls();
		cout << "Invalid choice was provided." << endl;
		exitApp(3);
	}

	return 0;
}

// Copyright (C) Deliever (https://deliever42.me)