#include "game.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "../Engine3D/stb_image.h"
#include <stdio.h>
#include <iostream>
#include "scene.h"
#include <Windows.h>
#include <random>
#include <fstream>

char actions[] = { 'U', 'D', 'L', 'R', 'F', 'B' };

static void printMat(const glm::mat4 mat)
{
	std::cout << " matrix:" << std::endl;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
			std::cout << mat[j][i] << " ";
		std::cout << std::endl;
	}
}
std::string RotateFaceClockwise(std::string rep, int start_index);
std::string RotateFaceCounterClockwise(std::string rep, int start_index);
std::string CubeAction(char action, int direction, std::string rep);
void writeToTxtFile(const std::string& str, const std::string& filename);
std::string solveCube(const std::string& cubeRepresentation);
Game::Game() : Scene(){}

Game::Game(float angle, float relationWH, float near1, float far1) : Scene(angle, relationWH, near1, far1)
{
	rCube = RubikCube(this);
	isAnimating = 0.0f;
}

void Game::Init()
{
	AddShader("../res/shaders/pickingShader");
	AddShader("../res/shaders/basicShader");
	AddTexture("../res/textures/plane.png", false);
	int counter = 0;
	for (int i = -1; i <= 1; i++) {
		for (int j = -1; j <= 1; j++) {
			for (int k = -1; k <= 1; k++) {
				if (i != 0 || j != 0 || k != 0) {
					AddShape(Cube, -1, TRIANGLES);
					shapes[counter]->MyScale({ 0.5,0.5, 0.5 });
					shapes[counter]->MyTranslate({ i,j,k }, 0);
					SetShapeTex(counter, 0);
					counter++;
				}
			}
		}
	}
	cubeRep = "UUUUUUUUURRRRRRRRRFFFFFFFFFDDDDDDDDDLLLLLLLLLBBBBBBBBB";
	printf("%d", counter);
	pickedShape = 1;
	MoveCamera(0, zTranslate, 20);
	pickedShape = -1;

	

	//ReadPixel(); //uncomment when you are reading from the z-buffer
}

void Game::Update(const glm::mat4& MVP, const glm::mat4& Model, const int  shaderIndx)
{
	if (!isAnimating && !moves.empty()) {
		char currentMove = moves.front();
		printf("pressed %c\n", currentMove);
		if (currentMove == 'R' || currentMove == 'L' || currentMove == 'U' || currentMove == 'D' ||  currentMove == 'F' || currentMove == 'B')
			isAnimating = 1;
		moves.pop();
		if (currentMove == 'R') {
			
			cubeRep = CubeAction('R', rCube.direction, cubeRep);
			rCube.setArray(rCube.right, rotatingWall, 1);
			delta = glm::vec3(-1, 0, 0);
			if (rCube.direction == 1) rCube.RotateFaceClockWise(rCube.right);
			else rCube.RotateFaceCounterClockWise(rCube.right);
		}
		else if (currentMove == 'L') {
			cubeRep = CubeAction('L', rCube.direction, cubeRep);
			rCube.setArray(rCube.left, rotatingWall, 1);
			delta = glm::vec3 (1, 0, 0);
			if (rCube.direction == 1) rCube.RotateFaceClockWise(rCube.left);
			else rCube.RotateFaceCounterClockWise(rCube.left);
		}
		else if (currentMove == 'U') {
			cubeRep = CubeAction('U', rCube.direction, cubeRep);
			rCube.setArray(rCube.top, rotatingWall, 1);
			delta = glm::vec3 (0, -1, 0);
			if (rCube.direction == 1) rCube.RotateFaceClockWise(rCube.top);
			else rCube.RotateFaceCounterClockWise(rCube.top);
		}
		else if (currentMove == 'D') {
			cubeRep = CubeAction('D', rCube.direction, cubeRep);
			rCube.setArray(rCube.bottom, rotatingWall, 1);
			delta = glm::vec3(0, 1, 0);
			if (rCube.direction == 1) rCube.RotateFaceClockWise(rCube.bottom);
			else rCube.RotateFaceCounterClockWise(rCube.bottom);
		}
		else if (currentMove == 'F') {
			cubeRep = CubeAction('F', rCube.direction, cubeRep);
			rCube.setArray(rCube.front, rotatingWall, 1);
			delta = glm::vec3 (0, 0, -1);
			if (rCube.direction == 1) rCube.RotateFaceClockWise(rCube.front);
			else rCube.RotateFaceCounterClockWise(rCube.front);
		}
		else if (currentMove == 'B') {
			cubeRep = CubeAction('B', rCube.direction, cubeRep);
			rCube.setArray(rCube.back, rotatingWall, 1);
			delta = glm::vec3(0, 0, 1);
			if (rCube.direction == 1) rCube.RotateFaceClockWise(rCube.back);
			else rCube.RotateFaceCounterClockWise(rCube.back);
			
		}
		else if (currentMove == ' ') {
			if (rCube.direction == 1) rCube.direction = -1;
			else rCube.direction = 1;
		}
		else if (currentMove == 'Z') {
			if (rCube.rotationCount == 2.0f) rCube.rotationCount = 1.0f;
		}
		else if (currentMove == 'A') {
			if (rCube.rotationCount == 1.0f) rCube.rotationCount = 2.0f;
		}
		else if (currentMove == 'M') {
			generateRandomActionSequence();
		}
		else if (currentMove == 'S') {
			CubeSolver(cubeRep);
		}
	}

	if (isAnimating) {
		rCube.rotateWall(rotatingWall, delta);
		isAnimating = isAnimating + 0.01f;
		if (rCube.rotationCount == 1) {
			if (isAnimating >= 91.0f) {
				isAnimating = 0;
			}
		}
		else {
			if (isAnimating >= 181.0f) {
				isAnimating = 0;
			}
		}
	}
	Shader* s = shaders[shaderIndx];
	s->Bind();
	s->SetUniformMat4f("MVP", MVP);
	s->SetUniformMat4f("Normal", Model);
	s->Unbind();
}

