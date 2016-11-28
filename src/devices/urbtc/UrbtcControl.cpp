/*
 * Copyright (C) 2007 Jonas Ruesch
 * CopyPolicy: Released under the terms of the LGPLv2.1 or later, see LGPL.TXT
 *
 */


#include "UrbtcControl.h"

#include "../../../ControlBoardInterfacesImpl.inl"

yarp::dev::UrbtcControl::UrbtcControl() : ImplementPositionControl<UrbtcControl, IPositionControl>(this), 
                                          ImplementVelocityControl<UrbtcControl, IVelocityControl>(this),
                                          ImplementPidControl<UrbtcControl, IPidControl>(this),
                                          ImplementEncoders<UrbtcControl, IEncoders>(this),
                                          ImplementControlLimits<UrbtcControl, IControlLimits>(this)
{

	_intNumControllers = 0;

    _devicePrefix = "/dev/urbtc";
    _intOffset = 32767;

    _pidAccuracy     = 1000.0;
    
	// *** default values ***
	// without any gear
	// 965 tics per radian
    // 200'000 is _approximately_ 180 degrees with a gear of 1:66
    _dblFactDeg2Raw = 200000.0/180.0;
	_intLimitDefault = 32000;

    _pidPosDefault.kp = 0.5;
    _pidPosDefault.ki = 0.0;
    _pidPosDefault.kd = 0.5;

    _pidVelDefault.kp = 0.0;
    _pidVelDefault.ki = 0.0;
    _pidVelDefault.kd = 20.0;           // strange, but necessary for velocity control (?)

	// *** end of default ***

	// encoder counter thread
    _threadRate = 5;                   // milliseconds
    _periodTicks = 65535;               // encoder ticks
    _periodTicksHalf = _periodTicks/2;   // +/- encoder ticks
    _posAccuracy = 1000;                  // encoder ticks +/-
}

yarp::dev::UrbtcControl::~UrbtcControl(){

}

