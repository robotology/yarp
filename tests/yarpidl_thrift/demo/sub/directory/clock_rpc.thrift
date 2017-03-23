namespace yarp testing

service ClockServer {
    oneway void pauseSimulation();
    oneway void continueSimulation();
    oneway void stepSimulation(1:i32 numberOfSteps = 1);
}
