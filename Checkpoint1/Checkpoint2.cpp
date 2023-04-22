// Checkpoint1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <locale>
#include <codecvt>
#include "boost/regex.hpp"
#include "libstemmer.h"
using namespace std;


struct WordInfo {
    //map<int, int> docFrequency;
    int docFrequency = 1;
    int currentDoc = 0;
    //int globalFrequency = 1;
    int wordCode = 0;
    //doc id , tf
    map<int, int> iIndex;
};



//sort by word code frequency 
bool compare(pair<string, WordInfo>& a, pair<string, WordInfo>& b) {
    return a.second.wordCode < b.second.wordCode;
}

//write each index file
void printLargestToSmallest(unordered_map<string, WordInfo>& M, int index) {
    vector<pair<string, WordInfo>> A;
    string indexFile;
    if(index < 10)
        indexFile = "index00000" + to_string(index) + ".txt";
    else
        indexFile = "index0000" + to_string(index) + ".txt";


    

    // change to index 
    ofstream write(indexFile);
    if (write.is_open())
    {
        for (auto& it : M) {
            A.push_back(it);
        }
        sort(A.begin(), A.end(), compare);
        for (auto& it : A) {
            write << it.second.wordCode << " " << it.first << " " << it.second.docFrequency << " ";

            // print pairs
            for (auto& elem : it.second.iIndex)
            {
                write << "(" << elem.first << ", " << elem.second << ") ";
            }
            write << endl;
        }
        write.close();
    }

}

const int indexSize = 32;


int main()
{
    //filler words to ignore
    unordered_set<string> stopWords{ "a", "about", "actually", "almost", "also", "although", "always", "am", "an", "and", "any", "are", "as", "at", "be", "became", "become", "but", "by", "can", "could",
                                "did", "do", "does", "each", "either", "else,", "for", "from", "had", "has", "have", "he", "her", "his", "she", "him", "they", "their", "them" ,"the", "to", "too"
                                "hence", "how", "i", "if", "in", "is", "it", "its", "just", "may", "maybe", "me", "we", "there", "was", "that", "were", "this", "after",
                                "might", "mine", "must", "my,", "mine", "must", "my", "neither", "nor", "not", "now", "of", "oh", "on", "ok", "when", "where", "whereas", "wherever,", "whenever", "whether", "which",
                                "while", "who", "whom", "whoever", "whose", "why", "will", "with", "within", "without", "would", "yes", "yet", "you", "your" };

    map<string, WordInfo> wordMap;
    int doc = 0;

    unordered_map<string, WordInfo> invertedIndex[indexSize];

    string word = "";

 
    struct sb_stemmer * stemmer = sb_stemmer_new("english", nullptr);
    const sb_symbol* stemmed;
    string stemmed_string;
    int length;

    WordInfo tempAdd;
    string checkIfURL = "https:";
    map<string, WordInfo>::iterator it;
    unordered_map<string, WordInfo>::iterator indexIt;

    //regex filter("[^a-zA-Z]+");
    boost::regex e("[^a-zA-Z]+");

    string fileName = "wiki2022_small.0000";
    string fileNames[indexSize];

    for (int j = 0; j < indexSize; j++)
    {
        if (j < 10)
        {
            fileNames[j] = fileName + "0" + to_string(j);
        }
        else
        {
            fileNames[j] = fileName + to_string(j);
        }

    }

   
    for (int n = 0; n < indexSize; n++)
    {
        //C:\Users\salam\source\repos\Checkpoint2\Checkpoint1\wiki2022
        ifstream myfile(fileNames[n]);
        if (myfile.is_open())
        {
            while (myfile >> word)
            {

                if (word.substr(0, 6).compare(checkIfURL) != 0)
                {



                    //filters out two letter words and words that appear in stop word set
                    if (word.length() < 3)
                        continue;
                    transform(word.begin(), word.end(), word.begin(), ::tolower);
                    if (stopWords.count(word) == 1)
                        continue;
                    //token word
                    
                    word = boost::regex_replace(word, e, "");
                    
                    
                     //stem word
                    
                    stemmed = sb_stemmer_stem(stemmer, (const unsigned char*)word.c_str(), strlen(word.c_str()));                  
                    length = sb_stemmer_length(stemmer);
                    string word(reinterpret_cast<const char*>(stemmed), length);
                    
                                       
                     if (word.length() < 3)
                         continue;
                     transform(word.begin(), word.end(), word.begin(), ::tolower);
                     if (stopWords.count(word) == 1)
                         continue;


                    it = wordMap.find(word);
                    if (it != wordMap.end())
                    {
                        //it exists

                        //check to see if word exists in current invertedIndex
                        indexIt = invertedIndex[n].find(word);
                        if (indexIt == invertedIndex[n].end())
                        {
                            invertedIndex[n].insert({ word, tempAdd });
                            invertedIndex[n].at(word).iIndex.insert({ doc, 1 });
                        }

                        if (wordMap.at(word).currentDoc != doc)
                        {

                            wordMap.at(word).currentDoc = doc;                           
                            invertedIndex[n].at(word).iIndex[doc]++;
                            invertedIndex[n].at(word).docFrequency++;
                        }
                        else
                        {
                           
                            invertedIndex[n].at(word).iIndex[doc]++;
                        }
                    }
                    else
                    {
                        //it doesnt
                        
                        wordMap.insert({ word, tempAdd });
                        wordMap.at(word).currentDoc = doc;
                        invertedIndex[n].insert({ word, tempAdd });

                        invertedIndex[n].at(word).iIndex.insert({doc, 1});



        
                    }

                }
                else
                {
                    
                    word.erase(0, 36);
                    try {
                        doc = stoi(word);
                    }
                    catch (exception& e)
                    {
                        cout << "Error: " << doc << endl;
                    }
                }

            }
            myfile.close();
        }

        
    }

    sb_stemmer_delete(stemmer);

    // write to dictionary.txt
    ofstream write("dictionary.txt");

     if (write.is_open())
    {
 
        int i = 0;
        for (auto& it : wordMap)
        {
            write << it.first << endl;
            it.second.wordCode = i;
            for (int n = 0; n < indexSize; n++)
            {              
                indexIt = invertedIndex[n].find(it.first);
                if (indexIt != invertedIndex[n].end())
                {
                    invertedIndex[n].at(it.first).wordCode = i;
                }
            }
            
            i++;
        }
        for (int n = 0; n < indexSize; n++)
        {
            printLargestToSmallest(invertedIndex[n], n);
        }
        

        write.close();
    }




    return 0;
}