void Game::WhenRotate(){}
void Game::WhenTranslate(){}
void Game::Motion() {if (isActive){}}

Game::~Game(void) {}
void RubikCube::setArray(int* from[3][3], int* (&to)[3][3], int shouldCopy) {
	if (!shouldCopy)
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
				to[i][j] = from[i][j];
		}
	else {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
			{
				int* temp = new int;
				*temp = *from[i][j];
				to[i][j] = temp;
			}
		game->isAnimating = 1;
	}
}

RubikCube::RubikCube(Game* game)
{
	this->game = game;
	direction = 1;			//0 is clockwise, 1 is counter clockwise
	rotationCount = 1.0f;		//amount of rotations
	int** arr = (int**)malloc(26*sizeof(int*));
	for (int i = 0; i < 26; i++) {
		arr[i] = (int*)malloc(sizeof(int));
		*arr[i] = i;
	}
	int* tempFront[3][3] = {{arr[2],arr[11],arr[19]},{arr[5],arr[13],arr[22]},{arr[8],arr[16],arr[25]}};
	int* tempBack[3][3] = {{arr[17],arr[9],arr[0]},{arr[20],arr[12],arr[3]},{arr[23],arr[14],arr[6]}};
	int* tempTop[3][3] = {{arr[8],arr[16],arr[25]},{arr[7],arr[15],arr[24]},{arr[6],arr[14],arr[23]}};
	int* tempBottom[3][3] = {{arr[0],arr[9],arr[17]},{arr[1],arr[10],arr[18]},{arr[2],arr[11],arr[19]}};
	int* tempLeft[3][3] = {{arr[0],arr[1],arr[2]},{arr[3],arr[4],arr[5]},{arr[6],arr[7],arr[8]}};
	int* tempRight[3][3] = {{arr[19],arr[18],arr[17]},{arr[22],arr[21],arr[20]},{arr[25],arr[24],arr[23]}};
	setArray(tempFront, front, 0); setArray(tempBack, back, 0); setArray(tempTop, top,0);
	setArray(tempBottom, bottom, 0); setArray(tempLeft, left, 0); setArray(tempRight, right, 0);
}

void RubikCube::rotateWall(int* wall[3][3], glm::vec3 delta) {
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++)
		{
			game->getShapes()[*wall[i][j]]->MyRotate(direction * 0.01f, delta, 0);
		}
	}
}

void RubikCube::RotateFaceClockWise(int* (&face)[3][3]) {
	for (int i = 0; i < rotationCount; i++) {
		int prev[] = { *face[1][0], *face[2][1], *face[2][0] };
		*face[2][0] = *face[0][0];
		*face[0][0] = *face[0][2];
		*face[1][0] = *face[0][1];

		*face[0][1] = *face[1][2];
		*face[2][1] = prev[0];

		*face[0][2] = *face[2][2];
		*face[1][2] = prev[1];
		*face[2][2] = prev[2];
	}
}
void RubikCube::RotateFaceCounterClockWise(int* (&face)[3][3]) {
	for(int i = 0; i < 3; i ++) RotateFaceClockWise(face);
}

