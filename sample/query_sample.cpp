#include<cstdio>

#include<iostream>
#include<string>
#include<fstream>
#include<sstream>

#include<logger.h>
#include<vocabulary.h>
#include<dalm.h>

#define NGRAMORDER 5

using namespace std;

void push(DALM::VocabId *ngram, size_t n, DALM::VocabId wid){
	for(size_t i = n-1; i+1 >= 1 ; i--){
		ngram[i] = ngram[i-1];
	}
	ngram[0] = wid;
}

int main(int argc, char **argv){
	if(argc != 3){
		cerr << "Usage : " << argv[0] << " order dalm-dir" << endl;
		return 1;
	}
	
	unsigned char order = atoi(argv[1]);
    string path = argv[2];

    ////////////////
    // LOADING LM //
    ////////////////

    // Preparing a logger object.
    DALM::Logger logger(stderr);
    logger.setLevel(DALM::LOGGER_INFO);

    // Load ini file.
    DALM::Model ini_file(path, order, logger);

    // Load the vocabulary file.
    DALM::Vocabulary &vocab = *ini_file.vocab;

    // Load the language model.
    DALM::LM &lm = *ini_file.lm;

    ////////////////
    // WORD LIST  //
    ////////////////
    ifstream ifs(ini_file.wordstxt.c_str());
    string word;
    size_t word_count = 0;

	// a wordstxt file contains a word per line.
	getline(ifs, word);
	while(ifs){
		word_count++;
		getline(ifs, word);
	}
	cerr << "WORD COUNT=" << word_count << endl;


	//////////////
	// QUERYING //
	//////////////
	string stagstart = "<s>";
	string stagend = "</s>";
	DALM::VocabId wid_start = vocab.lookup(stagstart.c_str());
	DALM::VocabId wid_end = vocab.lookup(stagend.c_str());
	string line;
	DALM::VocabId ngram[NGRAMORDER];
	
	getline(cin, line);
	while(cin){
		for(size_t i = 0; i < NGRAMORDER; i++){
			ngram[i] = wid_start;
		}
		istringstream iss(line);
		string word;
		iss >> word;
		while(iss){
			// GETTING WORDID
			// If the word is an unknown word, WORDID is Vocabulary::unk().
			DALM::VocabId wid = vocab.lookup(word.c_str());
			push(ngram, NGRAMORDER, wid);

			float prob = 0.0;
			// QUERYING
			// Note that the ngram array is in reverse order.
			prob = lm.query(ngram, NGRAMORDER);

			cout << word << " => " << prob << endl;

			iss >> word;
		}
		push(ngram, NGRAMORDER, wid_end);
		float prob = lm.query(ngram, NGRAMORDER);
		cout << stagend << " => " << prob << endl;
		
		getline(cin, line);
	}

	return 0;
}