bool yarp::dev::UrbtcControl::open(yarp::os::Searchable& config){
    
//	ConstString fname = 
//             config.check("source",
//                         Value("default.avi"),    // a default value
//                         "movie file to read from").asString();


	//cout << "Urbtc config: " << config.toString() << endl;
	
	 // checking for number of attached devices
    _intNumControllers = getNumberOfDevices();
    if (_intNumControllers == 0){
        printf("No controller device files found. Controllers attached? Access rights ok?\n");
        return false;
    }

	// init data structures
    setDeviceFilenames(_intNumControllers);
    _fd = new int[_intNumControllers];
    _cmd = new ccmd[_intNumControllers];
    _out = new uout[_intNumControllers];
    _in = new uin[_intNumControllers];
    _pos = new long[_intNumControllers];
    _posOld = new long[_intNumControllers];
    _posDesired = new long[_intNumControllers];
    _periodCounter = new int[_intNumControllers];
    for (int i = 0; i < _intNumControllers; i++){
        _pos[i] = 0;
        _posOld[i] = 0;
        _posDesired[i] = 0;
        _periodCounter[i] = 0;
    }

	bool configValid = true;

	// encoder tic factors
	double *encoderFactors = new double[_intNumControllers];
	if (config.check("encoderFactors", "how many encoder tics are 1 radian/degree? List of double values (1 value for each axis)")){
		Bottle botEncoderFactors = config.findGroup("encoderFactors");
		if (botEncoderFactors.size() > _intNumControllers){ // bottle contains key as well
			for (int i = 1; i <= _intNumControllers; i++)
					encoderFactors[i-1] = botEncoderFactors.get(i).asDouble();
		}
		else{
			configValid = false;
			printf("Caution: UrbtcControl::open(): encoderFactors set to default, number of encoder factors in configuration too low.\n");
		}
	}
	else{
		configValid = false;
        }
	if (!configValid){
		// default configuration
		for (int i = 0; i < _intNumControllers; i++)
			encoderFactors[i] = _dblFactDeg2Raw;
	}

	configValid = true;

	// axis map
	int *axisMap = new int[_intNumControllers];
	if (config.check("axisMap","Mapping of axis indices. List of int values (1 value for each axis)")){
		Bottle botAxisMap = config.findGroup("axisMap");
		if (botAxisMap.size() > _intNumControllers){ // bottle contains key as well
			for (int i = 1; i <= _intNumControllers; i++)
					axisMap[i-1] = botAxisMap.get(i).asInt();
		}
		else{
			configValid = false;
			printf("Caution: UrbtcControl::open(): axisMap set to default, number of values in configuration too low.\n");
		}
	}
	else{
		configValid = false;
		printf("Caution: UrbtcControl::open(): axisMap set to default, values not found in configuration.\n");
        }
	if (!configValid){
		// default configuration
		for (int i = 0; i < _intNumControllers; i++)
			axisMap[i] = i;
	}

	configValid = true;

	// limits
	_limitsMax = new int[_intNumControllers];
	_limitsMin = new int[_intNumControllers];
    config.check("limitsMax", "+ limits in radian. List of double values (1 for each axis)."); // for --verbose switch only
    config.check("limitsMin","- limits in radian, List of double values (1 for each axis).");  // for --verbose switch only
	if (config.check("limitsMax") &&
        config.check("limitsMin")){
		Bottle botLimitsMin = config.findGroup("limitsMin");
		Bottle botLimitsMax = config.findGroup("limitsMax");
		if (botLimitsMin.size() > _intNumControllers && botLimitsMax.size() > _intNumControllers){ // bottle contains key as well
			for (int i = 1; i <= _intNumControllers; i++){
					_limitsMin[i-1] = (int)(botLimitsMin.get(i).asDouble() * encoderFactors[i-1]);
					_limitsMax[i-1] = (int)(botLimitsMax.get(i).asDouble() * encoderFactors[i-1]);
			}
		}
		else{
			configValid = false;
			printf("Caution: UrbtcControl::open(): limitsMin, limitsMax set to default, number of values in configuration too low.\n");
		}
	}
	else{
		configValid = false;
		printf("Caution: UrbtcControl::open(): limitsMin, limitsMax set to default, values not found in configuration.\n");
        }
	if (!configValid){
		// default configuration
		for (int i = 0; i < _intNumControllers; i++){
			_limitsMin[i] = -_intLimitDefault;
			_limitsMax[i] = _intLimitDefault;
		}
	}

	configValid = true;
	
	// pid values
    config.check("kp", "The p values for the PID controller. List of double values (1 for each axis).");
    config.check("ki", "The i values for the PID controller. List of double values (1 for each axis).");
    config.check("kd", "The d values for the PID controller. List of double values (1 for each axis).");
	if (config.check("kp") &&
        config.check("ki") &&
        config.check("kd")){
		Bottle botKp = config.findGroup("kp");
		Bottle botKi = config.findGroup("ki");
		Bottle botKd = config.findGroup("kd");
		if (botKp.size() > _intNumControllers && botKi.size() > _intNumControllers && botKd.size() > _intNumControllers){ // bottle contains key as well
			for (int i = 1; i <= _intNumControllers; i++){
				 for (int j=0; j<4; j++) {
						_out[i-1].ch[j].x = 0;          // target position
						_out[i-1].ch[j].d = 0;          // target velocity
						_out[i-1].ch[j].kp = (signed short)(botKp.get(i).asDouble() * _pidAccuracy);
						_out[i-1].ch[j].kpx = (unsigned short) _pidAccuracy;        // P gain  = 5/1 = 5  
						_out[i-1].ch[j].kd = (signed short)(botKd.get(i).asDouble() * _pidAccuracy);         
						_out[i-1].ch[j].kdx =(unsigned short) _pidAccuracy;        // D gain = 2/1 = 2   
						_out[i-1].ch[j].ki = (signed short)(botKi.get(i).asDouble() * _pidAccuracy);         
						_out[i-1].ch[j].kix = (unsigned short)_pidAccuracy;        // I gain = 0/1 = 0
                        //printf ("kp nomin: %lf\n" , (botKp.get(i).asDouble() * _pidAccuracy));
                        //printf ("kd nomin: %lf\n" , (botKd.get(i).asDouble() * _pidAccuracy));
                        //printf ("ki nomin: %lf\n" , (botKi.get(i).asDouble() * _pidAccuracy));
				}
			}
		}
		else{
			configValid = false;
			printf("Caution: UrbtcControl::open(): kp, ki, kd set to default, number of values in configuration too low.\n");
		}
	}
	else{
		configValid = false;
		printf("Caution: UrbtcControl::open(): kp, ki, kd set to default, values not found in configuration.\n");
        }
        
	if (!configValid){
		// default configuration
		for (int i = 0; i < _intNumControllers; i++){
			for (int j=0; j<4; j++) {
					_out[i].ch[j].x = 0;          // target position
					_out[i].ch[j].d = 0;          // target velocity
					_out[i].ch[j].kp = (signed short)(_pidPosDefault.kp * _pidAccuracy);
					_out[i].ch[j].kpx = (unsigned short) _pidAccuracy;        // P gain  = 5/1 = 5  
					_out[i].ch[j].kd = (signed short)(_pidPosDefault.kd * _pidAccuracy);         
					_out[i].ch[j].kdx =(unsigned short) _pidAccuracy;        // D gain = 2/1 = 2   
					_out[i].ch[j].ki = (signed short)(_pidPosDefault.ki * _pidAccuracy);         
					_out[i].ch[j].kix = (unsigned short)_pidAccuracy;        // I gain = 0/1 = 0   
			}
		}
	}

    // motion done accuracy
    _posAccuracy = config.check("motionDoneAccuracy",
                                Value(1000),
                                "Integer value specifying +/- range of encoder tics").asInt();

    
   // encoder zeros
   // limit exceedings
    double *encoderZeros = new double[_intNumControllers];
    _limitsExceeding = new bool[_intNumControllers];
    for (int i = 0; i < _intNumControllers; i++){
        encoderZeros[i] = 0.0;
        _limitsExceeding[i] = false;
    }

	// Wrapper initialization
    ImplementPositionControl<UrbtcControl, IPositionControl>::
            initialize(_intNumControllers, axisMap, encoderFactors, encoderZeros);

    ImplementVelocityControl<UrbtcControl, IVelocityControl>::
            initialize(_intNumControllers, axisMap, encoderFactors, encoderZeros);

    ImplementPidControl<UrbtcControl, IPidControl>::
            initialize(_intNumControllers, axisMap, encoderFactors, encoderZeros);

    ImplementEncoders<UrbtcControl, IEncoders>::
            initialize(_intNumControllers, axisMap, encoderFactors, encoderZeros);
    
    ImplementControlLimits<UrbtcControl, IControlLimits>::
            initialize(_intNumControllers, axisMap, encoderFactors, encoderZeros);



    // output devices found 
    printf("Number of urbtc devices found: %d\n", _intNumControllers);

    // opening devices
    for (int i = 0; i < _intNumControllers; i++){   // for all found devices

        printf("Trying to open ");
        puts (_deviceFilename[i]);

        // open devicefile
        if ( (_fd[i] = ::open(_deviceFilename[i], O_RDWR)) == -1 ){
            printf("Error opening device number: %d\n", i);
            return false;
        }

        // init ccmd 

        // position
        _cmd[i].retval = 0; 
        _cmd[i].setoffset  = CH0 | CH1 | CH2 | CH3;
        _cmd[i].setcounter = CH0 | CH1 | CH2 | CH3;
        _cmd[i].resetint   = CH0 | CH1 | CH2 | CH3;
        _cmd[i].selin = SET_SELECT | SET_CH2_HIN;
        _cmd[i].dout = 0;
        _cmd[i].selout = SET_SELECT | CH2;
        _cmd[i].offset[0] = _cmd[i].offset[1] = _cmd[i].offset[2] = _cmd[i].offset[3] = _intOffset;
        _cmd[i].counter[0] = _cmd[i].counter[1] = _cmd[i].counter[2] = _cmd[i].counter[3] = 0;
        _cmd[i].posneg = SET_POSNEG | CH0 | CH1 | CH2 | CH3; /*POS PWM out*/
        _cmd[i].breaks = SET_BREAKS | CH0 | CH1 | CH2 | CH3; /*No Brake*/
        _cmd[i].magicno = 0x00;
        _cmd[i].wrrom = 0; /* WR_MAGIC | WR_OFFSET | WR_SELOUT;*/  


        // do some ioctl stuff
        if (ioctl(_fd[i], URBTC_COUNTER_SET) < 0)   {			
            printf("Error on ioctl() for device number: %d\n",i);
            return false;
        }	  		

        // write the cmd struct out to the device 
        if (write(_fd[i], &_cmd[i], sizeof(_cmd[i])) < 0)    {			
            printf("Error writing cmd struct to device number: %d\n",i);
            return false;
        }
        
 	    // do some more ioctl() stuff
        if (ioctl(_fd[i], URBTC_DESIRE_SET) < 0)  {
            printf("Error ioctl() URBTC_DESIRE_SET\n");
            return false;
        }
        if (ioctl(_fd[i], URBTC_CONTINUOUS_READ) < 0)   {
            printf("Error ioctl() URBTC_CONTINUOUS_READ\n");
            return false;
        }
        if (ioctl(_fd[i], URBTC_BUFREAD) < 0) {
            printf("Error ioctl() URBTC_BUFREAD\n");
            return false;
        }
        int test;
        if ((test = read(_fd[i], &_in[i], sizeof(_in[i]))) != sizeof(_in[i])) {
            printf("Warning: read size mismatch %d != %d\n", test, sizeof(_in[i]));
        }

        printf("Successfully opened.\n");
    }

    Thread::start();

    return true;
}

