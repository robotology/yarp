refactor_doc_libYARP_dev {#yarp-3.8}
-------------------

### libYARP_dev interfaces doxygen tags

#### `yarp::dev`

- Added missing doxygen tags to:
  - IAudioRender (`dev_iface_media`)
  - ICalibrator (`dev_iface_motor`)
  - IFrameGrabberControlsDC1394 (`dev_iface_media`)
  - IFrameGrabberOf (`dev_iface_media`)
  - IHapticDevice (`dev_iface_other`)
  - IRemoteCalibrator (`dev_iface_motor`)
  - IVisualServoing (`dev_iface_other`)
- Created new `raw` tag inside `dev_iface_motor` and `dev_iface_other` to separate raw interfaces from the others.
- Interfaces affected by this change (for some of them the tag was added since it was missing and for the other ones it was changes to the `raw` *sub-tag*):
  - IAmplifierControlRaw (`dev_iface_motor_raw` - added)
  - IAxisInfoRaw (`dev_iface_motor_raw` - added)
  - IControlCalibrationRaw (`dev_iface_motor_raw` - added)
  - IControlLimitsRaw (`dev_iface_motor_raw` - added)
  - IControlModeRaw (`dev_iface_motor_raw` - changed from `dev_iface_motor`)
  - ICurrentControlRaw (`dev_iface_motor_raw` - added)
  - IEncodersRaw (`dev_iface_motor_raw` - changed from `dev_iface_motor`)
  - IEncodersTimedRaw (`dev_iface_motor_raw` - changed from `dev_iface_motor`)
  - IInteractionModeRaw (`dev_iface_motor_raw` - changed from `dev_iface_motor`)
  - IJointFaultRaw (`dev_iface_motor_raw` - changed from `dev_iface_motor`)
  - IMotorRaw (`dev_iface_motor_raw` - changed from `dev_iface_motor`)
  - IMotorEncodersRaw (`dev_iface_motor_raw` - changed from `dev_iface_motor`)
  - IPWMControlRaw (`dev_iface_motor_raw` - added)
  - IPidControlRaw (`dev_iface_motor_raw` - added)
  - IPositionControlRaw (`dev_iface_motor_raw` - changed from `dev_iface_motor`)
  - IPositionDirectRaw (`dev_iface_motor_raw` - changed from `dev_iface_motor`)
  - IRemoteVariablesRaw (`dev_iface_motor_raw` - changed from `dev_iface_motor`)
  - ITorqueControlRaw (`dev_iface_motor_raw` - changed from `dev_iface_motor`)
  - IVelocityControlRaw (`dev_iface_motor_raw` - changed from `dev_iface_motor`)
  - IVirtualAnalogSensorRaw (`dev_iface_other_raw` - changed from `dev_iface_other`)
