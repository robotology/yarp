// -*- mode:C++; tab-width:4; c-basic-offset:4; indent-tabs-mode:nil -*-

/*
 * Copyright (C) 2006 Julio Gomes, Alexandre Bernardino
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */

/***********************************************************************

  YARP2 MicrophoneDeviceDriver

  Windows Native Implementation for sound acquisition

  Adapted from YARP1 - YarpSoundDeviceDriver, YarpSoundCardUtils

  Author of YARP1 (original) Code: Carlos Beltran, Lira-Lab, DIST, UNIGE

  Authors of YARP2 (adapted) Code: Julio Gomes, Alexandre Bernardino, VisLab, ISR-IST

  Contact: jgomes(a)isr.ist.utl.pt, alex(a)isr.ist.utl.pt

************************************************************************/

#include <MicrophoneDeviceDriver.h>
#include <yarp/os/all.h>
#include <ace/Sched_Params.h>

#undef main
#include <windows.h>
#include <mmsystem.h>

using namespace yarp::os;
using namespace yarp::dev;


/*
  SoundOpenParameters contains initialization parameters.
  It is inherited from YARP1 and is now obsolete.
  Future: use Searchable class instead.
*/
struct SoundOpenParameters
{
	SoundOpenParameters()
	{
		m_callbackthread_identifier = 0;
		m_Channels      = 0;
		m_SamplesPerSec = 0;
		m_BitsPerSample = 0;
		m_BufferLength  = 0;
	}

	int m_callbackthread_identifier;  //This is the thread identifier for the callback
	int m_Channels;
	int m_SamplesPerSec;
	int m_BitsPerSample;
	int m_BufferLength;
};

/*
  SoundResources: 
  Encapsulates windows native stuff for sound acquisition.
*/

class SoundResources : public yarp::os::Semaphore, public yarp::os::Thread
{
public:
	//----------------------------------------------------------------------
	//  Constructor/Destructor
	//----------------------------------------------------------------------
	SoundResources (void) : _bmutex(1),
							_new_frame(0),
							_canpost(true),
							numSamples(2048),
							microDistance(0.14)
	{
		//----------------------------------------------------------------------
		// Initialize variables
		// Default: 16-bit, 44KHz, stereo
		//----------------------------------------------------------------------
		m_InRecord     = false;
		dwBufferLength = 8192;
		freqSample     = 44100;
		nBitsSample    = 16;
		channels       = 2;
	}

	~SoundResources () { _uninitialize (); }

	//----------------------------------------------------------------------
	// Variables  
	//----------------------------------------------------------------------
	yarp::os::Semaphore _bmutex;
	yarp::os::Semaphore _new_frame;
	bool          _canpost;

	//Declare usefull variables
    HWAVEIN       m_WaveInHandle;    // Handle to the WAVE In Device
    HMIXER        m_MixerHandle;     // Handle to Mixer for WAVE In Device
    WAVEHDR       m_WaveHeader[3];   // We use two WAVEHDR's for recording (ie, double-buffering) in this example
    bool          m_InRecord;        // Variable used to indicate whether we are in record
    unsigned char m_DoneAll;         // Variable used by recording thread to indicate whether we are in record

    MMRESULT      m_err;
    WAVEFORMATEX  m_waveFormat;
    MIXERLINE     m_mixerLine;
    HANDLE        m_waveInThread;
    unsigned long m_n;
    unsigned long m_numSrc;
	
	// Control structures 
	MIXERCONTROL      m_mixerControlArray;
	MIXERLINECONTROLS m_mixerLineControls;

	//Local lockable buffer
	unsigned char *_rawBuffer;

	//----------------------------------------------------------------------
	//  Parameters
	//----------------------------------------------------------------------
	DWORD        dwBufferLength;
	const DWORD  numSamples;       //dwBufferLength/4
	DWORD        freqSample;
	const double microDistance;    // babybot
	WORD         nBitsSample;
	WORD         channels;

	//----------------------------------------------------------------------
	// Public Method definitions 
	//----------------------------------------------------------------------
	int _initialize (const SoundOpenParameters& params);
	int _uninitialize (void);
	int _select_line(unsigned int type);
	int _select_control(unsigned int control);
	int acquireBuffer (void *buffer);
	int releaseBuffer (void);
	int waitOnNewFrame(void);
	void run(void);
	

protected:
	
	//----------------------------------------------------------------------
	//  Protected method definitions
	//----------------------------------------------------------------------
	int _init (const SoundOpenParameters& params);
	void _prepareBuffers (void);
	void _print_dst_lines();
	void _print_src_lines();
};