std::string solveCube(const std::string& cubeRepresentation) {
	// Specify the relative path to the kociemba.exe executable
	std::string executablePath = "C:/Users/abdsa/Documents/Courses/Graphics/Rubiks/Graphics-Assignment3-Rubiks2/res/kociemba.exe";

	// Construct the command to execute kociemba.exe with the cube representation as an argument
	std::string command = executablePath + " " + cubeRepresentation;

	// Execute the command and capture its output
	std::string result = "";

	// Use popen() to run the command
	FILE* pipe = _popen(command.c_str(), "r");
	if (!pipe) {
		std::cerr << "Error: Could not execute command." << std::endl;
		return "";
	}

	char buffer[128];
	while (!feof(pipe)) {
		if (fgets(buffer, 128, pipe) != NULL)
			result += buffer;
	}

	_pclose(pipe);

	return result;
}

void writeToTxtFile(const std::string& str, const std::string& filename) {
	// Open the file in output mode
	std::ofstream outputFile(filename);

	// Check if the file is successfully opened
	if (outputFile.is_open()) {
		// Write the string to the file
		outputFile << str;

		// Close the file
		outputFile.close();

		// Output success message
		std::cout << "String successfully written to " << filename << std::endl;
	}
	else {
		// Output error message if the file couldn't be opened
		std::cerr << "Unable to open file " << filename << " for writing." << std::endl;
	}
}

void Game::simulateKeystrokes(const std::string& keys) {
	// Iterate over each character in the string
	for (char key : keys) {
		moves.push(key);
	}
}
std::string Game::generateRandomActionSequence() {
	std::random_device rd; // Obtain a random number from hardware
	std::mt19937 eng(rd()); // Seed the random number generator
	std::uniform_int_distribution<> distr(0, 5); // Define the range

	std::string actionSequence;
	for (int i = 0; i < 8; ++i) {
		int index = distr(eng); // Generate a random index
		actionSequence += actions[index]; // Add a random action to the sequence
	}
	writeToTxtFile(actionSequence, "mixer.txt");
	simulateKeystrokes(actionSequence);
	return actionSequence;
}
std::string RotateFaceClockwise(std::string rep, int start_index) {
	char temp = rep[start_index];
	rep[start_index] = rep[start_index + 6];
	rep[start_index + 6] = rep[start_index + 8];
	rep[start_index + 8] = rep[start_index + 2];
	rep[start_index + 2] = temp;
	temp = rep[start_index + 1];
	rep[start_index + 1] = rep[start_index + 3];
	rep[start_index + 3] = rep[start_index + 7];
	rep[start_index + 7] = rep[start_index + 5];
	rep[start_index + 5] = temp;
	return rep;
}

std::string RotateFaceCounterClockwise(std::string rep, int start_index) {
	char temp = rep[start_index];
	rep[start_index] = rep[start_index + 2];
	rep[start_index + 2] = rep[start_index + 8];
	rep[start_index + 8] = rep[start_index + 6];
	rep[start_index + 6] = temp;
	temp = rep[start_index + 1];
	rep[start_index + 1] = rep[start_index + 5];
	rep[start_index + 5] = rep[start_index + 7];
	rep[start_index + 7] = rep[start_index + 3];
	rep[start_index + 3] = temp;
	return rep;
}

