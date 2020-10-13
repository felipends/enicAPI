#include <iostream>
#include <fstream>
#include "utils/InstanceGenerator.h"
#include "utils/InstanceReader.h"
#include "Heuristic.h"

using namespace std;

int runInstanceFromFile(string inputFilename, string outputFilename, string dictFilename)
{
   InstanceReader instanceReader(inputFilename);
   EventInfo *eventInfo = instanceReader.readInstance();

   Heuristic heuristic(eventInfo, dictFilename);
   int result = heuristic.solve(&instanceReader);

   heuristic.getOutput()->saveOutputIntoFile(outputFilename, (*eventInfo));

   return result;
}

void saveResult(string filename, int result) {
   std::ofstream outfile;
   outfile.open(filename, std::ios_base::app);
   outfile << result << endl;
}

int main(int argc, char** argv) {
   if (argc < 2) {
      cout << "Incorrect number of params!" << endl;
      cout << "You should provide at least two parameters" << endl;
      return 0;
   }

   //nome da instancia
   string intputFileName(argv[1]);
   //int result = runInstanceFromFile(intputFileName, argv[2]);
   if (argc == 4) {
		//nome do json
      string outputFileName(argv[2]);
	  //nome do arquivo dicionario
      string dictFilename(argv[3]);
      int result = runInstanceFromFile(intputFileName, outputFileName, dictFilename);
      // saveResult(outputFileName, result);
      
   }

   return 0; 
}
