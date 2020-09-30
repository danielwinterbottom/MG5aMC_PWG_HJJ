// g++ -Wall -o mergeLheFiles mergeLheFiles.cpp

#include <iostream>
#include <fstream>
#include <vector>



int main(int argc, char** argv)
{
  if(argc < 3)
  {
    std::cout << ">>>splitLheFile.cpp::Usage:   " << argv[0] << "   initialFile.lhe   fileToAdd1.lhe   fileToAdd2.lhe ..." << std::endl;
    return -1;
  }
  
  char* initialFileName = argv[1]; 
  std::cout << "initialFileName = " << initialFileName << std::endl;
  
  std::vector<char*> fileToAddNames;
  for(int fileIt = 0; fileIt < argc-2; ++fileIt)
  {
    fileToAddNames.push_back( argv[2+fileIt] );
    std::cout << "fileToAddName = " << fileToAddNames.at(fileIt) << std::endl;
  }
  
  
  // open lhe file
  std::ifstream initialFile(initialFileName, std::ios::in);
  std::ofstream outFile("out.lhe", std::ios::out);
  
  std::string line;
  std::string line2;
  bool writeEvent = false;
  int eventIt = 0;
  
  while(!initialFile.eof())
  {
    getline(initialFile, line);
    if( !initialFile.good() ) break;
  

    if( line != "</LesHouchesEvents>") {
      //write all lines in first file before this one
      outFile << line << std::endl;
    } else if (line == "</LesHouchesEvents>"){

      for(int fileIt = 0; fileIt < argc-2; ++fileIt)
      {
        std::ifstream fileToAdd(fileToAddNames.at(fileIt), std::ios::in);
        
        while(!fileToAdd.eof())
        {
          getline(fileToAdd, line2); 
          
          // decide whether to skip event or not 
          if( line2 == "<event>" )
          {
            ++eventIt;
            writeEvent = true;
          }
                
          
          // write line to outFile
          if(writeEvent == true)
            outFile << line2 << std::endl;
          
          
          // end of event
          if( line2 == "</event>" )
            writeEvent = false;
        }
      }
      outFile << "</LesHouchesEvents>" << std::endl;
      break;
    } 
    
  }
  
  
  std::cout << "Added " << eventIt << " events from " << argc-2 << " files to file " << initialFileName << std::endl;
  return 0;
}