std::string CubeAction(char action, int direction, std::string rep) {
	char temp;
	char temp2;
	char temp3;
	switch (action) {
	case 'U':
		if (direction == 1) {
			rep = RotateFaceClockwise(rep, 0); // Rotate up face clockwise
			temp = rep[36];
			temp2 = rep[37];
			temp3 = rep[38];//coppy L top

			rep[36] = rep[18];//move F top to L top
			rep[37] = rep[19];
			rep[38] = rep[20];

			rep[18] = rep[9];//move R top to F top
			rep[19] = rep[10];
			rep[20] = rep[11];

			rep[9] = rep[45];//move B top to R top
			rep[10] = rep[46];
			rep[11] = rep[47];

			rep[45] = temp;//move L top to B top
			rep[46] = temp2;
			rep[47] = temp3;
		}
		else if (direction == -1) {
			rep = RotateFaceCounterClockwise(rep, 0); // Rotate up face counter-clockwise

			temp = rep[36];
			temp2 = rep[37];
			temp3 = rep[38];//coppy L top

			rep[36] = rep[45];//move B top to L top
			rep[37] = rep[46];
			rep[38] = rep[47];

			rep[45] = rep[9];//move R top to B top
			rep[46] = rep[10];
			rep[47] = rep[11];

			rep[9] = rep[18];//move f top to R top
			rep[10] = rep[19];
			rep[11] = rep[20];

			rep[18] = temp;//move L top to B top
			rep[19] = temp2;
			rep[20] = temp3;
		}
		break;
	case 'D':
		if (direction == 1) {
			rep = RotateFaceClockwise(rep, 27); // Rotate down face clockwise
			temp = rep[42];
			temp2 = rep[43];
			temp3 = rep[44];//coppy L bottom

			rep[42] = rep[51];//move B bottom to L bottom
			rep[43] = rep[52];
			rep[44] = rep[53];

			rep[51] = rep[15];//move R bottom to B bottom
			rep[52] = rep[16];
			rep[53] = rep[17];

			rep[15] = rep[24];//move F bottom to R bottom
			rep[16] = rep[25];
			rep[17] = rep[26];

			rep[24] = temp;//move L bottom to F bottom
			rep[25] = temp2;
			rep[26] = temp3;
			
		}
		else if (direction == -1) {
			rep = RotateFaceCounterClockwise(rep, 27); // Rotate down face counter-clockwise
			temp = rep[42];
			temp2 = rep[43];
			temp3 = rep[44];//coppy L bottom

			rep[42] = rep[24];//move F bottom to L bottom
			rep[43] = rep[25];
			rep[44] = rep[26];

			rep[24] = rep[15];//move R bottom to F bottom
			rep[25] = rep[16];
			rep[26] = rep[17];

			rep[15] = rep[51];//move B bottom to R bottom
			rep[16] = rep[52];
			rep[17] = rep[53];

			rep[51] = temp;//move L bottom to F bottom
			rep[52] = temp2;
			rep[53] = temp3;
			
		}
		break;
	case 'F':
		if (direction == 1) {
			rep = RotateFaceClockwise(rep, 18); // Rotate front face clockwise

			temp = rep[6];
			temp2 = rep[7];
			temp3 = rep[8];//coppy U bottom

			rep[6] = rep[44];//move L right to U bottom
			rep[7] = rep[41];
			rep[8] = rep[38];

			rep[38] = rep[27];//move D top to L right
			rep[41] = rep[28];
			rep[44] = rep[29];

			rep[27] = rep[15];//move R left to D top
			rep[28] = rep[12];
			rep[29] = rep[9];

			rep[9] = temp;//move L bottom to F bottom
			rep[12] = temp2;
			rep[15] = temp3;
			
		}
		else if (direction == -1) {
			rep = RotateFaceCounterClockwise(rep, 18); // Rotate front face counter-clockwise
			temp = rep[6];
			temp2 = rep[7];
			temp3 = rep[8];//coppy U bottom

			rep[6] = rep[9];//move R left to U bottom
			rep[7] = rep[12];
			rep[8] = rep[15];

			rep[15] = rep[27];//move D top to R left
			rep[12] = rep[28];
			rep[9] = rep[29];

			rep[27] = rep[38];//move L right to D top
			rep[28] = rep[41];
			rep[29] = rep[44];

			rep[44] = temp;
			rep[41] = temp2;
			rep[38] = temp3;
		}
		break;
	case 'B':
		if (direction == 1) {
			rep = RotateFaceClockwise(rep, 45); // Rotate back face clockwise
			temp = rep[0];
			temp2 = rep[1];
			temp3 = rep[2];//coppy U top

			rep[0] = rep[11];//move R right to U top
			rep[1] = rep[14];
			rep[2] = rep[17];

			rep[11] = rep[35];//move D bottom to R right
			rep[14] = rep[34];
			rep[17] = rep[33];

			rep[33] = rep[36];//move L left to D bottom
			rep[34] = rep[39];
			rep[35] = rep[42];

			rep[42] = temp;
			rep[39] = temp2;// U top to L left
			rep[36] = temp3;
			
		}
		else if (direction == -1) {
			rep = RotateFaceCounterClockwise(rep, 45); // Rotate back face counter-clockwise
			for (int i = 0; i < 3; i++) {//B3 is same as B'
				temp = rep[0];
				temp2 = rep[1];
				temp3 = rep[2];//coppy U top

				rep[0] = rep[11];//move R right to U top
				rep[1] = rep[14];
				rep[2] = rep[17];

				rep[11] = rep[35];//move D bottom to R right
				rep[14] = rep[34];
				rep[17] = rep[33];

				rep[33] = rep[36];//move L left to D bottom
				rep[34] = rep[39];
				rep[35] = rep[42];

				rep[42] = temp;
				rep[39] = temp2;// U top to L left
				rep[36] = temp3;
			}
		}
		break;
	case 'L':
		if (direction == 1) {
			// Rotate left face clockwise and update adjacent pieces
			rep = RotateFaceClockwise(rep, 36); // Left face
			temp = rep[0];
			temp2 = rep[3];
			temp3 = rep[6];//coppy U left

			rep[0] = rep[53];//move B right to U left
			rep[3] = rep[50];
			rep[6] = rep[47];

			rep[47] = rep[33];//move D left to B right
			rep[50] = rep[30];
			rep[53] = rep[27];

			rep[27] = rep[18];//move F left to D left
			rep[30] = rep[21];
			rep[33] = rep[24];

			rep[18] = temp;
			rep[21] = temp2;// U top to L left
			rep[24] = temp3;
			
		}
		else if (direction == -1) {
			// Rotate left face counter-clockwise and update adjacent pieces
			rep = RotateFaceCounterClockwise(rep, 36); // Left face
			for (int i = 0; i < 3; i++) {//B3 is same as B'
				temp = rep[0];
				temp2 = rep[3];
				temp3 = rep[6];//coppy U left

				rep[0] = rep[53];//move B right to U left
				rep[3] = rep[50];
				rep[6] = rep[47];

				rep[47] = rep[33];//move D left to B right
				rep[50] = rep[30];
				rep[53] = rep[27];

				rep[27] = rep[18];//move F left to D left
				rep[30] = rep[21];
				rep[33] = rep[24];

				rep[18] = temp;
				rep[21] = temp2;// U top to L left
				rep[24] = temp3;
			}
		}
		break;
	case 'R':
		if (direction == 1) {
			rep = RotateFaceClockwise(rep, 9); // Rotate right face clockwise
			temp = rep[2];
			temp2 = rep[5];
			temp3 = rep[8];//coppy U right

			rep[2] = rep[20];//move F right to U right
			rep[5] = rep[23];
			rep[8] = rep[26];

			rep[20] = rep[29];//move D right to F right
			rep[23] = rep[32];
			rep[26] = rep[35];

			rep[29] = rep[51];//move B left to D right
			rep[32] = rep[48];
			rep[35] = rep[45];

			rep[51] = temp;
			rep[48] = temp2;// U top to L left
			rep[45] = temp3;
		}
		else if (direction == -1) {
			rep = RotateFaceCounterClockwise(rep, 9); // Rotate right face counter-clockwise
			for (int i = 0; i < 3; i++) {//B3 is same as B'
				temp = rep[2];
				temp2 = rep[5];
				temp3 = rep[8];//coppy U right

				rep[2] = rep[20];//move F right to U right
				rep[5] = rep[23];
				rep[8] = rep[26];

				rep[20] = rep[29];//move D right to F right
				rep[23] = rep[32];
				rep[26] = rep[35];

				rep[29] = rep[51];//move B left to D right
				rep[32] = rep[48];
				rep[35] = rep[45];

				rep[51] = temp;
				rep[48] = temp2;// U top to L left
				rep[45] = temp3;
			}
		}
		break;

	default:
		std::cout << "Invalid action!" << std::endl;
		break;
	}
	return rep;
}

std::string Game::CubeSolver(std::string rep) {
	// Solve the cube
	std::string solution = solveCube(rep);
	std::cout << "Solution: " << solution << std::endl;
	std::string translated = "";
	boolean add = false;
	//translate
	for (int i = 0; i < solution.length(); i++) {
		
		if (i < solution.length() - 1 && solution[i] != ' ') {
			if (solution[i + 1] == '2') {
				translated += solution[i];
			}
			if (solution[i + 1] == 39) {
				translated += ' ';
				add = true;
			}
		}
		if (solution[i] != '2' && solution[i] != 39 && solution[i] != ' ') {
			translated += solution[i];

			if(add) translated += ' ';
			add = false;
		}
			
		
	}
	//push actions
	writeToTxtFile(translated, "solver.txt");
	simulateKeystrokes(translated);
	// Display the solution
	return translated;
}