bool yarp::dev::UrbtcControl::close(){

    Thread::stop ();
    
    ImplementPositionControl<UrbtcControl, IPositionControl>::uninitialize ();
    ImplementVelocityControl<UrbtcControl, IVelocityControl>::uninitialize ();
    ImplementPidControl<UrbtcControl, IPidControl>::uninitialize();
    ImplementEncoders<UrbtcControl, IEncoders>::uninitialize();
    ImplementControlLimits<UrbtcControl, IControlLimits>::uninitialize();

    printf("\n");
    for (int i=0; i < _intNumControllers; i++){
        printf("Closing urbtc controller device ");
        puts(_deviceFilename[i]);
        ::close(_fd[i]);
    }
    return true;
}


// ********************** position raw ****************************


bool yarp::dev::UrbtcControl::getAxes(int *ax){
    //printf("num axes: " << _intNumControllers << endl;
    *ax = (int)_intNumControllers;
    return true;
}


bool yarp::dev::UrbtcControl::setPositionMode(){
    // caution, method sets new pid values.
    // setting here pid values which are known to work for position mode
//    for (int i = 0; i < _intNumControllers; i++) {
//		_mutex.wait();
//        for (int j=0; j<4; j++) {
//            _out[i].ch[j].x = 0;          // target position
//            _out[i].ch[j].d = 0;          // target velocity
//            _out[i].ch[j].kp = (signed short)(_pidPosDefault.kp*_pidAccuracy);
//            _out[i].ch[j].kpx = (unsigned short)(_pidAccuracy);        // P gain  = 5/1 = 5  
//            _out[i].ch[j].kd = (signed short) (_pidPosDefault.kd * _pidAccuracy);         
//            _out[i].ch[j].kdx = (unsigned short) (_pidAccuracy);        // D gain = 2/1 = 2   
//            _out[i].ch[j].ki = (signed short)(_pidPosDefault.ki * _pidAccuracy);         
//            _out[i].ch[j].kix = (unsigned short) (_pidAccuracy);        // I gain = 0/1 = 0   
//        }
//		_mutex.post();
//    }
    return true;
}

bool yarp::dev::UrbtcControl::positionMoveRaw(int j, double ref){

     // check axis
    if (j > -1 && j < _intNumControllers){
        // check limits
        if ( ((int)ref) > _limitsMin[j] &&
             ((int)ref) < _limitsMax[j]){
            _mutex.wait();
            _posDesired[j] = (long)ref;
            _out[j].ch[0].x = _out[j].ch[1].x = _out[j].ch[2].x = _out[j].ch[3].x  = (short)ref; 
            if (write(_fd[j], &_out[j], sizeof(_out[j])) < 0){
                _mutex.post();
                return false;
            }
            _mutex.post();
            return true;
        }
        printf("Axis %d exceeded limts. Requested value: %d Limits: %d %d\n",
               j, ((int)ref), _limitsMin[j], _limitsMax[j]);
    }
    return false;
    
//         if (j > -1 && j < _intNumControllers){
// 
//             unsigned short velMoveClose = 200;
//             long toleranceMoveClose = 100;
//             short localPos = 0;
// 
//             _posDesired[j] = (long)ref;
//             //cout << "pos desired: " << _posDesired[j] << endl;  
//            
//             // move close to position with given tolerance if ref is not in period 0
//             velocityMoveClose(j,(long)ref, velMoveClose, toleranceMoveClose);
//             //positionMoveClose(j,(long)ref, 500, 500);
// 
//             // calculate the absolute position in local position period (-32768 to 32767)
//             double posInPeriods = ((double)ref)/((double)_periodTicks);
//             localPos  = (short)((posInPeriods - ((double)_periodCounter[j])) * _periodTicks);
//             
//             // do absolute positioning now within local period
//             _mutex.wait();
//             //cout << "pid: kp: " << _out[j].ch[0].kp << " kpx: " << _out[j].ch[0].kpx << " kd: " << _out[j].ch[0].kd << " kdx: " << _out[j].ch[0].kdx << endl;
//             _out[j].ch[0].x = _out[j].ch[1].x = _out[j].ch[2].x = _out[j].ch[3].x  = localPos; 
//             if (write(_fd[j], &_out[j], sizeof(_out[j])) < 0){
//                  _mutex.post();
//                  return false;
//              }
//             _mutex.post();
//             return true;
//         }
//         return false;


}

bool yarp::dev::UrbtcControl::positionMoveRaw(const double *refs){

    bool success = true;
    for (int i = 0; i < _intNumControllers; i++)
        success = success && positionMoveRaw(i, refs[i]);
    return success;
}