//--------------------------------------------------------------------------------------
//       Class:  SoundResources
//      Method:  _print_src_lines
// Description:  This function prints in the screen the source lines present
//--------------------------------------------------------------------------------------
void
SoundResources::_print_src_lines()
{
	for (int i = 0; i < m_numSrc; i++)
        {
            m_mixerLine.cbStruct = sizeof(MIXERLINE);
            m_mixerLine.dwSource = i;

            if (!(m_err = mixerGetLineInfo((HMIXEROBJ)m_MixerHandle, &m_mixerLine, MIXER_GETLINEINFOF_SOURCE)))
                {
                    if (m_mixerLine.dwComponentType != MIXERLINE_COMPONENTTYPE_SRC_SYNTHESIZER)
                        printf("\t\t#%lu: %s\n", i, m_mixerLine.szName);
                }
        }
}

//--------------------------------------------------------------------------------------
//       Class:  SoundResources
//      Method:  (public)_select_control
// Description:  This methos allows to select the type of control in the selected line
//--------------------------------------------------------------------------------------
int
SoundResources::_select_control(unsigned int control_type)
{
	//----------------------------------------------------------------------
	//  Fill the mixerLineControls structure
	//----------------------------------------------------------------------
	m_mixerLineControls.cbStruct      = sizeof(MIXERLINECONTROLS);
	m_mixerLineControls.dwLineID      = m_mixerLine.dwLineID;
	m_mixerLineControls.cControls     = 1;
	m_mixerLineControls.dwControlType = control_type;
	m_mixerLineControls.pamxctrl      = &m_mixerControlArray;
	m_mixerLineControls.cbmxctrl      = sizeof(MIXERCONTROL);

	m_err = mixerGetLineControls((HMIXEROBJ)m_MixerHandle, 
								 &m_mixerLineControls, 
								 MIXER_GETLINECONTROLSF_ONEBYTYPE);
	
	if (m_err != MMSYSERR_NOERROR) 
		printf("yarpsounddriver: %s has no %s control!\n", 
			   m_mixerLine.szName, 
			   m_mixerLineControls.pamxctrl->szName);
	
	return 1;
}

int SoundResources::_select_line(unsigned int type)
{

    for(int i = 0; i < m_numSrc; i++) {
		m_mixerLine.cbStruct = sizeof(MIXERLINE);	
		m_mixerLine.dwSource = i;

		m_err = mixerGetLineInfo((HMIXEROBJ)m_MixerHandle,
								 &m_mixerLine,
								 MIXER_GETLINEINFOF_SOURCE);
		
		if (m_err != MMSYSERR_NOERROR) continue;
		
		if (m_mixerLine.dwComponentType == type)
            {
                printf("yarpsounddriver: source line found\n");
                return 1;
            }	
	}
	printf("yarpsounddriver: -warning- source line not found\n");
	return -1;

}

