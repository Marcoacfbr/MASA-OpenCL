/*******************************************************************************
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
 ******************************************************************************/

/*
 * MasaNetCLI.cpp
 *
 *  Created on: Oct 16, 2013
 *      Author: edans
 */

#include "MasaNetCLI.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>

/*
 * Default prompt.
 */
#define DEFAULT_PROMPT		"masa-cli# "


#define WELLCOME_MESSAGE	"\n\
This is the \033[1mMASA\033[0m Command Line Interface (masa-cli).\n\
Type \"help\" or ? for further assistance\n\
\n"

/*
 * Allow parsing of the ~/.inputrc file .
 */
#define READLINE_NAME 		"masa"



command_t MasaNetCLI::commands[] = {
  { "connect", 	MasaNetCLI::cmd_connect, 	"Starts a new connection with the given hostname[:port]" },
  { "show status", 	MasaNetCLI::cmd_status, 	"Shows the current connection status" },
  { "show discovered", 	MasaNetCLI::cmd_show_discovered, 	"Shows the discovered peers" },
  { "show connected", 	MasaNetCLI::cmd_show_connected, 	"Shows the connected peers" },
  { "show ring", 	MasaNetCLI::cmd_show_ring, 	"Shows the data peers in the ring" },
  { "test ring", 	MasaNetCLI::cmd_test_ring, 	"Test all the peers in the ring" },
  { "create ring", 	MasaNetCLI::cmd_create_ring, 	"Creates the buffers ring" },

  { "align local", 	(command_f)NULL, 	"" },

  { "help", 	MasaNetCLI::cmd_help, 		"Lists the commands help" },
  { "quit", 	MasaNetCLI::cmd_quit, 		"Quits the CLI" },
  { (char *)NULL, (command_f)NULL, (char *)NULL },
};



MasaNet* MasaNetCLI::masaNet = NULL;

void MasaNetCLI::initialize() {
	if (masaNet == NULL) {
		masaNet = new MasaNet(TYPE_CLI, "Command Line Interface");
	}
}

void MasaNetCLI::openConsole() {

  	/* Allow conditional parsing of the ~/.inputrc file. */
  	rl_readline_name = READLINE_NAME;

  	/* Tell the completer that we want a crack first. */
  	rl_attempted_completion_function = console_completion;

    char* input;
    char shell_prompt[100];

    printf(WELLCOME_MESSAGE);

    // Create prompt string from user name and current working directory.
    snprintf(shell_prompt, sizeof(shell_prompt), DEFAULT_PROMPT);

    // Configure readline to auto-complete paths when the tab/? keys are hit.
    //rl_bind_key('\t', rl_complete);
    //rl_bind_key('?', rl_complete);

    while (1) {


        // Display prompt and read input (n.b. input must be freed after use)...
        input = readline(shell_prompt);
        // Check for EOF.
        if (!input) {
        	//printf("\n");
            break;
        }

      	char* s = strip_whitespaces(input);
      	if (*s) {
      		// Add input to history.
          	add_history(s);
          	if (execute_line(s) == CMD_ABORT) {
          		return;
         	}
        }
        // Do stuff...

        // Free input.
        free(input);
    }


	sleep(500);
}





/* Execute a command line. */
int MasaNetCLI::execute_line (char *line) {
	register int i;
	command_t *command;
	char *word;

	/* Isolate the command word. */
	i = 0;
	while (line[i] && whitespace (line[i])) {
		i++;
	}
	word = line + i;

	//if (line[i])
	//line[i++] = '\0';

	command = find_command(line);

	if (!command) {
		fprintf(stderr, "%s: No such command for FileMan.\n", word);
		return CMD_ERROR;
	}

	i += strlen(command->name);
	while (line[i] && whitespace (line[i])) {
		i++;
	}
	word = line + i;

	/* Call the function. */
	return ((*(command->func)) (word));
}


/* Look up NAME as the name of a command, and return a pointer to that
   command.  Return a NULL pointer if NAME isn't a command name. */
command_t* MasaNetCLI::find_command (char *name) {
	register int i;

	for (i = 0; commands[i].name; i++) {
		int len = strlen(commands[i].name);
		if (strncmp(name, commands[i].name, len) == 0) {
			if (name[len] == '\0' || whitespace(name[len])) {
				return (&commands[i]);
			}
		}
	}
	return ((command_t *) NULL);
}

/* Strip whitespace from the start and end of STRING.  Return a pointer
   into STRING. */
char *MasaNetCLI::strip_whitespaces (char * string) {

  register char *s, *t;

  for (s = string; whitespace (*s); s++)
    ;

  if (*s == 0)
    return (s);

  t = s + strlen (s) - 1;
  while (t > s && whitespace (*t))
    t--;
  *++t = '\0';

  return s;
}


/* Attempt to complete on the contents of TEXT.  START and END
   bound the region of rl_line_buffer that contains the word to
   complete.  TEXT is the word to complete.  We can use the entire
   contents of rl_line_buffer in case we want to do some simple
   parsing.  Returnthe array of matches, or NULL if there aren't any. */
char ** MasaNetCLI::console_completion (const char *text, int start, int end) {
  char **matches;

  //printf("%s\n", rl_line_buffer);

  matches = (char **)NULL;
  /* If this word is at the start of the line, then it is a command
     to complete.  Otherwise it is the name of a file in the current
     directory. */
  //if (start == 0)
    matches = rl_completion_matches (text, command_generator);
  //rl_attempted_completion_over=true;

  //printf("\n%s %s %s %d %d\n\n", rl_line_buffer, matches[0], text, start, end);

  return (matches);
}

/* Generator function for command completion.  STATE lets us
   know whether to start from scratch; without any state
   (i.e. STATE == 0), then we start at the top of the list. */
char * MasaNetCLI::command_generator (const char *text, int state) {

  static int list_index, len;
  char *name;

  /* If this is a new word to complete, initialize now.  This
     includes saving the length of TEXT for efficiency, and
     initializing the index variable to 0. */
  if (!state)
    {
      list_index = 0;
      len = strlen (rl_line_buffer);
    }

  /* Return the next name which partially matches from the
     command list. */
  while (name = commands[list_index].name)
    {
      list_index++;

      if (len == 0 || strncmp (name, rl_line_buffer, len) == 0) {
    	  int p = len-1;
    	  while (name[p] != ' ' && p != 0) {
    		p--;
    	  }
    	  if (name[p] == ' ') p++;
    	  char* ptr = &name[p];
    	  char* r = (char*)malloc (strlen (ptr) + 1);

    	  strcpy (r, ptr);
    	  //printf(".%s\n", r);
    	  return r;
      }
    }

  /* If no names matched, then return NULL. */
  return ((char *)NULL);
}


void MasaNetCLI::printTopology(int type) {
	vector<Peer*> peers = masaNet->getConnectedPeers().getAllPeers();
	vector< vector<Peer*> > topology;

	for (vector<Peer*>::const_iterator it = peers.begin() ; it != peers.end(); ++it) {
		Peer* src = (*it);
		vector<Peer*> v;
		v.push_back(src);

		vector<Peer*> remote_peers = masaNet->getRemotePeers(src->getRemoteId().c_str(), type);
		for (vector<Peer*>::const_iterator it2 = remote_peers.begin() ; it2 != remote_peers.end(); ++it2) {
			Peer* dst = (*it2);
			v.push_back(dst);
		}
		topology.push_back(v);
	}

	for (vector< vector<Peer*> >::const_iterator it = topology.begin() ; it != topology.end(); ) {
		vector<Peer*> v = *it;
		Peer* src = v.front();
		if (it != topology.begin()) {
			printf("|\n");
		}
		it++;
		printf("+-+- %s:\n", src->toString().c_str());
		if (v.size() > 1) {
			char space = ' ';
			if (it != topology.end()) {
				space = '|';
			}
			printf("%c |\n", space);
			for (vector<Peer*>::const_iterator it2 = v.begin()+1 ; it2 != v.end(); ++it2) {
				Peer* dst = *it2;

				printf("%c +----- %s\n", space, dst->toString().c_str());
			}
		}
	}

}




int MasaNetCLI::cmd_connect(char* arg) {
	if (strcmp(arg, "all") == 0) {
		vector<Peer*> peers = masaNet->getConnectedPeers().getProcessingPeers();
		set<string> newPeers;

		for (vector<Peer*>::const_iterator it = peers.begin() ; it != peers.end(); ++it) {
			vector<Peer*> remote_peers = masaNet->getRemotePeers((*it)->getRemoteId().c_str(), CMD_DISCOVERED_PEERS);
			for (vector<Peer*>::const_iterator it2 = remote_peers.begin() ; it2 != remote_peers.end(); ++it2) {
				if (masaNet->getConnectedPeers().get((*it2)->getRemoteId()) == NULL) {
					newPeers.insert((*it2)->getRemoteAddress());
				}
			}
		}

		for (set<string>::const_iterator it = newPeers.begin(); it != newPeers.end(); ++it) {
			printf("Should connect to %s\n", (*it).c_str());
			masaNet->connectToPeer(*it, CONNECTION_TYPE_CTRL);
		}
		return CMD_OK;
	} else {
		Peer* ret = masaNet->connectToPeer(arg, CONNECTION_TYPE_CTRL);
		ret->waitHandshake();
		if (ret != NULL) {
			//printf("Connection Established.\n");
			return CMD_OK;
		} else {
			//printf("Connection Error.\n");
			return CMD_ERROR;
		}
	}


}

/* The user wishes to quit using this program.  Just set DONE
   non-zero. */
int MasaNetCLI::cmd_quit (char *arg) {
  exit(0);
  return CMD_OK;
}


int MasaNetCLI::cmd_status(char* arg) {
	char host[256];
	char pstr[16];
	int port = 0;
	sscanf(arg, "%[^:]:%d", host, &port);
	sprintf(pstr, "%d", port);
	MasaNetStatus* status = masaNet->getPeerStatus();

	printf("\n%s\n", status->toString().c_str());
	//masaNet->waitConnected();
	return CMD_OK;
}


int MasaNetCLI::cmd_show_connected(char* arg) {

	printTopology(CMD_CONNECTED_PEERS);

	return CMD_OK;
}

int MasaNetCLI::cmd_show_connected_remote(char* arg) {
	return CMD_OK;
}

int MasaNetCLI::cmd_show_discovered(char* arg) {
	printTopology(CMD_DISCOVERED_PEERS);

	return CMD_OK;
}

int MasaNetCLI::cmd_show_ring(char* arg) {
	printTopology(CMD_DATA_PEERS);

	return CMD_OK;
}

int MasaNetCLI::cmd_create_ring(char* arg) {
	masaNet->createRing();

	return CMD_OK;
}


int MasaNetCLI::cmd_test_ring(char* arg) {
	const set<string>& ids = masaNet->testRing(arg);
	printf("Ring size: %d\n", ids.size());
	int count = 0;
	for (std::set<string>::iterator it=ids.begin(); it!=ids.end(); ++it) {
		printf("%3d: %s\n", count++, (*it).c_str());
	}

	return CMD_OK;
}

/* Print out help for ARG, or for all of the commands if ARG is
   not present. */
int MasaNetCLI::cmd_help (char *arg) {
  register int i;
  int printed = 0;

  for (int  i = 0; commands[i].name; i++)
    {
      if (!*arg || (strcmp (arg, commands[i].name) == 0))
        {
          printf ("%s\t\t%s.\n", commands[i].name, commands[i].doc);
          printed++;
        }
    }

  if (!printed)
    {
      printf ("No commands match `%s'.  Possibilties are:\n", arg);

      for (i = 0; commands[i].name; i++)
        {
          /* Print in six columns. */
          if (printed == 6)
            {
              printed = 0;
              printf ("\n");
            }

          printf ("%s\t", commands[i].name);
          printed++;
        }

      if (printed)
        printf ("\n");
    }
  return CMD_OK;
}



int main(int argc, char** argv) {
	if (argc > 2) {
		fprintf(stderr, "usage: %s [host[:port]]\n", argv[0]);
		exit(1);
	}
	MasaNetCLI::initialize();
	if (argc == 2) {
		MasaNetCLI::cmd_connect(argv[1]);
	}
	MasaNetCLI::openConsole();
}