bool yarp::dev::UrbtcControl::relativeMoveRaw(int j, double delta){
    
    // check axis
    if ((j > -1) && (j < _intNumControllers)){
        // check limits
        //printf("out channel axis %d: %d\n", j, _out[j].ch[0].x);
        if ( ((int)delta + (int)_out[j].ch[0].x) > _limitsMin[j]  &&
             ((int)delta + (int)_out[j].ch[0].x) < _limitsMax[j] ){
            _mutex.wait();
            // for all channels of axis j
            _out[j].ch[0].x += (short)delta;
            _out[j].ch[1].x += (short)delta;
            _out[j].ch[2].x += (short)delta;
            _out[j].ch[3].x += (short)delta;
            _posDesired[j] = _out[j].ch[0].x;
            if (write(_fd[j], &_out[j], sizeof(_out[j])) < 0){
                _mutex.post();
                printf("writing failed!\n");
                return false;
            }
            _mutex.post();
            return true;
        }
        printf("Axis %d exceeded limts. Requested value: %d Limits: %d %d\n",
               j, ((short)delta + _out[j].ch[0].x), _limitsMin[j], _limitsMax[j]);
    }
    return false;
}

bool yarp::dev::UrbtcControl::relativeMoveRaw(const double *deltas){

    // no check on the array done!
    bool success = true;
    for (int i = 0; i < _intNumControllers; i++)
        success = success && relativeMoveRaw(i, deltas[i]);
    return success;
}

bool yarp::dev::UrbtcControl::checkMotionDoneRaw(int j, bool *flag){

    // being out of bounds is considered to be 'motion done'
    if ( _pos[j] > _limitsMax[j] || _pos[j] < _limitsMin[j]){
        //printf("motion done true due to limit!\n");
        *flag = true;
    }
    // take difference between t-1 and t into account too.
    else if ( (_pos[j] > (_posOld[j] - _posAccuracy)) &&
         (_pos[j] < (_posOld[j] + _posAccuracy)) &&
         (_pos[j] > (_out[j].ch[0].x - _posAccuracy)) &&
         (_pos[j] < (_out[j].ch[0].x + _posAccuracy)) ){
        *flag = true;
    }
    else
        *flag = false;
    return true;
}

bool yarp::dev::UrbtcControl::checkMotionDoneRaw(bool *flag){

    bool success = true;
    for (int i = 0; i < _intNumControllers; i++){
        success = success && checkMotionDoneRaw(i,&flag[i]);
    }
    return success;
}

bool yarp::dev::UrbtcControl::setRefSpeedRaw(int j, double sp) {
    printf("UrbtcControl::setRefSpeedRaw(): Not implemented.\n");
    bool success = true;
    return success;
}

bool yarp::dev::UrbtcControl::setRefSpeedsRaw(const double *spds) {
    printf("UrbtcControl::setSetRefSpeed(): Not implemented.\n");
    bool success = true;
    return success;
}

bool yarp::dev::UrbtcControl::setRefAccelerationRaw(int j, double acc) {
    printf("UrbtcControl::setRefAccelerationRaw(): Not implemented.\n");
    bool success = true;
    return success;
}

bool yarp::dev::UrbtcControl::setRefAccelerationsRaw(const double *accs) {
    printf("UrbtcControl::setRefAcceleartionsRaw(): Not implemented.\n");
    bool success = true;
    return success;
}

bool yarp::dev::UrbtcControl::getRefSpeedRaw(int j, double *ref) {
    printf("UrbtcControl::getRefSpeedRaw(): Not implemented.\n");
   bool success = true;
    return success;
}

bool yarp::dev::UrbtcControl::getRefSpeedsRaw(double *spds) {
    bool success = true;
    for (int i = 0; i < _intNumControllers; i++){
        success = success && getRefSpeedRaw(i,&spds[i]);
    }
    return success;
}

bool yarp::dev::UrbtcControl::getRefAccelerationRaw(int j, double *acc) {
    printf("UrbtcControl::getRefAccelarationRaw(): Not implemented.\n");
    return true;
}

bool yarp::dev::UrbtcControl::getRefAccelerationsRaw(double *accs) {
    bool success = true;
    for (int i = 0; i < _intNumControllers; i++){
        success = success && getRefAccelerationRaw(i,&accs[i]);
    }
    return success;
}

bool yarp::dev::UrbtcControl::stopRaw(int j) {
    
    if (j > -1 && j < _intNumControllers){
		_mutex.wait();
        _out[j].ch[0].d = _out[j].ch[1].d = _out[j].ch[2].d = _out[j].ch[3].d  = (short)0; 
		if (write(_fd[j], &_out[j], sizeof(_out[j])) < 0){
			_mutex.post();
            return false;
		}
		_mutex.post();
        return true;
    }
    return false;
}

bool yarp::dev::UrbtcControl::stopRaw() {

    for (int i = 0; i < _intNumControllers; i++){
		_mutex.wait();
        _out[i].ch[0].d = _out[i].ch[1].d = _out[i].ch[2].d = _out[i].ch[3].d  = (short)0; 
		if (write(_fd[i], &_out[i], sizeof(_out[i])) < 0){
            return false;
			_mutex.post();
		}
		_mutex.post();
    }
    return true;
}

// ********************** velocity raw ****************************


bool yarp::dev::UrbtcControl::setVelocityMode(){
    // caution, method sets new pid values.
    // setting here pid values which are known to work for velocity mode (strange controller behavior..)
//    for (int i = 0; i < _intNumControllers; i++) {
//		_mutex.wait();
//        for (int j=0; j<4; j++) {
//            _out[i].ch[j].x = 0;          // target position
//            _out[i].ch[j].d = 0;          // target velocity
//            _out[i].ch[j].kp = (signed short)(_pidVelDefault.kp * _pidAccuracy);
//            _out[i].ch[j].kpx = (unsigned short) (_pidAccuracy);        // P gain  = 5/1 = 5  
//            _out[i].ch[j].kd = (signed short)(_pidVelDefault.kd * _pidAccuracy);
//            _out[i].ch[j].kdx = (unsigned short) (_pidAccuracy);        // D gain = 2/1 = 2   
//            _out[i].ch[j].ki = (signed short) (_pidVelDefault.ki * _pidAccuracy);
//            _out[i].ch[j].kix = (unsigned short) (_pidAccuracy);        // I gain = 0/1 = 0   
//        }
//		_mutex.post();
//    }
    return true;
}

