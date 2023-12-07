/*
 * SPDX-FileCopyrightText: 2023-2023 Istituto Italiano di Tecnologia (IIT)
 * SPDX-License-Identifier: BSD-3-Clause
 */

//-------------------------------------------------
//    bool send(const Bottle& msg) override;
//    bool send(char *msg, size_t size) override;
//    bool receive(Bottle& msg) override;
//    int receiveChar(char& c) override;
//
//    int receiveLine(char* line, const int MaxLineLength) override;
//    int receiveBytes(unsigned char* bytes, const int size) override;
//-------------------------------------------------

service ISerialMsgs
{
    bool setDTR (1:bool enable);
    i32 flush ();
}