void SoundResources::_prepareBuffers(void) {

	//----------------------------------------------------------------------
	//  Preparing all memory buffer allocation
	//----------------------------------------------------------------------
	m_WaveHeader[2].dwBufferLength = 
        m_WaveHeader[1].dwBufferLength = 
        m_WaveHeader[0].dwBufferLength = dwBufferLength;

	m_WaveHeader[0].lpData = (char *)VirtualAlloc(0, 
												  m_WaveHeader[0].dwBufferLength, 
												  MEM_COMMIT, 
												  PAGE_READWRITE);
	m_WaveHeader[1].lpData = (char *)VirtualAlloc(0, 
												  m_WaveHeader[1].dwBufferLength, 
												  MEM_COMMIT, 
												  PAGE_READWRITE);
	m_WaveHeader[2].lpData = (char *)VirtualAlloc(0, 
												  m_WaveHeader[2].dwBufferLength, 
												  MEM_COMMIT, 
												  PAGE_READWRITE);
	
	//----------------------------------------------------------------------
	// Initialize dwFlags and dwLoops to 0. This seems to be necesary according to the
	// Microsoft Windows documentation 
	//----------------------------------------------------------------------
	m_WaveHeader[0].dwFlags = m_WaveHeader[1].dwFlags = m_WaveHeader[2].dwFlags = 0L;
	m_WaveHeader[0].dwLoops = m_WaveHeader[1].dwLoops = m_WaveHeader[2].dwFlags = 0L; 

	//----------------------------------------------------------------------
	// Initialize the headers
	//----------------------------------------------------------------------
	if ((m_err = waveInPrepareHeader(m_WaveInHandle, &m_WaveHeader[0], sizeof(WAVEHDR)))) 
		printf("yarpsounddriver: Error preparing WAVEHDR -- %08X\n", m_err);
	if ((m_err = waveInPrepareHeader(m_WaveInHandle, &m_WaveHeader[1], sizeof(WAVEHDR)))) 
		printf("yarpsounddriver: Error preparing WAVEHDR -- %08X\n", m_err);
	if ((m_err = waveInPrepareHeader(m_WaveInHandle, &m_WaveHeader[2], sizeof(WAVEHDR)))) 
		printf("yarpsounddriver: Error preparing WAVEHDR -- %08X\n", m_err);

	//----------------------------------------------------------------------
	//  It is necessary to queue the two buffers.  
	//----------------------------------------------------------------------
	if ((m_err = waveInAddBuffer(m_WaveInHandle, &m_WaveHeader[0], sizeof(WAVEHDR))))
		printf("yarpsounddriver: Error queueing WAVEHDR 1! -- %08X\n", m_err);
	if ((m_err = waveInAddBuffer(m_WaveInHandle, &m_WaveHeader[1], sizeof(WAVEHDR))))
		printf("yarpsounddriver: Error queueing WAVEHDR 2! -- %08X\n", m_err);
	if ((m_err = waveInAddBuffer(m_WaveInHandle, &m_WaveHeader[2], sizeof(WAVEHDR))))
		printf("yarpsounddriver: Error queueing WAVEHDR 2! -- %08X\n", m_err);

	return;
}


void SoundResources::_print_dst_lines() {

	MIXERCAPS mixerCaps;

	mixerGetDevCaps(0,&mixerCaps, sizeof(mixerCaps));

	for (int i = 0; i < mixerCaps.cDestinations; i++)
        {
            m_mixerLine.cbStruct      = sizeof(MIXERLINE);
            m_mixerLine.dwSource      = 0;
            m_mixerLine.dwDestination = i;

            if (!(m_err = mixerGetLineInfo((HMIXEROBJ)m_MixerHandle, &m_mixerLine, MIXER_GETLINEINFOF_DESTINATION))) {
				printf("\t#%lu: %s\n", i, m_mixerLine.szName);
            }
            m_numSrc = m_mixerLine.cConnections;
            _print_src_lines();
        }

	return;
}



int SoundResources::_initialize (const SoundOpenParameters& params)
{
	_init (params);
	_prepareBuffers ();
	
	//start continuous acquisition
	if ((m_err = waveInStart(m_WaveInHandle))) {
		printf("MicrophoneDeviceDriver: Error starting record! -- %08X\n", m_err);
	}
	m_InRecord = true;

	return 1;
}

//--------------------------------------------------------------------------------------
//       Class: SoundResources 
//      Method: (public)_uninitialize
// Description:  
//--------------------------------------------------------------------------------------
int SoundResources::_uninitialize (void)
{
	_bmutex.wait ();
	
	m_InRecord = false;
    mixerClose(m_MixerHandle);   // Close mixer
    waveInReset(m_WaveInHandle); // Reset the wave input device

    if(_rawBuffer != NULL){
        delete[] _rawBuffer;     // Delete the shared buffer
        _rawBuffer = NULL;
    }
    _bmutex.post ();

	return 1;
}


