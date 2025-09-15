/****************************************************************/
/*                                                              */
/*          Advanced Navigation Packet Protocol Library         */
/*             Serial Port Data Logger, Version 4.1             */
/*              Copyright 2017, Advanced Navigation             */
/*                                                              */
/****************************************************************/
/*
 * Copyright (C) 2017 Advanced Navigation
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <wiringPi.h> // for GPIO

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

#include "rs232/rs232.h"

static const uint8_t request_all_configuration[] = { 0xE2, 0x01, 0x10, 0x9A, 0x73, 0xB6, 0xB4, 0xB5, 0xB8, 0xB9, 0xBA, 0xBC, 0xBD, 0xC0, 0xC2, 0xC3, 0xC4, 0x03, 0xC6, 0x45, 0xC7 };

int main(int argc, char *argv[])
{
	uint8_t buffer[2048];
	int bytes_received;

	FILE *log_file;
	char filename[32];
	time_t rawtime;
	struct tm * timeinfo;
	int write_counter = 0;
	int if_started=0;
	// GPIO settings
	wiringPiSetup();	// Setup the library
	//pullUpDnControl(4,PUD_DOWN);
	//pinMode(4, INPUT); // toggle switch for ANPPrecoder
	pinMode(3, OUTPUT); // LED  for ANPP  recoder
	fprintf(stdout,"Init Started");
	fflush(stdout);

	if (argc != 3)
	{
		printf("Usage - program com_port baud_rate\nExample - packet_example.exe COM1 115200\n");
		exit(EXIT_FAILURE);
	}

	while (1)
	{
		if(digitalRead(3) == 1)
					{
						if(!if_started) {
						rawtime = time(NULL);
						timeinfo = localtime (&rawtime);
						sprintf(filename, "/root/LOG/SpatialLog_%02d-%02d-%02d_%02d-%02d-%02d.anpp", timeinfo->tm_year-100, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);

						log_file = fopen(filename, "wb");
						//printf("LOG started");
						//digitalWrite(3,1);
						//fflush(stdout);
						if_started=1;

						/* Open the com port */
						if (OpenComport(argv[1], atoi(argv[2])))
						{
							exit(EXIT_FAILURE);
						}
						/* Request all the configuration and the device information from the unit */
						SendBuf(request_all_configuration, sizeof(request_all_configuration));
						}

						if ((bytes_received = PollComport(buffer, sizeof(buffer))) > 0)
						{
							fwrite(buffer, sizeof(uint8_t), bytes_received, log_file);
						}
						if(write_counter++ >= 100)
						{
							fflush(log_file);
							write_counter = 0;
						}
						#ifdef _WIN32
    						Sleep(10);
						#else
   						usleep(1000);
						#endif
					}
					else if(digitalRead(3) == 0 && if_started == 1)
					{
						fflush(log_file);
						write_counter = 0;
						fclose(log_file);
						CloseComport();
		        		printf("Record Saved");
						//digitalWrite(3,0);
						fflush(stdout);
						if_started=0;
					}
					else
					{
						//cout<< "Waiting...";
					}
		usleep(1000);

	}
}
