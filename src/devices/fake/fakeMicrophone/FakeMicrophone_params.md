* |  | period            | double  | s              |  0.010                   | No                          | the period of processing thread                                   | A value of 10ms is recommended. Do to not modify it |
* |  | waveform          | string  | -              | sine                     | No                          | Defines the shape of the waveform. Can be one of the following: sine,sawtooth,square,constant | - |
* |  | signal_frequency  | int     | Hz             | 440                      | No                          | Frequency of the generated signal | - |
* |  | signal_amplitude  | int     |                | 32000                    | No                          | Amplitude of the generated signal | - |
* |  | driver_frame_size | int     | samples        |  512                     | No                          | the number of samples to process on each iteration of the thread  | - |
