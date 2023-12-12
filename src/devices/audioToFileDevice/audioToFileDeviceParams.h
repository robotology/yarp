/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <yarp/os/Searchable.h>
#include <yarp/dev/IDeviceDriverParams.h>
#include <string>

 /**
 * @ingroup dev_impl_media
 *
 * Parameters required by this device are:
 * | Parameter name | SubParameter   | Type    | Units          | Default Value            | Required                    | Description                                                       | Notes |
 * |:--------------:|:--------------:|:-------:|:--------------:|:------------------------:|:--------------------------: |:-----------------------------------------------------------------:|:-----:|
 * | file_name      |      -         | string  | -              |  audio_out.wav           | No                          | The name of the file written by the module                        | Only .wav and .mp3 files are supported   |
 * | save_mode      |      -         | string  | -              |  overwrite_file          | No                          | Affects the behavior of the module and defines the save mode, as described in the documentation.   |       |
 * | add_marker     |      -         | bool    | -              |  -                       | No                          | If set, it will add a marker at the beginning and at the ending of each received waveform.   |       |
 *
 * Three different operating modes are available, defined by the optional string parameter `save_mode`:
 * if save_mode == "append_data", the file is written only when the module terminates.
 * Every start/stop operation just pauses the module. On resume, the new data is concatenated at the end of the file.
 *
 * if save_mode == "overwrite_file", the output file is written every time the stop() method is called or when the module terminates.
 * If the file already exists, it will be overwritten with the new data.
 *
 * if save_mode = "rename_file", the output file is written to a NEW file every time the stop() method is called or when the module terminates.
 * The file name is modified, using an incremental counter appended at the end of the file name.
 *
 * if save_mode = "break_file", the output file is written to a NEW file every time a yarp::sig::sound is received or when the module terminates.
 * The file name is modified, using an incremental counter appended at the end of the file name.
 *
 */
class audioToFileDevice_params : public yarp::dev::IDeviceDriverParams
{
public:
    ~audioToFileDevice_params() override = default;

private:
    std::string      m_device_type = "audioToFileDevice";

public:
    bool             m_add_marker = false;
    std::string      m_audio_filename = "audio_out.wav";
    std::string      m_save_mode_s = "overwrite_file";

public:
    bool          parseParams(const yarp::os::Searchable& config) override;
    std::string   getDeviceType() const override { return m_device_type; }
    std::string   getDocumentationOfDeviceParams()  const override;
    std::vector<std::string> getListOfParams() const override;
};
