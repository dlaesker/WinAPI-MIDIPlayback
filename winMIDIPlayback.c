#include <windows.h>
// #include <mmeapi.h>
#include <stdio.h>
#include <conio.h>

void CALLBACK MidiInProc(
	HMIDIIN   hMidiIn,		// Handle to the MIDI input device.
   	UINT      wMsg,			// Type of MIDI input message.
   	DWORD_PTR dwInstance,	// Can be used to pass in user-defined data.
   	
   	// Message parameters; dependent on the type of wMsg.
   	DWORD_PTR mParam1,		
   	DWORD_PTR mParam2		
);

HMIDIIN 	inHandle;	// Handle to input MIDI device (Piano/Keyboard, etc)
HMIDIOUT 	outHandle; 	// Handle to output MIDI device (Playback)


int main(){
	/* 1. Query on MIDI input devices in the system: */
	MIDIINCAPS 		mic;
	unsigned long 	inNumDevs, i, inDeviceID = -1;

	/* Get the number of MIDI input devices in the system */
	inNumDevs = midiInGetNumDevs();

	/* Iterate through all avilable input devices and display their names */
	for (i = 0; i < inNumDevs; i++){
	    /* Get info about the next device */
	    if (!midiInGetDevCaps(i, &mic, sizeof(MIDIINCAPS))){
	        /* Display its Device ID and name */
	        printf("Device ID #%u: %s\r\n", i, mic.szPname);

	        inDeviceID = i;
	        break; // Keep ID of the first device found
	    }
	}

	/* 2. Now query on MIDI output devices in the system: */
	MIDIOUTCAPS     moc;
	unsigned long  	outNumDevs, outDeviceID = -1;

	/* Get the number of MIDI output devices in the system */
	outNumDevs = midiOutGetNumDevs();

	/* Iterate through all avilable output devices and display their names */
	for (i = 0; i < outNumDevs; i++){
	    /* Get info about the next device */
	    if (!midiOutGetDevCaps(i, &moc, sizeof(MIDIOUTCAPS))){
	        /* Display its Device ID and name */
	        printf("Device ID #%u: %s\r\n", i, moc.szPname);

	        outDeviceID = i;
	        break; // Keep ID of the first device found
	    }
	}

	if(inDeviceID < 0 || outDeviceID < 0) exit(EXIT_FAILURE);
	// printf("here\n");

	/* 3. Connect to the input and output devices */
	MMRESULT res;
	// https://docs.microsoft.com/en-us/windows/win32/api/mmeapi/nf-mmeapi-midiinopen
	res = midiInOpen(
		&inHandle, 					// HMIDIIN handle
		inDeviceID, 				
		(DWORD_PTR)(MidiInProc), 	// Callback function
		0, 							// No data is being passed
		CALLBACK_FUNCTION			// Flag specifying callback as a function
	);

	if(res) exit(EXIT_FAILURE); 	// ERROR

	// https://docs.microsoft.com/en-us/windows/win32/api/mmeapi/nf-mmeapi-midioutopen
	res = midiOutOpen(				
		&outHandle, 				// HMIDIOUT handle
		outDeviceID, 				// 0 = Windows GS Wavetable Synth 
		0, 							// No callback function...
		0, 							// ...nor user-defined data
		CALLBACK_NULL				// There is no callback mechanism.
	);

	if(res) exit(EXIT_FAILURE); 	// ERROR

	/* 4. Start receiving data from the MIDI input device */
	
	// https://docs.microsoft.com/en-us/windows/win32/api/mmeapi/nf-mmeapi-midiinstart
	res = midiInStart(inHandle); 	// Start receiving MIDI input
	
	if(res) exit(EXIT_FAILURE); 	// ERROR
    
	/* 5. Read data until the user quits */
    while (1) {
        if (!_kbhit()) {	// If no keyboard input, put thread to sleep
            Sleep(100);
            continue;
        }
        // Else read input from the keyboard
        int c = _getch();
        if (c == VK_ESCAPE) break;
        if (c == 'q') break;
    }

    /* 6. Close the connection to the MIDI input/output device */
    // https://docs.microsoft.com/en-us/windows/win32/api/mmeapi/nf-mmeapi-midiinstop
    res = midiInStop(inHandle); 	// Stop receiving MIDI input
    if(res) exit(EXIT_FAILURE); 	// ERROR

    // https://docs.microsoft.com/en-us/windows/win32/api/mmeapi/nf-mmeapi-midiinclose
	res = midiInClose(inHandle);	// Close the connection to the MIDI input device
	if(res) exit(EXIT_FAILURE); 	// ERROR

	// https://docs.microsoft.com/en-us/windows/win32/api/mmeapi/nf-mmeapi-midioutclose
	res = midiOutClose(outHandle);
	if(res) exit(EXIT_FAILURE); 	// ERROR

	// END
	return 0;
}

// MIDI Input Callback Function
void CALLBACK MidiInProc(
   HMIDIIN hMidiIn, UINT wMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2
){
    switch (wMsg) {
    // For a list of messages: 
    // https://docs.microsoft.com/en-us/windows/win32/multimedia/midi-messages
    case MIM_DATA: {	
    	// dwParam1 = MIDI message
    	// dwParam2 = Timestamp 

    	// Uncomment to see the midi message and timestamp
    	// printf("dwParam1=%x, dwParam2=%x\n", dwParam1, dwParam2); 
		
		// Send the MIDI message over to the output device (i.e., GS Wavetable Synth)
    	// https://docs.microsoft.com/en-us/windows/win32/api/mmeapi/nf-mmeapi-midioutshortmsg
    	midiOutShortMsg(outHandle, dwParam1);
    	break;

    }
    
    case MIM_OPEN: 	printf("Connection established.\n"); break;
    case MIM_CLOSE: printf("Connection closed.\n"); break;
    default:
    	break;
    }
    return;
}