int SoundResources::_init (const SoundOpenParameters& params)
{
	//----------------------------------------------------------------------
	//  Initalize local parameters with the open params. It the open param
	//  is != 0 then the local param is updated if not the default value is
	//  used.
	//  Default configuration:
	//  16-bit, 44KHz, stereo
	//----------------------------------------------------------------------
	if (params.m_Channels      != 0) channels       = params.m_Channels;
	if (params.m_SamplesPerSec != 0) freqSample     = params.m_SamplesPerSec;
	if (params.m_BitsPerSample != 0) nBitsSample    = params.m_BitsPerSample;
	if (params.m_BufferLength  != 0) dwBufferLength = params.m_BufferLength;

	//----------------------------------------------------------------------
	//  Initialize the wave in
	//----------------------------------------------------------------------
	m_waveFormat.wFormatTag 	 = WAVE_FORMAT_PCM;
	m_waveFormat.nChannels 		 = channels;
	m_waveFormat.nSamplesPerSec  = freqSample;
	m_waveFormat.wBitsPerSample  = nBitsSample;
	m_waveFormat.nBlockAlign     = m_waveFormat.nChannels * (m_waveFormat.wBitsPerSample/8);
	m_waveFormat.nAvgBytesPerSec = m_waveFormat.nSamplesPerSec * m_waveFormat.nBlockAlign;
	m_waveFormat.cbSize          = 0;

	m_err = waveInOpen(&m_WaveInHandle, 
					   WAVE_MAPPER, 
					   &m_waveFormat, 
					   (DWORD)params.m_callbackthread_identifier, //Here I have to add the thread ID 
					   (DWORD)this, 
					   CALLBACK_THREAD);

	if (m_err != MMSYSERR_NOERROR) {
		printf("Can't open WAVE In Device! %d",m_err);
		return(-2);
	} 

	//----------------------------------------------------------------------
	//  Initialize Mixter
	//----------------------------------------------------------------------
	m_err = mixerOpen(&m_MixerHandle, 
					  (DWORD)m_WaveInHandle, 
					  0, 
					  0, 
					  MIXER_OBJECTF_HWAVEIN);

	if (m_err != MMSYSERR_NOERROR) {
		printf("yarpsounddriver: Device does not have mixer support! -- %08X\n", m_err);
	}

	//----------------------------------------------------------------------
	//  Print all the present lines in the audio interface
	//----------------------------------------------------------------------
	printf("MicrophoneDeviceDriver: LINES PRESENT\n");
	_print_dst_lines();
	
	//----------------------------------------------------------------------
	// This device should have a WAVEIN destination line. Let's get its ID so
	// that we can determine what source lines are available to record from
	//----------------------------------------------------------------------
	m_mixerLine.cbStruct        = sizeof(MIXERLINE);
	m_mixerLine.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_WAVEIN;

	m_err = mixerGetLineInfo((HMIXEROBJ)m_MixerHandle, 
							 &m_mixerLine, 
							 MIXER_GETLINEINFOF_COMPONENTTYPE);

	if (m_err != MMSYSERR_NOERROR) {
		printf("Device does not have a WAVE recording control! -- %08X\n", m_err);
	}
	
	m_numSrc = m_mixerLine.cConnections; // Get how many source lines are available from which to record. 
	m_err = _select_line(MIXERLINE_COMPONENTTYPE_SRC_LINE); //select default source line

	if (m_err == -1){ //Not line found, trying the auxiliary....
		m_err = _select_line(MIXERLINE_COMPONENTTYPE_SRC_AUXILIARY);
	}

	if (m_err != 1)
		printf("MicrophoneDeviceDriver: Atention Source line not found!!!\n");
		
	//----------------------------------------------------------------------
	// Initialize the local buffers 
	//----------------------------------------------------------------------
	_bmutex.wait ();
	_rawBuffer = new unsigned char [dwBufferLength];
    //	ACE_ASSERT (_rawBuffer != NULL);
	_bmutex.post ();

	return 1; 
}



// ===  FUNCTION  ======================================================================
// 
//         Name: RES  
// 
//  Description: This is a convenient function to recover the pointer to the SoundResources
//  object  
// 
//    Author:  Ing. Carlos Beltran
//  Revision:  none
// =====================================================================================
inline SoundResources& RES(void *res) 
{ 
	return *(SoundResources *)res; 
}

//--------------------------------------------------------------------------------------
//       Class:  SoundResources
//      Method:  acquireBuffer
// Description:  This method blocks the access to the lockable buffer. This allows to
// obtain access to a clear copy without DMA interferences
//--------------------------------------------------------------------------------------
int SoundResources::acquireBuffer (void *buffer)
{
	_bmutex.wait ();
	(*(unsigned char **)buffer) = _rawBuffer;

	return 1;
}

//--------------------------------------------------------------------------------------
//       Class:  SoundResources
//      Method:  releaseBuffer
// Description:  This method release the mutex controlling the exclusive access to the 
// buffer containing the data, thus the thread can write in the common buffer getting the
// data from the memory used by the DMA.
//--------------------------------------------------------------------------------------
int SoundResources::releaseBuffer ()
{
	_canpost = true;
	_bmutex.post ();

	return 1;
}

//--------------------------------------------------------------------------------------
//       Class:  MicrophoneDeviceDriver
//      Method:  waitOnNewFrame
// Description:  This call blocks until new data is available in the common buffer
//--------------------------------------------------------------------------------------
int SoundResources::waitOnNewFrame ()
{
	_new_frame.wait ();

	return 1;
}


