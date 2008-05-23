/***************************************************************************
 *   Copyright (C) 2007 by George Kiagiadakis                              *
 *   gkiagia@users.sourceforge.net                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.          *
 ***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/un.h>

#define VERSION "0.1"
#define PROGRAM_MESSAGE "qtwine-terminal-helper: "

#define die(msg) do { perror(PROGRAM_MESSAGE msg); sleep(3); exit(EXIT_FAILURE); } while(0)

#ifdef DEBUG
#define debug(msg) do { puts(PROGRAM_MESSAGE msg); } while(0)
#else
#define debug(msg) do { } while(0);
#endif

void usage()
{
	puts("QtWine terminal helper version "VERSION);
	puts("Copyright (C) 2007 by George Kiagiadakis <gkiagia@users.sourceforge.net>\n");

	puts("Usage: qtwine-terminal-helper [socket address]\n");

	puts("This is a small C program that does two things: it connects to a unix");
	puts("domain socket and prints to stdout everything that it reads from the socket");
	puts("and simultaneously it watches stdin for input and when it gets some");
	puts("input from it, it sends the data across the socket.\n");

	puts("The socket must be binded to the filesystem using bind(2) by the server");
	puts("and this program's first argument (argv[1]) must be the filename of");
	puts("the socket on the filesystem.");
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		usage();
		return 1;
	} else if ( strcmp(argv[1], "--help") == 0 ) {
		usage();
		return 0;
	}

	/* setup the socket */
	int sockfd;
	struct sockaddr_un serverAddress;

	debug("trying to connect...");

	if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1)
		die("socket() failed");

	memset(&serverAddress, 0, sizeof(struct sockaddr_un));
	serverAddress.sun_family = AF_UNIX;
	strcpy(serverAddress.sun_path, argv[1]); //the socket address must be in argv[1]

	int len = strlen(serverAddress.sun_path) + sizeof(serverAddress.sun_family);
	if (connect(sockfd, (struct sockaddr *)&serverAddress, len) == -1)
		die("connect() failed");

	debug("connected");

	/* prepare to select() */
	fd_set rfds;
	int retval;
	ssize_t bytesRead;
	char buffer[BUFSIZ];

	debug("starting polling...");

	/* Start polling for data I/O 
	The process can now only be terminated by a signal, probably SIGTERM */
	while(1) {
		FD_ZERO(&rfds);
		FD_SET(sockfd, &rfds); //watch the socket for input
		FD_SET(0, &rfds); //watch stdin for input

		retval = select(sockfd+1, &rfds, NULL, NULL, NULL);

		if (retval == -1)
			die("select() failed");
		else if (retval > 0) {

			if ( FD_ISSET(sockfd, &rfds) ) {
				debug("reading from socket");
				bytesRead = recv(sockfd, buffer, BUFSIZ, 0);

				if ( bytesRead > 0 && write(1, buffer, bytesRead) == -1 )
					die("write()");
				else if ( bytesRead == -1 )
					die("recv()");
			}
			if ( FD_ISSET(0, &rfds) ) {
				debug("reading from stdin");
				bytesRead = read(0, buffer, BUFSIZ);

				if ( bytesRead > 0 && send(sockfd, buffer, bytesRead, 0) == -1 )
					die("send()");
				else if ( bytesRead == -1 )
					die("read()");
			}
		}
	}
}