bool yarp::dev::UrbtcControl::velocityMoveRaw(int j, double sp){
    
    if (j > -1 && j < _intNumControllers){
		_mutex.wait();
        _out[j].ch[0].d = _out[j].ch[1].d = _out[j].ch[2].d = _out[j].ch[3].d  = (short)sp; 
        // writing
        if (write(_fd[j], &_out[j], sizeof(_out[j])) < 0){
			_mutex.post();
            return false;
        }
		_mutex.post();
    }
    return false;
}

bool yarp::dev::UrbtcControl::velocityMoveRaw(const double *sp){

    for (int i = 0; i < _intNumControllers; i++){
		_mutex.wait();
        _out[i].ch[0].d = _out[i].ch[1].d = _out[i].ch[2].d = _out[i].ch[3].d  = (int)sp; 
		if (write(_fd[i], &_out[i], sizeof(_out[i])) < 0){
			_mutex.post();
            return false;
		}
		_mutex.post();
    }
    return true;
}


// ********************** pid control raw ****************************

bool yarp::dev::UrbtcControl::setPidRaw(int j, const Pid &pid){

    if (j > -1 && j < _intNumControllers){
        for (int i = 0; i < 4; i++){
			_mutex.wait();
            _out[j].ch[i].kp = (signed short)(pid.kp * _pidAccuracy);
            _out[j].ch[i].kpx = (signed short)(_pidAccuracy);        // P gain  = 5/1 = 5  
            _out[j].ch[i].kd = (signed short)(pid.kd * _pidAccuracy);         
            _out[j].ch[i].kdx = (signed short)(_pidAccuracy);        // D gain = 2/1 = 2   
            _out[j].ch[i].ki = (signed short)(pid.ki * _pidAccuracy);         
            _out[j].ch[i].kix = (signed short)(_pidAccuracy);        // I gain = 0/1 = 0   
			_mutex.post();
		}
    }
    return true;
}

bool yarp::dev::UrbtcControl::setPidsRaw(const Pid *pids){

    for (int j = 0; j < _intNumControllers; j++){
        setPidRaw(j,pids[j]);
    }
    return true;
}


bool yarp::dev::UrbtcControl::setReferenceRaw(int j, double ref){
     printf("UrbtcControl::setReferenceRaw(): Not implemented.\n");
     return true;
}

bool yarp::dev::UrbtcControl::setReferencesRaw(const double *refs){
     printf("UrbtcControl::setReferencesRaw(): Not implemented.\n");
     return true;
}

bool yarp::dev::UrbtcControl::setErrorLimitRaw(int j, double limit){
     printf("UrbtcControl::setErrorLimitRaw(): Not implemented.\n");
     return true;
}

bool yarp::dev::UrbtcControl::setErrorLimitsRaw(const double *limits){
     printf("UrbtcControl::setErrorLimitsRaw(): Not implemented.\n");
     return true;
}

bool yarp::dev::UrbtcControl::getErrorRaw(int j, double *err){
     printf("UrbtcControl::getErrorRaw(): Not implemented.\n");
     return true;
}

bool yarp::dev::UrbtcControl::getErrorsRaw(double *errs){
     printf("UrbtcControl::getErrorsRaw(): Not implemented.\n");
     return true;
}

bool yarp::dev::UrbtcControl::getOutputRaw(int j, double *out){
     printf("UrbtcControl::getOutputRaw(): Not implemented.\n");
     return true;
}

bool yarp::dev::UrbtcControl::getOutputsRaw(double *outs){
     printf("UrbtcControl::getOutputsRaw(): Not implemented.\n");
     return true;
}

bool yarp::dev::UrbtcControl::getPidRaw(int j, Pid *pid){

    if (j > -1 && j < _intNumControllers){
        pid->kp = ((double)_out[j].ch[0].kp)/((double)_out[j].ch[0].kpx);
        pid->ki = ((double)_out[j].ch[0].ki)/((double)_out[j].ch[0].kix);
        pid->kd = ((double)_out[j].ch[0].kd)/((double)_out[j].ch[0].kdx);
        return true;
    }
    return false;
}

bool yarp::dev::UrbtcControl::getPidsRaw(Pid *pids){
    // ?? shouldn't argument be Pid **pids ?
    for (int i = 0; i < _intNumControllers; i++){
        pids[i].kp = ((double)_out[i].ch[0].kp)/((double)_out[i].ch[0].kpx);
        pids[i].ki = ((double)_out[i].ch[0].ki)/((double)_out[i].ch[0].kix);
        pids[i].kd = ((double)_out[i].ch[0].kd)/((double)_out[i].ch[0].kdx);
    }
    return true;
}

bool yarp::dev::UrbtcControl::getReferenceRaw(int j, double *ref){
    printf("UrbtcControl::getReferenceRaw(): Not implemented.\n");
    return true;
}

bool yarp::dev::UrbtcControl::getReferencesRaw(double *refs){
    printf("UrbtcControl::getReferencesRaw(): Not implemented.\n");
    return true;
}

bool yarp::dev::UrbtcControl::getErrorLimitRaw(int j, double *limit){
    printf("UrbtcControl::getErrorLimitRaw(): Not implemented.\n");
    return true;
}

bool yarp::dev::UrbtcControl::getErrorLimitsRaw(double *limits){
    printf("UrbtcControl::getErrorLimitsRaw(): Not implemented.\n");
    return true;
}

bool yarp::dev::UrbtcControl::resetPidRaw(int j){
    printf("UrbtcControl::resetPidRaw(): Not implemented.\n");
    return true;
}

bool yarp::dev::UrbtcControl::disablePidRaw(int j){
    printf("UrbtcControl::disablePidRaw(): Not implemented.\n");
    return true;
}