void SoundResources::run()
{
	MSG		msg;
	
	//----------------------------------------------------------------------
	//  Wait for a message sent by the audio driver
	//----------------------------------------------------------------------

	while ((GetMessage(&msg, 0, 0, 0) == 1) && isRunning()) 
        {
            switch (msg.message) {
			case MM_WIM_DATA: //Buffer filled
				/*********************************************************************************
				 * the msg.lParam contains a pointer to the WAVEHDR structure for the filled buffer. *
				 *********************************************************************************/
				if (((WAVEHDR *)msg.lParam)->dwBytesRecorded) {
					//----------------------------------------------------------------------
					//  Here write in the local buffer using the syncronization mutexes
					//----------------------------------------------------------------------
					if (_bmutex.check()) { // buffer acquired. Reading from the buffer
						if (m_InRecord)
							memcpy (_rawBuffer, 
									((WAVEHDR *)msg.lParam)->lpData, 
                                    ((WAVEHDR *)msg.lParam)->dwBytesRecorded); // Note: is this right?
                                                                               // The buffer could not be full...
						if (_canpost) {
							_canpost = false;
							_new_frame.post();
						}
						_bmutex.post ();
					}
					else
                        {
                            //----------------------------------------------------------------------
                            //  can't acquire, it means the buffer is still in use.
                            //  silently ignores this condition.
                            //----------------------------------------------------------------------
                            //ACE_DEBUG ((LM_DEBUG, "lost a frame, acq thread\n"));
                            printf("lost a frame, acq thread\n");
                        }
				}

				//----------------------------------------------------------------------
				//  Requeue the used buffer	
				//----------------------------------------------------------------------
				waveInAddBuffer(m_WaveInHandle, 
								(WAVEHDR *)msg.lParam, 
								sizeof(WAVEHDR));

				break;
				
				/* Our main thread is opening the WAVE device */
			case MM_WIM_OPEN:
				//printf( "ace_Debug: MicrophoneDeviceDriver: sound device opened\n");
				break;
				
				/* Our main thread is closing the WAVE device */
			case MM_WIM_CLOSE:
				break;
				
			default:
				//ACE_DEBUG ((LM_DEBUG, "yarpsounddriver: received an unknown message\n"));
				break;
				
            }
        }
}



MicrophoneDeviceDriver::MicrophoneDeviceDriver() 
{
    dsp = -1;
	system_resources = (void *) (new SoundResources);
}



MicrophoneDeviceDriver::~MicrophoneDeviceDriver() 
{

	if (system_resources != NULL)
		delete (SoundResources *)system_resources;
	system_resources = NULL;
    close();
}

bool MicrophoneDeviceDriver::open(yarp::os::Searchable& config) 
{
	int ret=1;
	SoundOpenParameters *res=new SoundOpenParameters();
	SoundResources& d = RES(system_resources);

	d.start();  //Start the thread first. The identifier is necesary in the lower level	

	/***************************************************************************
	 * The thread ID is necesary to run the waveIn call that uses it to assing *
	 * the callback message to this thread. Therefore, the Body part of the    *
	 * thread will receive the messages (linked internally to hardware         *
	 * interrupts) generated by the low level sound driver.                    *
	 ***************************************************************************/
	res->m_callbackthread_identifier = d.getKey();	
	ret = d._initialize (*(SoundOpenParameters *)res);
	if (ret)
        {
            printf("returning true\n");
            return true;
        }
	else 
        {		
            printf("returning false\n");
            return false;
        }
}

bool MicrophoneDeviceDriver::close(void) 
{
	SoundResources& d = RES(system_resources);
	d.stop(); //This ends the thread. Will see if is really necessary in the sound driver	
	int ret = d._uninitialize ();
    if(ret)
		return true;
	else
		return false;
}


bool MicrophoneDeviceDriver::getSound(yarp::sig::Sound& sound) 
{
	SoundResources& d = RES(system_resources);
	unsigned char *tmp;

	d.waitOnNewFrame ();
	d.acquireBuffer(&tmp);
    short int *buffer = (short int*)tmp;
	sound.resize(d.numSamples,d.channels);
    sound.setFrequency(d.freqSample);
	
	for (int a=0;a<d.numSamples; a++)
		for(int c = 0; c < d.channels; c++)
			sound.set(buffer[a*2+c],a,c);
	d.releaseBuffer ();
    return true;
}

bool MicrophoneDeviceDriver::startRecording()
{
    return false;
}

bool MicrophoneDeviceDriver::stopRecording()
{
    return false;
}
