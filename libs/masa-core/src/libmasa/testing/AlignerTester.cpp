/*
 * AlignerTester.cpp
 *
 *  Created on: May 4, 2013
 *      Author: edans
 */

#include "AlignerTester.hpp"

#include "../../common/cudalign.hpp"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static score_t best_score;
static FILE* lastRow;
static FILE* lastColumn;

static void processBestScoreFunction(score_t score, int bx, int by) {
	if (best_score.score < score.score) {
		best_score = score;
		fprintf(stderr, " (%d,%d,%d)\n", score.i, score.j, score.score);
	}
}

static void processLastRowFunction(int i, int j, int len, cell_t* data) {
	if (lastRow != NULL) {
		fwrite(data, sizeof(cell_t), len, lastRow);
	}
}

static void processLastColumnFunction(int i, int j, int len, cell_t* data) {
	if (lastColumn != NULL) {
		fwrite(data, sizeof(cell_t), len, lastColumn);
	}
}


AlignerTester::AlignerTester(IAligner* aligner) {
	this->aligner = aligner;
}

AlignerTester::~AlignerTester()
{
	// TODO Auto-generated destructor stub
}

void AlignerTester::compare_files(const char* file0, const char* file1) {
	FILE* f0 = fopen(file0, "rb");
	if (f0 == NULL) {
		fprintf(stderr, "File %s not found\n", file0);
		exit(1);
	}
	FILE* f1 = fopen(file1, "rb");
	if (f1 == NULL) {
		fprintf(stderr, "File %s not found\n", file1);
		exit(1);
	}
	char c0;
	char c1;
	int bytes = 0;
	int equal_bytes = 0;
	while (fread(&c0, 1, 1, f0) == 1 && fread(&c1, 1, 1, f1) == 1) {
		bytes++;
		if (c0 == c1)
			equal_bytes++;
	}
	if (ftell(f0) != ftell(f1)) {
		fprintf(stderr, "CHECK ERROR: File size Differs\n");
	}
	if (bytes == equal_bytes) {
		fprintf(stderr, "CHECK OK   : Identifical Files (%d bytes)\n", bytes);
	} else {
		fprintf(stderr, "CHECK ERROR: Different Files (%d bytes differs)\n",
				bytes - equal_bytes);
	}
	fclose(f0);
	fclose(f1);
}

bool AlignerTester::test(char* test_filename) {
	FILE* file = fopen(test_filename, "rt");
	if (file == NULL) {
		fprintf(stderr, "File %s not found\n", test_filename);
		exit(1);
	}

	string output_dir = "./test_output/";
	mkdir(output_dir.c_str(), 0774);
	string lastRowName = output_dir + "last_row.bin";
	string lastColumnName = output_dir + "last_column.bin";


	char line[1024];
	char cmd[3][1024];
	int i0 = 0;
	int i1 = 0;
	int j0 = 0;
	int j1 = 0;
    Sequence* seq[2];

	while (fgets(line, sizeof(line), file) != NULL) {
		fprintf(stderr, "%s", line);
		cmd[0][0] = '\0';
		cmd[1][0] = '\0';
		cmd[2][0] = '\0';
		sscanf(line, "%s %s %s", cmd[0], cmd[1], cmd[2]);
		if (strcmp(cmd[0], "set") == 0) {
			if (strcmp(cmd[1], "partition") == 0) {
				sscanf(line, "%*s %*s %d%d%d%d", &i0, &i1, &j0, &j1);
				printf("set partition %d %d %d %d\n", i0, i1, j0, j1);
			} else if (strcmp(cmd[1], "sequence") == 0) {
				int id;
				char fasta_file[1024];
				sscanf(line, "%*s %*s %d %s", &id, fasta_file);

		    	SequenceInfo* sequenceInfo = new SequenceInfo();
		    	sequenceInfo->setFilename(fasta_file);

		    	SequenceModifiers* modifiers = new SequenceModifiers();
			    modifiers->setClearN(false);
			    modifiers->setReverse(false);
			    modifiers->setComplement(false);
			    modifiers->setTrimStart(0);
			    modifiers->setTrimEnd(0);

			    seq[id] = new Sequence(sequenceInfo, modifiers);
			}
		} else if (strcmp(cmd[0], "check") == 0) {
			if (strcmp(cmd[1], "score") == 0) {
				score_t check_score;
				sscanf(line, "%*s %*s %d%d%d", &check_score.i, &check_score.j, &check_score.score);

				if (check_score.score == best_score.score) {
					fprintf(stderr, "CHECK OK   : Score: %d\n", best_score.score);
				} else {
					fprintf(stderr, "CHECK ERROR: Score: %d != %d\n", check_score.score, best_score.score);
				}
				if (check_score.i == best_score.i && check_score.j == best_score.j) {
					fprintf(stderr, "CHECK OK   : Pos (%d, %d)\n", best_score.i, best_score.j);
				} else {
					fprintf(stderr, "CHECK ERROR: Pos (%d, %d) != (%d, %d)\n", check_score.i, check_score.j, best_score.i, best_score.j);
				}
			} else if (strcmp(cmd[1], "column") == 0) {
				int i;
				char file[1024];
				sscanf(line, "%*s %*s %d %s", &i, file);
				compare_files(file, lastColumnName.c_str());
			} else if (strcmp(cmd[1], "row") == 0) {
				int i;
				char file[1024];
				sscanf(line, "%*s %*s %d %s", &i, file);
				compare_files(file, lastRowName.c_str());
			}
		} else if (strcmp(cmd[0], "test") == 0) {
			lastRow = fopen(lastRowName.c_str(), "wb");
			if (lastRow == NULL) {
				fprintf(stderr, "Could not create file %s not found\n", lastRowName.c_str());
				exit(1);
			}

			lastColumn = fopen(lastColumnName.c_str(), "wb");
			if (lastColumn == NULL) {
				fprintf(stderr, "Could not create file %s not found\n", lastColumnName.c_str());
				exit(1);
			}

			Sequence* seq_vertical = new Sequence(seq[0]);
			Sequence* seq_horizontal = new Sequence(seq[1]);

			const score_params_t* score_params = aligner->getScoreParameters();
			AlignerManager* sw = new AlignerManager(aligner);

			sw->setFirstRowSource(false);
			//sw->setFirstRowSource(file);
			sw->setBlockPruning(false);
			//sw->setProcessLastCellFunction(processLastCellFunction);
//			sw->setProcessScoreFunction(processBestScoreFunction);
//			sw->setProcessLastRowFunction(processLastRowFunction);
//			sw->setProcessLastColumnFunction(processLastColumnFunction);
			sw->setFirstColumnSource(false);
			sw->setRecurrenceType(SMITH_WATERMAN);
			sw->setSequences(seq_vertical, seq_horizontal);
			//sw->setProcessSpecialRowFunction(processSpecialRowFunction);
			aligner->initialize();

			Partition partition(i0, j0, i1, j1);
			best_score.score = -1;
			sw->alignPartition(partition, START_TYPE_MATCH);
			/*aligner->prepareIterations();
			while (aligner->hasMoreIterations()) {
				fprintf(stderr, "NextIteration\n");
				aligner->processNextIteration();
			}*/
			fprintf(stderr, "Done\n");
			fclose(lastRow);
			fclose(lastColumn);

		}
	}


	return true;
}