bool yarp::dev::UrbtcControl::enablePidRaw(int j){
    printf("UrbtcControl::enablePidRaw(): Not implemented.\n");
    return true;
}

/**
* Change the reference position from which position is calculated and controlled. 
* Caution, setting a new offset results in a movement: Fullfilling position request with respect to new origin.
* Caution, the offset value can be between 0 and 2*32768 only.
*/
bool yarp::dev::UrbtcControl::setOffsetRaw(int j, double v){
//     if (j > -1 && j < _intNumControllers){
//         _cmd[j].offset[0] = _cmd[j].offset[1] = _cmd[j].offset[2] = _cmd[j].offset[3] = (unsigned short)(((int)v) + ((int)_intOffset));
// 
//         // prepare for writing _cmd
//         if (ioctl(_fd[j], URBTC_COUNTER_SET) < 0)
//             fprintf(stderr, "UrbtcControl::setOffsetRaw: ioctl: URBTC_COUNTER_SET error\n");
//         
//         // write the cmd struct to the device
//         if (write(_fd[j], &_cmd[j], sizeof(_cmd[j])) < 0)    {			
//             cout << "UrbtcControl::setOffsetRaw: Error writing cmd struct to device number: " << j << endl;
//             return false;
//         }
// 
//         // prepare for writing _out again
//         if (ioctl(_fd[j], URBTC_DESIRE_SET) < 0) 
//             cout << "Error ioctl() URBTC_DESIRE_SET" << endl;
//         if (ioctl(_fd[j], URBTC_CONTINUOUS_READ) < 0)
//             cout << "Error ioctl() URBTC_CONTINUOUS_READ" << endl;
//         if (ioctl(_fd[j], URBTC_BUFREAD) < 0) 
//             cout << "Error ioctl() URBTC_BUFREAD" << endl;
// 
//         int test;
//         if ((test = read(_fd[j], &_in[j], sizeof(_in[j]))) != sizeof(_in[j])) {
//             cout << "Warning: read size mismatch " << test << " != " << sizeof(_in[j]) << endl;
//         }
// 
//         return true;
//     }
//     return false;
    printf("UrbtcControl::setOffsetRaw(): Not implemented.\n");
    return true;
}

// ********************* EncodersRaw ***********************

bool yarp::dev::UrbtcControl::resetEncoderRaw(int j){
    
    if(j > -1 && j < _intNumControllers){
    
		_mutex.wait();

        _cmd[j].retval = 0; 
        _cmd[j].setoffset  = CH0 | CH1 | CH2 | CH3;
        _cmd[j].setcounter = CH0 | CH1 | CH2 | CH3;
        _cmd[j].resetint   = CH0 | CH1 | CH2 | CH3;
        _cmd[j].selin = SET_SELECT | SET_CH2_HIN;
        _cmd[j].dout = 0;
        _cmd[j].selout = SET_SELECT | CH2;
        _cmd[j].offset[0] = _cmd[j].offset[1] = _cmd[j].offset[2] = _cmd[j].offset[3] = 32768;
        _cmd[j].counter[0] = _cmd[j].counter[1] = _cmd[j].counter[2] = _cmd[j].counter[3] = 0;
        _cmd[j].posneg = SET_POSNEG | CH0 | CH1 | CH2 | CH3; /*POS PWM out*/
        _cmd[j].breaks = SET_BREAKS | CH0 | CH1 | CH2 | CH3; /*No Brake*/
        _cmd[j].magicno = 0x00;
        _cmd[j].wrrom = 0; /* WR_MAGIC | WR_OFFSET | WR_SELOUT;*/  
    
        // prepare for writing _cmd
        if (ioctl(_fd[j], URBTC_COUNTER_SET) < 0)   {			
            printf("Error on ioctl() for device number: %d\n", j);
			_mutex.post();
            return false;
        }	  		
    
        // write the cmd struct out to the device 
        if (write(_fd[j], &_cmd[j], sizeof(_cmd[j])) < 0)    {			
            printf("Error writing cmd struct to device number: %d\n", j);
			_mutex.post();
            return false;
        }
    
        // prepare for writing _out again
        if (ioctl(_fd[j], URBTC_DESIRE_SET) < 0)  {
            printf("Error ioctl() URBTC_DESIRE_SET\n");
			_mutex.post();
            return false;
        }
        if (ioctl(_fd[j], URBTC_CONTINUOUS_READ) < 0)   {
            printf("Error ioctl() URBTC_CONTINUOUS_READ\n");
			_mutex.post();
            return false;
        }
        if (ioctl(_fd[j], URBTC_BUFREAD) < 0) {
            printf("Error ioctl() URBTC_BUFREAD\n");
			_mutex.post();
            return false;
        }
    
        // do a read 
        int test;
        if ((test = read(_fd[j], &_in[j], sizeof(_in[j]))) != sizeof(_in[j])) {
            printf("Warning: read size mismatch %d != %d\n", test, sizeof(_in[j]));
        }
    
        // set position to 0 (otherwise controller would move to the last specified position relative to new origin)
        positionMoveRaw(j, 0.0);
		_mutex.post();
        return true;
    }
    return false;
}

bool yarp::dev::UrbtcControl::resetEncodersRaw() {

    bool success = true;
    for (int i = 0; i < _intNumControllers; i++)
        success = success && resetEncoderRaw(i);
    return success;
//    cout << "UrbtcControl::resetEncoderRaw(): Not implemented." << endl;
}

bool yarp::dev::UrbtcControl::setEncoderRaw(int j, double val) {
    printf("UrbtcControl::resetEncoderRaw(): Not implemented.\n");
    return true;
}

bool yarp::dev::UrbtcControl::setEncodersRaw(const double *vals) {
    printf("UrbtcControl::resetEncoderRaw(): Not implemented.\n");
    return true;
}

