/*******************************************************************************
 *
 * Copyright (c) 2010, 2013   Edans Sandes
 *
 * This file is part of CUDAlign.
 * 
 * CUDAlign is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * CUDAlign is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with CUDAlign.  If not, see <http://www.gnu.org/licenses/>.
 *
 ******************************************************************************/

#include <stdio.h>

#include "../aligner/AbstractAligner.hpp"
#include "../aligner/example/ExampleAligner.hpp"
#include "../common/biology/biology.hpp"

int main( int argc, char** argv ) {
	AbstractAligner* aligner = new ExampleAligner();

	aligner->initialize();
	aligner_capabilities_t capabilities = aligner->getCapabilities();

	char* fasta_file[] = {"NB", "NC"};
	Sequence* seq[2];

	for (int i=0; i<2; i++) {

		SequenceInfo* sequenceInfo = new SequenceInfo();
		sequenceInfo->setFilename(fasta_file[i]);

		SequenceModifiers* modifiers = new SequenceModifiers();
		modifiers->setClearN(false);
		modifiers->setReverse(false);
		modifiers->setComplement(false);
		modifiers->setTrimStart(0);
		modifiers->setTrimEnd(0);

		SequenceInfo* info = new SequenceInfo();
		SequenceModifiers* modifiers;
		seq[i] = new Sequence(info, modifiers);
	}

	aligner->setSequences(seq[0], seq[0]);
	aligner->setPartition(0,0,500,500,TYPE_MATCH);
	int i = 0;
	while (aligner->hasMoreIterations()) {
		aligner->processNextIteration();
		printf("Count: %d\n", i++);
	}

	return 0;
}