bool yarp::dev::UrbtcControl::getEncoderRaw(int j, double *v){
    

    // get the accumulated encoder value
    if (j > -1 && j < _intNumControllers){
        //_mutex.wait();
        //cout << "getting encoder value: " << (double)_pos[j] << endl;
        *v = (double)_pos[j];
        //_mutex.post();
    }
    return true;

        // get the real encoder value
//     if (j > -1 && j < _intNumControllers){
//         if ((read(_fd[j], &_in[j], sizeof(_in[j]))) != sizeof(_in[j])) {
//             cout << "UrbtcControl::setEncoderRaw(): Warning: read size mismatch." << endl;
//         }
//         // actually only channel 0 should contain the encoder value, the others should be 0
//         *v = _in[j].ct[0] + _in[j].ct[1] + _in[j].ct[2] + _in[j].ct[3];
//         return true;
//     }
//     return false;
}

bool yarp::dev::UrbtcControl::getEncodersRaw(double *encs) {

    // get the accumulated encoder value
    _mutex.wait();
    for (int i = 0; i < _intNumControllers; i++){
        encs[i] = _pos[i];
    }
    _mutex.post();
    return true;

        // get the real encoder value
//     for (int i = 0; i < _intNumControllers; i++){
//         if ((read(_fd[i], &_in[i], sizeof(_in[i]))) != sizeof(_in[i])) {
//             cout << "UrbtcControl::setEncodersRaw(): Warning: read size mismatch." << endl;
//         }
//         // actually only channel 0 should contain the encoder value, the others should be 0
//         encs[i] = _in[i].ct[0] + _in[i].ct[1] + _in[i].ct[2] + _in[i].ct[3];
//     }
//     return true;
}

bool yarp::dev::UrbtcControl::getEncoderSpeedRaw(int j, double *sp) {
    printf("UrbtcControl::getEncoderSpeedRaw(): Not implemented.\n");
    return true;
}

bool yarp::dev::UrbtcControl::getEncoderSpeedsRaw(double *spds) {
    printf("UrbtcControl::getEncoderSpeedsRaw(): Not implemented.\n");
    return true;
}

bool yarp::dev::UrbtcControl::getEncoderAccelerationRaw(int j, double *spds) {
    printf("UrbtcControl::getEncoderAccelerationRaw(): Not implemented.\n");
    return true;
}
 
bool yarp::dev::UrbtcControl::getEncoderAccelerationsRaw(double *accs) {
    printf("UrbtcControl::getEncoderAccelerationsRaw(): Not implemented.\n");
    return true;
}

// ********************* Limits Raw ***********************

bool yarp::dev::UrbtcControl::setLimitsRaw (int axis, double min, double max){
    
    //cout << "set limits called" << endl;
    if(axis > -1 && axis < _intNumControllers){
        _mutex.wait();
        _limitsMax[axis] = (int)max;
        _limitsMin[axis] = (int)min;
        //cout << "Limits set (raw) to: " << min << " " << max << " for axis: " << axis;
        _mutex.post();
        return true;
    }
    return false;
}
    
bool yarp::dev::UrbtcControl::getLimitsRaw (int axis, double *min, double *max){
     
    if(axis > -1 && axis < _intNumControllers){
         *min = (double)_limitsMin[axis];
         *max = (double)_limitsMax[axis];
         return true;
     }
     else
         return false;
}


// ********************* PRIVATE ***********************


int yarp::dev::UrbtcControl::getNumberOfDevices(){
    
    bool            blnFound = true;
    int             deviceNumber = 0;
    int             fd;
    char            device[30];
    char            chrNumber[30];

    while(blnFound){
        strcpy(device, _devicePrefix);
        sprintf (chrNumber, "%d", deviceNumber);
        strcat(device, chrNumber);
        if ( (fd = ::open(device, O_RDWR)) == -1 ) {
            blnFound = false;
        }
        else {
            ::close(fd);
            deviceNumber++;
        }  
    }
    return deviceNumber;
}

void yarp::dev::UrbtcControl::setDeviceFilenames(int num){

    char curr[30];
    _deviceFilename = new char*[num];
    for (int i = 0; i < num; i++){
        sprintf(curr, "%d", i);
        _deviceFilename[i] = new char[30];
        strcpy(_deviceFilename[i], _devicePrefix);
        strcat(_deviceFilename[i], curr);
    }
}

// Runnable

void yarp::dev::UrbtcControl::run() {

    double before = 0;
    double now = 0;
    double k = 0;
    double *oldPos = new double[_intNumControllers];
    double *newPos = new double[_intNumControllers];

    for (int i = 0; i < _intNumControllers; i++){
        oldPos[i] = 0.0;
        newPos[i] = 0.0;
    }

    while (!isStopping()){
    
        before = yarp::os::Time::now();
 
        for (int i = 0; i < _intNumControllers; i++){

			// calculate the accumulated position value
            oldPos[i] = newPos[i];
            newPos[i] = getUrbtcEncoderValue(i);
            
            if ( (oldPos[i] - newPos[i]) > 32000)    // switched from 32768 to -32768
                _periodCounter[i]++;
            if ( (oldPos[i] - newPos[i]) < -32000)   // switched from -32768 to 32768
                 _periodCounter[i]--;
            _mutex.wait();
            _posOld[i] = _pos[i];
            _pos[i] = (long)(_periodCounter[i] * _periodTicks) + (long)newPos[i]; 
            _mutex.post();

			// make sure motors do not run over limits
			if (_pos[i] > _limitsMax[i] || _pos[i] < _limitsMin[i]){
				//printf("Caution: UrbtcControl::run(): Axis %d is exceeding limits! Stopped.\n", i);
                printf("Caution: UrbtcControl::run(): Axis %d is exceeding limits, position is at: %d!\n", i, (int)_pos[i]);
				//if (!_limitsExceeding[i]) // dangerous if in velocity mode
				//stopRaw(i);	// stop first time exceeding limit
				_limitsExceeding[i] = true;
			}
			else{
				_limitsExceeding[i] = false;
			}
        }
        //cout << "Positions: " << _pos[0] << " " << _pos[1] << " " << _pos[2] << " old positions: " << _posOld[0] << " " << _posOld[1] << " " << _posOld[2] << " period array: " << _periodCounter[0] << " " << _periodCounter[1] << " " << _periodCounter[2] << " real encoder value: " << newPos[0] << " " << newPos[1] << " " << newPos[2] << endl;


        now = yarp::os::Time::now();
        if ((now - before)*1000 < _threadRate){

            k = double(_threadRate)/1000.0-(now-before);
            yarp::os::Time::delay(k);
        }
        else{
            printf("UrbtcControl::run(): Encoder value reading too slow...");
        }

    }
    delete [] oldPos;
}

short yarp::dev::UrbtcControl::getUrbtcEncoderValue(int axis){

    _mutex.wait();
    if ((read(_fd[axis], &_in[axis], sizeof(_in[axis]))) != sizeof(_in[axis])) {
        printf("UrbtcControl::setEncoderRaw(): Warning: read size mismatch.\n");
    }
    // actually only channel 0 should contain the encoder value, the others should be 0
    _mutex.post();
    return (_in[axis].ct[0] + _in[axis].ct[1] + _in[axis].ct[2] + _in[axis].ct[3]);
}

//bool yarp::dev::UrbtcControl::positionMoveClose(int axis, long position, unsigned short stepSize, long tolerance){
//
//    if (axis > -1 && axis < _intNumControllers){
//        if ( (abs(position - _pos[axis])) > tolerance){
//        
//            int   msSleep = 10;
//            short signedStepSize = stepSize;
//
//             check direction, change if negative speed required
//            if ( (position - _pos[axis]) < 0)
//                signedStepSize = -signedStepSize;
//
//            _mutex.wait();
//             let the motor run
//             split up in two whiles to make sure tolerance band is not 'over-jumped'
//            if (signedStepSize > 0){
//                while( _pos[axis] < (position - tolerance)) {   // _pos is updated by position update thread
//                    cout << "writing: " << _pos[axis]+signedStepSize << endl;
//                    _out[axis].ch[0].x = _out[axis].ch[axis].x = _out[axis].ch[2].x = _out[axis].ch[3].x  = _pos[axis] + signedStepSize;
//                     writing
//                    if (write(_fd[axis], &_out[axis], sizeof(_out[axis])) < 0){
//                        _mutex.post();
//                        return false;
//                    }
//                    yarp::os::Time::delay(0.001);
//                    cout << "_pos: " << _pos[axis] << "  required: " << position << endl;
//                }
//            }
//            else {
//                while( _pos[axis] > (position + tolerance)) {   // _pos is updated by position update thread
//                    _out[axis].ch[0].x = _out[axis].ch[axis].x = _out[axis].ch[2].x = _out[axis].ch[3].x  = _pos[axis] + signedStepSize;
//                     writing
//                    if (write(_fd[axis], &_out[axis], sizeof(_out[axis])) < 0){
//                        _mutex.post();
//                        return false;
//                    }
//                    yarp::os::Time::delay(0.001);
//                    cout << "_pos: " << _pos[axis] << "  required: " << position << endl;
//                }
//            }
//            _mutex.post();
//    
//        }
//    }
//}

// to be used only when in position mode
// This function does not return until the position is reached!
// This function relies on encoder value update thread having enough time to update encoder value! If this is not the case
// the move might last too long! If position update thread cannot update position fast enough it prints a warning.
// if something bad happens, the motor might keep running in velocity mode!!!
//bool yarp::dev::UrbtcControl::velocityMoveClose(int axis, long position, unsigned short speed, long tolerance){
//
//    if (axis > -1 && axis < _intNumControllers){
//        if ( (abs(position - _pos[axis])) > tolerance){
//
//            int   msSleep = 10;
//            short signedSpeed = speed;
//
//            // backup current pid values
//            double *kp = new double[_intNumControllers];
//            double *ki = new double[_intNumControllers];
//            double *kd = new double[_intNumControllers];
//            for (int i = 0; i < _intNumControllers; i++) {
//                kp[i] = ((double)_out[i].ch[0].kp)/((double)_out[i].ch[0].kpx);
//                ki[i] = ((double)_out[i].ch[0].ki)/((double)_out[i].ch[0].kix);
//                kd[i] = ((double)_out[i].ch[0].kd)/((double)_out[i].ch[0].kdx);
//            }
//    
//            
//            
//            setVelocityMode();
//            
//            // check direction, change if negative speed required
//            if ( (position - _pos[axis]) < 0)
//                signedSpeed = -signedSpeed;
//            
//
//            // start moving at velocity speed
//            _mutex.wait();
//            _out[axis].ch[0].d = _out[axis].ch[axis].d = _out[axis].ch[2].d = _out[axis].ch[3].d  = signedSpeed; 
//            // writing
//            if (write(_fd[axis], &_out[axis], sizeof(_out[axis])) < 0){
//                return false;
//            }
//            _mutex.post();
//    
//            // let the motor run
//            if (signedSpeed > 0){
//                while( _pos[axis] < (position - tolerance)) {   // _pos is updated by position update thread
//                    usleep(msSleep);
//                    //cout << "_pos: " << _pos[axis] << "  required: " << position << endl;
//                }
//            }
//            else {
//                while( _pos[axis] > (position + tolerance)) {   // _pos is updated by position update thread
//                    usleep(msSleep);
//                    //cout << "_pos: " << _pos[axis] << "  required: " << position << endl;
//                }
//            }
//    
//            // we are now in tolerance area, stop motor
//            stopRaw(axis);
//    
//            // set back to position mode
//            setPositionMode();
//    
//            // reset backed up pid values (this will be written to controller next time when setting any positioning)
//            for (int j = 0; j < _intNumControllers; j++){
//                for (int i = 0; i < 4; i++){
//                    _out[j].ch[i].kp = (signed short)(kp[j] * _pidAccuracy);
//                    _out[j].ch[i].kpx = (unsigned short) _pidAccuracy;        // P gain  = 5/1 = 5  
//                    _out[j].ch[i].kd = (signed short)(kd[j] * _pidAccuracy);         
//                    _out[j].ch[i].kdx =(unsigned short) _pidAccuracy;        // D gain = 2/1 = 2   
//                    _out[j].ch[i].ki = (signed short)(ki[j] * _pidAccuracy);         
//                    _out[j].ch[i].kix = (unsigned short)_pidAccuracy;        // I gain = 0/1 = 0   
//                }
//            }
//            return true;
//        }
//    }
//    return false;
//
//}



